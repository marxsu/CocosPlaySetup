#include "dlgsetup.h"
#include "ui_dlgsetup.h"
#include "QMessageBox"
#include "QDir"
#include "QFileDialog"
#include "QProcess"
#include "QStandardPaths"
#include "QDebug"
#include "QMovie"
#include "QTextCodec"
#include "noticetip.h"

#define PROGRAME_EXE "/CocosPlay.exe"

BUILD_DLG(DlgSetup)
DlgSetup::DlgSetup(QWidget *parent) :
    CCtrlDialog(parent),
    ui(new Ui::DlgSetup)
{
    ui->setupUi(this);



#ifdef Q_OS_WIN
    QRectF rcShow = ui->imgBg->geometry();
    QRectF rcNine = QRectF(30, 30, 30, 30);
    ui->imgBg->init(":/dlgFrame", rcShow, rcNine);
#endif

    QMovie* movie = new QMovie(":/gifInstalling");
    ui->gifInstalling->setMovie(movie);
    movie->setParent(ui->gifInstalling);
    movie->start();

    ui->hFrameProgress->setVisible(false);
    ui->btnFinish->setVisible(false);
    ui->hFrameShowFinish->setVisible(false);

    QFileInfoList lsFile = QDir::drives();
    foreach (QFileInfo file , lsFile)
    {
        QString driver = file.absolutePath();
        if(driver == "D:/")
        {
            ui->lineEditFile->setText("D:/CocosPlay");
            break;
        }
    }

    m_pPythonThread = NULL;

    m_unZipPythonThread = NULL;

    m_nTimerUnzip = -1;
    m_nTimerProgress = -1;
    m_nCurProgress = 0;

    setProgress(m_nCurProgress);

    m_nSize = 100;

    ui->btnSetup->setFocus();


}

DlgSetup::~DlgSetup()
{
    delete ui;
}

void DlgSetup::startSetup()
{

    m_strTargetDir = ui->lineEditFile->text();
    QString runPath = QCoreApplication::applicationDirPath();

//    if(m_nTimerUnzip != -1)
//    {
//        killTimer(m_nTimerUnzip);
//    }
//    m_nTimerUnzip = startTimer(1000);

    /*
    if(m_nTimerProgress != -1)
    {
        killTimer(m_nTimerProgress);
    }
    m_nTimerProgress = startTimer(200);
    */

    QVariantMap map;
    QString src = runPath + "/Env.zip";
    map["src"] = src;
    map["dest"] = m_strTargetDir;
    if(!m_pPythonThread)
    {
        m_pPythonThread = new PythonToolThread();
    }
    m_nSize = m_pPythonThread->getUnzipFileSize(src);
    if(!m_pPythonThread->bExecTask())
    {
        m_pPythonThread->terminate();
        m_pPythonThread->StartOperate(PYTHON_TOOL_UNZIP, map);
        m_pPythonThread->start();
        m_pPythonThread->disconnect();
        connect(m_pPythonThread, SIGNAL(finished()), this, SLOT(unZipPython()));
    }

}

void DlgSetup::setProgress(int iValue)
{
    //ui->progressBar->setValue(iValue);
}

void DlgSetup::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();
    if(timerId == m_nTimerUnzip)
    {
        qint64 size = getDirSize(m_strTargetDir);
        if(m_nSize == 0)
            return;
        if(size/m_nSize * 100 > 100)
            m_nCurProgress = 100;


        m_nCurProgress = size/m_nSize * 100;
    }
    else if(timerId == m_nTimerProgress)
    {
        /*
        int iValue = ui->progressBar->value();
        if(iValue < m_nCurProgress)
        {
            float fSpeed = (float)(m_nCurProgress - iValue)/20.0;
            if(fSpeed)
            {
                fSpeed =  fSpeed<1.0?1.0:fSpeed;
            }

            iValue += fSpeed;
            iValue = iValue > 100?100:iValue;
            setProgress(iValue);

            if(iValue == 100)
            {
                if(m_nTimerProgress != -1)
                {
                    killTimer(m_nTimerProgress);
                    m_nTimerProgress = -1;
                }
                ui->hFrameProgress->setVisible(false);
                ui->btnFinish->setVisible(true);
            }
        }
        */
    }

}

qint64 DlgSetup::getDirSize(QString dest)
{
    int size = 0;
    QDir dir(dest);
    QFileInfoList lsFile = dir.entryInfoList();

    for(int idx = 0; idx < lsFile.count(); idx++)
    {
        QFileInfo fileInf = lsFile[idx];
        QString filename = fileInf.fileName();
        if(filename == "." || filename == "..")
        {
            continue;
        }
        if(fileInf.isFile())
        {
            size += fileInf.size();
        }
        else if(fileInf.isDir())
        {
            size += getDirSize(fileInf.absoluteFilePath());
        }
    }
    return size;
}

void DlgSetup::produceShortCut()
{
    QString runPath = QCoreApplication::applicationDirPath();
    m_strTargetDir = ui->lineEditFile->text();
    m_strTargetDir = m_strTargetDir + "/CocosPlay";

    QString workDir = m_strTargetDir + "/";
    //modify vbs
    QFile file(runPath + "/shortcut.vbs");
    file.open(QIODevice::ReadWrite);
    QByteArray aryContent = file.readAll();
    QString strContent(aryContent);
    int idxStart = strContent.indexOf("workDir=");
    int idxEnd = strContent.indexOf("'###", idxStart);
    if(idxStart != -1 && idxEnd != -1)
        strContent.replace(idxStart + 8, idxEnd - idxStart -8, "\""+ workDir + "\"");

    idxStart = strContent.indexOf("lnkFile=");
    idxEnd = strContent.indexOf("'@@@", idxStart);
    if(idxStart != -1 && idxEnd != -1)
        strContent.replace(idxStart + 8, idxEnd - idxStart -8, "\""+ runPath + "/CocosPlay.lnk\"");

    file.resize(0);
    file.write(strContent.toLocal8Bit().data());
    file.close();


    //produce shortcut
    QString cmdShortCut = "cscript " + runPath + "/shortcut.vbs";
    QProcess pro;
    pro.start(cmdShortCut);
    bool bFinish = pro.waitForFinished();

    //copy shortcut to desktop

    if(bFinish)
    {
        QString lnk = runPath + "/CocosPlay.lnk";
        QString destShortcut = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QFile::remove(destShortcut + "/CocosPlay.lnk");
        QFile::copy(lnk, destShortcut + "/CocosPlay.lnk");
    }


}

void DlgSetup::setSysEnv()
{
    QString runPath = QCoreApplication::applicationDirPath();
    QString str = ui->lineEditFile->text();
    QString m_strTargetDir;
    if('/' == str.at(str.size() - 1))
    {
       str.replace("/","");
    }
    m_strTargetDir = str + "/CocosPlay";

    m_strTargetDir.replace("/","\\");
    QString workDir = m_strTargetDir + "\\PythonSDK";
    //modify vbs
    QFile file(runPath + "/sysenv.vbs");
    file.open(QIODevice::ReadWrite);
    QByteArray aryContent = file.readAll();
    QString strContent(aryContent);
    int idxStart = strContent.indexOf("path=");
    int idxEnd = strContent.indexOf("'###", idxStart);
    if(idxStart != -1 && idxEnd != -1)
        strContent.replace(idxStart + 5, idxEnd - idxStart -5, "\""+ workDir + "\"");
    file.resize(0);
    file.write(strContent.toLocal8Bit().data());
    file.close();


    //produce shortcut
    QString cmdShortCut = "cscript " + runPath + "/sysenv.vbs";
    QProcess pro;
    pro.start(cmdShortCut);
    pro.waitForFinished();

}

void DlgSetup::installFinish()
{
    ui->hFrameShowFinish->setVisible(true);
    ui->btnFinish->setVisible(false);
    ui->hFrameProgress->setVisible(false);
    ui->btnFinish->setFocus();


    NoticeTip* tip = new NoticeTip(this);
    tip->setAttribute(Qt::WA_DeleteOnClose);
    tip->setNoticeText(QTextCodec::codecForName("GBK")->toUnicode("安装成功！双击桌面图标启动。"));
    tip->exec();


    this->close();
}

void DlgSetup::produceNewMenu()
{
    QString runPath = QCoreApplication::applicationDirPath();
    QString lnk = runPath + "/CocosPlay.lnk";
    if(QFile::exists(lnk))
    {
        QString appShortcut = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/CocosPlay";
        QDir appDir(appShortcut);
        if(!appDir.exists())
        {
            appDir.mkpath(appShortcut);
        }
        appShortcut = appShortcut + "/CocosPlay.lnk";
        if(QFile::exists(appShortcut))
        {
            QFile::remove(appShortcut);
        }

        QFile::copy(lnk, appShortcut);
    }

}

void DlgSetup::copyDLL()
{
    QString runPath = QCoreApplication::applicationDirPath();
    QDir dir(runPath);
    QFileInfoList lsInfo = dir.entryInfoList();
    foreach(QFileInfo info, lsInfo)
    {
        if(info.suffix() == "dll")
        {
            QFile::copy(info.absoluteFilePath(), m_strTargetDir + "/" + info.fileName());
        }
    }

    //copy python27.zip
    QFile::copy(runPath + "/python27.zip", m_strTargetDir + "/python27.zip");

    //copy dir(imageformats platforms sqldrivers)
    QDir dirImages(runPath + "/imageformats");
    QDir targetDirImg(m_strTargetDir + "/imageformats");
    if(dirImages.exists())
    {
        copyDirectory(dirImages, targetDirImg);
    }

    QDir dirplatforms(runPath + "/platforms");
    QDir targetDirplatforms(m_strTargetDir + "/platforms");
    if(dirplatforms.exists())
    {
        copyDirectory(dirplatforms, targetDirplatforms);
    }

    QDir dirsqldrivers(runPath + "/sqldrivers");
    QDir targetDirsqldrivers(m_strTargetDir + "/sqldrivers");
    if(dirsqldrivers.exists())
    {
        copyDirectory(dirsqldrivers, targetDirsqldrivers);
    }
}

void DlgSetup::produceUninstall()
{
    QString runPath = QCoreApplication::applicationDirPath();
    QString uninstallFile = runPath + "/uninstall.bat";
    QFile::copy(uninstallFile, m_strTargetDir + "/uninstall.bat");
}

bool DlgSetup::copyDirectory(const QDir &fromDir, const QDir &toDir, bool bCoverIfFileExists)
{
    QDir fromDir_ = fromDir;
    QDir toDir_ = toDir;

    if(!toDir_.exists())
    {
        if(!toDir_.mkdir(toDir.absolutePath()))
            return false;
    }

    //foreach(QFileInfo fileInfo, fileInfoList)

    QFileInfoList fileInfoList = fromDir_.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        //(拷贝子目录)
        if(fileInfo.isDir())
        {
            //(递归调用拷贝)
            if(!copyDirectory(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName())))
                return false;
        }
        //(拷贝子文件)
        else
        {
            if(bCoverIfFileExists && toDir_.exists(fileInfo.fileName()))
            {
                toDir_.remove(fileInfo.fileName());
            }
            if(!QFile::copy(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName())))

            {
                return false;
            }
        }
    }
    return true;
}

void DlgSetup::on_btnSetup_clicked()
{
    QString txtPath = ui->lineEditFile->text();
    txtPath = txtPath.trimmed();
    ui->lineEditFile->setText(txtPath);
//    if(txtPath.indexOf(" ") != -1)
//    {
//        ui->lineEditFile->setText("");
//        QString str = QTextCodec::codecForName("GBK")->toUnicode("路径不能有空格!");
//        ui->lineEditFile->setPlaceholderText(str);
//        QString style = "QLineEdit{border:1px solid rgb(217,217,217);color:rgb(255,0,0);background-color:rgb(255,255,255);}QLineEdit:hover{border:1px solid rgb(131,245,253);}";
//        ui->lineEditFile->setStyleSheet(style);

//        return;
//    }
    QDir dir(txtPath);
    if(txtPath == "" || !dir.isAbsolute() || (!dir.exists() && !dir.mkpath(dir.absolutePath())))
    {
        ui->lineEditFile->setText("");
        ui->lineEditFile->setPlaceholderText(QTextCodec::codecForName("GBK")->toUnicode("路径有误!"));
        QString style = "QLineEdit{border:1px solid rgb(217,217,217);color:rgb(255,0,0);background-color:rgb(255,255,255);}QLineEdit:hover{border:1px solid rgb(131,245,253);}";
        ui->lineEditFile->setStyleSheet(style);

        return;
    }


    ui->btnSetup->setVisible(false);
    ui->hFrameProgress->setVisible(true);
    ui->hFrameShowFinish->setVisible(false);

    ui->lineEditFile->setReadOnly(true);
    ui->btnbrowse->setEnabled(false);

    startSetup();
}

void DlgSetup::on_btnClose_clicked()
{
    this->close();
}

void DlgSetup::on_btnMinimize_clicked()
{
    QStringList lsStr = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
    qDebug()<<lsStr;

    return;
    this->showMinimized();
}

void DlgSetup::on_btnbrowse_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select setup directory."), ".");
    if(path.length())
    {
        ui->lineEditFile->setText(path);

        QString style = "QLineEdit{border:1px solid rgb(217,217,217);color:rgb(0,0,0);background-color:rgb(255,255,255);}QLineEdit:hover{border:1px solid rgb(131,245,253);}";
        ui->lineEditFile->setStyleSheet(style);
    }
}

void DlgSetup::on_lineEditFile_textChanged(const QString &arg1)
{
    QString style = "QLineEdit{border:1px solid rgb(217,217,217);color:rgb(0,0,0);background-color:rgb(255,255,255);}QLineEdit:hover{border:1px solid rgb(131,245,253);}";
    ui->lineEditFile->setStyleSheet(style);
}

void DlgSetup::unZipFinish()
{
    //解压完成
    if(m_nTimerUnzip != -1)
    {
        killTimer(m_nTimerUnzip);
        m_nTimerUnzip = -1;
    }
    m_nCurProgress = 100;

    produceShortCut();
    setSysEnv();
    copyDLL();
    produceNewMenu();
    produceUninstall();
    installFinish();
}
void DlgSetup::unZipPython()
{
    m_strTargetDir = ui->lineEditFile->text();
    QString runPath = QCoreApplication::applicationDirPath();

    QVariantMap map;
    QString src = runPath + "/Python27-for-win32.zip";
    map["src"] = src;
    map["dest"] = m_strTargetDir + "/CocosPlay";
    if(!m_unZipPythonThread)
    {
        m_unZipPythonThread = new PythonToolThread();
    }
    m_nSize = m_unZipPythonThread->getUnzipFileSize(src);
    if(!m_unZipPythonThread->bExecTask())
    {
        m_unZipPythonThread->terminate();
        m_unZipPythonThread->StartOperate(PYTHON_TOOL_UNZIP, map);
        m_unZipPythonThread->start();
        m_unZipPythonThread->disconnect();
        connect(m_unZipPythonThread, SIGNAL(finished()), this, SLOT(unZipFinish()));
    }
}

void DlgSetup::on_btnFinish_clicked()
{
    QString exec = "\"" + m_strTargetDir + PROGRAME_EXE + "\"";
    QProcess::startDetached(exec);
    this->close();
}

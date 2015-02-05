#ifndef DLGSETUP_H
#define DLGSETUP_H

#include <QDialog>
#include "cctrldialog.h"
#include "pythontoolthread.h"
#include "QDir"

namespace Ui {
class DlgSetup;
}

class DlgSetup : public CCtrlDialog
{
    Q_OBJECT

    DECLARE_DLG(DlgSetup)

public:
    explicit DlgSetup(QWidget *parent = 0);
    ~DlgSetup();

    void startSetup();

    void setProgress(int iValue);



protected:
    void timerEvent(QTimerEvent *event);

    qint64 getDirSize(QString dest);

    void produceShortCut();

    void setSysEnv();

    void installFinish();
    void produceNewMenu();
    void copyDLL();
    void produceUninstall();

    bool copyDirectory(const QDir &fromDir, const QDir &toDir, bool bCoverIfFileExists = true);

private slots:
    void on_btnSetup_clicked();

    void on_btnClose_clicked();

    void on_btnMinimize_clicked();

    void on_btnbrowse_clicked();

    void on_lineEditFile_textChanged(const QString &arg1);

    void unZipFinish();

    void unZipPython();

    void on_btnFinish_clicked();

private:
    Ui::DlgSetup *ui;

    PythonToolThread* m_pPythonThread;

    PythonToolThread* m_unZipPythonThread;

    int m_nTimerUnzip;
    int m_nTimerProgress;

    int m_nCurProgress;

    QString m_strTargetDir;

    qint64 m_nSize;
};

#endif // DLGSETUP_H

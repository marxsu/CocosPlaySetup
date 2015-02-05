#include "pythontoolthread.h"
#include "QCoreApplication"
#include <stdlib.h>
#include "pythreadstatelock.h"
#include "QCoreApplication"

PythonToolThread::PythonToolThread(QObject *parent) :
    QThread(parent)
{
    m_bExecTask = false;
}


void PythonToolThread::run()
{
    // (初始化python环境,第一次初始化可能会失败)
    if(!Py_IsInitialized())
    {
        Py_Initialize();
        if(!Py_IsInitialized())
            return ;
        // (初始化线程支持)
        PyEval_InitThreads();
        // 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
        PyEval_ReleaseThread(PyThreadState_Get());
    }

    switch(m_toolOperate)
    {
    case PYTHON_TOOL_UNZIP:
        {
            QVariantMap::Iterator iterSrc,iterDest;
            iterSrc = m_dictParam.find("src");
            iterDest = m_dictParam.find("dest");
            if(iterSrc == m_dictParam.end() || iterDest == m_dictParam.end())
            {
                m_bExecTask = false;
                return;
            }

            unzip(iterSrc.value().toString(), iterDest.value().toString());
        }
        break;
    case PYTHON_TOOL_ZIPFILE:
        {
            QVariantMap::Iterator iterSrc,iterDest,iterApkfile;
            iterSrc = m_dictParam.find("src");
            iterDest = m_dictParam.find("dest");
            iterApkfile = m_dictParam.find("apkfile");
            if(iterSrc == m_dictParam.end() || iterDest == m_dictParam.end() ||iterApkfile == m_dictParam.end() )
            {
                m_bExecTask = false;
                return;
            }
            zip(iterSrc.value().toString(), iterDest.value().toString(), iterApkfile.value().toString());
        }
        break;
    }

    m_bExecTask = false;

    this->exit();
}

bool PythonToolThread::StartOperate(PYTHON_TOOL_OPERATE operate, const QVariantMap &dictParam)
{
    if(m_bExecTask)
        return false;

    m_bExecTask = true;

    m_toolOperate = operate;

    m_dictParam.clear();
    m_dictParam = dictParam;
}

bool PythonToolThread::bExecTask()
{
    return m_bExecTask;
}

void PythonToolThread::unzip(QString src, QString dest)
{
    zipFunction("unZip", src, dest, "NULL");
}

void PythonToolThread::zip(QString src, QString dest, QString apkFile)
{
    zipFunction("zip", src, dest, apkFile);
}

void PythonToolThread::zipFunction(const char *szFunction, QString src, QString dest, QString apkFile)
{
    class PyThreadStateLock PyThreadLock;


    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");

    QString runPath = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    QString importPath = "sys.path.append(str('" + runPath + "'))";
#endif

#ifdef Q_OS_WIN
    QString importPath = "sys.path.append(str('" + runPath + "').encode('gbk'))";
#endif

    std::string str = importPath.toStdString();
    const char* szImport = str.c_str();
    PyRun_SimpleString("reload(sys);sys.setdefaultencoding('utf8');");
    PyRun_SimpleString(szImport);

    PyObject * pModule = NULL; //声明变量
    PyObject * pFunc = NULL;       //声明变量
    pModule =PyImport_ImportModule("pytoolForQt");  //这里是要调用的文件名

    if(pModule)
        pFunc= PyObject_GetAttrString(pModule, szFunction); //这里是要调用的函数名
    else
    {
        //qDebug()<<"Error:Not Found The File of pytoolForQt.py";
    }


    if(pFunc)
    {
        PyObject* pArgs = PyTuple_New(3);
        std::string str = src.toStdString();
        const char* szSrc = str.c_str();
        PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", szSrc));
        str = dest.toStdString();
        const char* szDest = str.c_str();
        PyTuple_SetItem(pArgs, 1, Py_BuildValue("s", szDest));
        PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", apkFile.toStdString().c_str()));

        PyEval_CallObject(pFunc, pArgs);            //调用函数
        Py_DECREF(pArgs);
    }
//    else
//        qDebug()<<"Error:Not Found Function";

    if(!pFunc)
    {
        Py_DECREF(pFunc);
    }
    if(!pModule)
    {
        Py_DECREF(pModule);
    }
}

qint64 PythonToolThread::getUnzipFileSize(QString src)
{
    qint64 unzipSize = 0;
    // (初始化python环境,第一次初始化可能会失败)
    if(!Py_IsInitialized())
    {
        Py_Initialize();
        if(!Py_IsInitialized())
            return 0;
        // (初始化线程支持)
        PyEval_InitThreads();
        // 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
        PyEval_ReleaseThread(PyThreadState_Get());
    }

    class PyThreadStateLock PyThreadLock;


    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");

    QString runPath = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    QString importPath = "sys.path.append(str('" + runPath + "'))";
#endif

#ifdef Q_OS_WIN
    QString importPath = "sys.path.append(str('" + runPath + "').encode('gbk'))";
#endif

    std::string str = importPath.toStdString();
    const char* szImport = str.c_str();
    PyRun_SimpleString("reload(sys);sys.setdefaultencoding('utf8');");
    PyRun_SimpleString(szImport);

    PyObject * pModule = NULL; //声明变量
    PyObject * pFunc = NULL;       //声明变量
    pModule =PyImport_ImportModule("pytoolForQt");  //这里是要调用的文件名

    if(pModule)
        pFunc= PyObject_GetAttrString(pModule, "getUnzipFileSize"); //这里是要调用的函数名
    else
    {
        //qDebug()<<"Error:Not Found The File of pytoolForQt.py";
    }


    if(pFunc)
    {
        PyObject* pArgs = PyTuple_New(1);
        std::string str = src.toStdString();
        const char* szSrc = str.c_str();
        PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", szSrc));

        PyObject * pRet = PyEval_CallObject(pFunc, pArgs);            //调用函数
        if(pRet)
        {
           PyArg_Parse(pRet, "l", &unzipSize);
           Py_DECREF(pRet);
        }
        Py_DECREF(pArgs);
    }
//    else
//        qDebug()<<"Error:Not Found Function";

    if(!pFunc)
    {
        Py_DECREF(pFunc);
    }
    if(!pModule)
    {
        Py_DECREF(pModule);
    }
    return unzipSize;
}

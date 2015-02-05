#ifndef PYTHONTOOLTHREAD_H
#define PYTHONTOOLTHREAD_H

#include <QThread>
#include "QVariantMap"

#define Py_NO_ENABLE_SHARED
#include "Python.h"


enum PYTHON_TOOL_OPERATE{
    PYTHON_TOOL_ZIPFILE = 0,//(参数需提供src,dest)
    PYTHON_TOOL_UNZIP,      //(参数需提供src,dest)
};

class PythonToolThread : public QThread
{
    Q_OBJECT
public:
    explicit PythonToolThread(QObject *parent = 0);

    virtual void run();

    //(根据操作，提供不同参数)
    bool StartOperate(PYTHON_TOOL_OPERATE operate,const QVariantMap& dictParam);

    bool bExecTask();

    qint64 getUnzipFileSize(QString src);

protected:
    void unzip(QString src, QString dest);
    void zip(QString src, QString dest, QString apkFile);

    void zipFunction(const char* szFunction, QString src, QString dest, QString apkFile);


signals:

public slots:

private:
    PYTHON_TOOL_OPERATE m_toolOperate;

    //(是否正在执行任务)
    bool m_bExecTask;

    QVariantMap m_dictParam;

};

#endif // PYTHONTHREAD_H

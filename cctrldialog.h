#ifndef CCTRLDIALOG_H
#define CCTRLDIALOG_H

#include <QDialog>

class CCtrlDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CCtrlDialog(QWidget *parent = 0);



protected:
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);

    virtual void paintEvent(QPaintEvent *event);

private:
    bool m_bPress;
    QPoint m_ptPress;


signals:

public slots:

};

//#define DECLARE_DLG(ClassName) \
//    public:\
//        static ClassName* shareInstance()\
//        {\
//            static ClassName singleInstance;\
//            return &singleInstance;\
//        }

#define DECLARE_DLG(ClassName) \
    private:\
        static ClassName* singleInstance;\
    public:\
        static ClassName* shareInstance()\
        {\
            if(NULL == singleInstance) singleInstance = new ClassName();\
            return singleInstance;\
        }\
    private:\
        class CGarbo\
        {\
            public:\
            ~CGarbo()\
            {if( ClassName::singleInstance )delete ClassName::singleInstance;}\
        };\
        static CGarbo Garbo;

#define BUILD_DLG(ClassName)\
    ClassName* ClassName::singleInstance = NULL;


#endif // CCTRLDIALOG_H

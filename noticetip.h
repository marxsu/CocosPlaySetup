#ifndef NOTICETIP_H
#define NOTICETIP_H

#include <QDialog>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QString>

class NoticeTip : public QDialog
{
    Q_OBJECT
public:
    explicit NoticeTip(QWidget *parent = 0);

    ~NoticeTip();

    void setNoticeText(QString& tmpText);

private:

    void initUi();

protected:

    virtual void timerEvent(QTimerEvent*);

    virtual void paintEvent(QPaintEvent*);

private:

    QString noticeText;

signals:

public slots:


};


#endif // NOTICETIP_H

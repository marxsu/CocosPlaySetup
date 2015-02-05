#include "cctrldialog.h"
#include "QMouseEvent"
#include "QPainter"

CCtrlDialog::CCtrlDialog(QWidget *parent) :
    QDialog(parent)
{
    m_bPress = false;

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
}

void CCtrlDialog::mousePressEvent(QMouseEvent *event)
{
    m_bPress = true;
    m_ptPress = event->globalPos() - this->pos();
    return QDialog::mousePressEvent(event);
}

void CCtrlDialog::mouseReleaseEvent(QMouseEvent *event)
{
    m_bPress = false;
    return QDialog::mouseReleaseEvent(event);
}

void CCtrlDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bPress)
    {
        QPoint posMove = event->globalPos() - m_ptPress;
        move(posMove);
    }
    return QDialog::mouseMoveEvent(event);
}

void CCtrlDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QDialog::paintEvent(event);
}

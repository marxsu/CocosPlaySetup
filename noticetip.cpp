#include "noticetip.h"

#include <QPainter>
#include <QPen>


NoticeTip::NoticeTip(QWidget *parent) :
    QDialog(parent)
{
    initUi();
}
NoticeTip::~NoticeTip()
{

}

void NoticeTip::initUi()
{
    setStyleSheet(QStringLiteral("background-color: #111111;"));
    setWindowFlags(Qt::ToolTip);
    setWindowOpacity(0.7);
    startTimer(3000);
    QPoint point = QPoint(parentWidget()->x() + parentWidget()->width()/2 - 150,parentWidget()->y() + parentWidget()->height()/2);
    setGeometry(point.x(),point.y(),330,40);
}

void NoticeTip::timerEvent(QTimerEvent *)
{
    this->close();
}

void NoticeTip::setNoticeText(QString &tmpText)
{
    noticeText = tmpText;
}

void NoticeTip::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::white);
    painter.setPen(pen);
    QFont font;
    font.setWeight(QFont::Black);
    font.setPointSize(12);
    painter.setFont(font);
    painter.drawText(50,20,noticeText);
}

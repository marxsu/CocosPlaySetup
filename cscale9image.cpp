#include "cscale9image.h"
#include "QPainter"
#include "QDebug"


CScale9Image::CScale9Image(QWidget *parent) :
    QLabel(parent)
{
    m_pPixmap = NULL;

    m_bAnimate = false;
    m_nCurrentTime = 0;
    m_nCurrentFrame = 0;

    m_nTimerId = -1;
}

void CScale9Image::init(const QString &filename, QRectF &rectShow, QRectF &rectNine)
{
    if(!m_pPixmap)
    {
        m_pPixmap = new QPixmap();
    }
    m_pPixmap->load(filename);
    m_rcShow = rectShow;
    m_rcNine = rectNine;

    /*
     0 1 2
     7 8 3
     6 5 4
    */
    int centerWidth = m_pPixmap->width() - m_rcNine.left() - m_rcNine.width();
    int centerHeight = m_pPixmap->height() - m_rcNine.top() - m_rcNine.height();
    //Left-Top
    m_img[0] = m_pPixmap->copy(0, 0, m_rcNine.left(), m_rcNine.top());
    //Center-Top
    m_img[1] = m_pPixmap->copy(m_rcNine.left(), 0, centerWidth, m_rcNine.top());
    //Right-Top
    m_img[2] = m_pPixmap->copy(m_rcNine.left()+centerWidth, 0, m_rcNine.width(), m_rcNine.top());
    //Right-Middle
    m_img[3] = m_pPixmap->copy(m_rcNine.left()+centerWidth, m_rcNine.top(), m_rcNine.width(), centerHeight);
    //Right-Bottom
    m_img[4] = m_pPixmap->copy(m_rcNine.left()+centerWidth, m_rcNine.top()+centerHeight, m_rcNine.width(), m_rcNine.height());
    //Center-Middle
    m_img[5] = m_pPixmap->copy(m_rcNine.left(), m_rcNine.top()+centerHeight, centerWidth, m_rcNine.height());
    //Center-Middle
    m_img[6] = m_pPixmap->copy(0, m_rcNine.top()+centerHeight, m_rcNine.left(), m_rcNine.height());
    //Center-Middle
    m_img[7] = m_pPixmap->copy(0, m_rcNine.top(), m_rcNine.left(), centerHeight);
    //Center-Center
    m_img[8] = m_pPixmap->copy(m_rcNine.left(), m_rcNine.top(), centerWidth, centerHeight);

    m_nShowCenterWidth = m_rcShow.width() - m_rcNine.left() - m_rcNine.width();
    m_nShowCenterHeight = m_rcShow.height() - m_rcNine.top() - m_rcNine.height();
}

void CScale9Image::initAnimate(QList<QString> lsImgUrl, float fDuration)
{
    for(int nIndex = 0; nIndex < m_lsPixmap.count(); nIndex++)
    {
        delete m_lsPixmap[nIndex];
    }
    m_lsPixmap.clear();

    for(int nIndex = 0; nIndex < lsImgUrl.count(); nIndex++)
    {
        QPixmap* pix = new QPixmap();
        pix->load(lsImgUrl[nIndex]);
        m_lsPixmap.push_back(pix);
    }

    m_bAnimate = true;
    m_fPerFrameTime = fDuration*(1000.0/30.0)/lsImgUrl.count();



}

void CScale9Image::paintEvent(QPaintEvent *event)
{
    if(m_bAnimate)
    {
        return QLabel::paintEvent(event);
    }
    QPainter painter(this);

    /*
     0 1 2
     7 8 3
     6 5 4
    */
    QRect target(0, 0, m_img[0].width(), m_img[0].height());
    painter.drawPixmap(target, m_img[0]);

    target = QRect(m_img[0].width(), 0, m_nShowCenterWidth, m_img[1].height());
    painter.drawPixmap(target, m_img[1]);

    target = QRect(m_img[0].width() + m_nShowCenterWidth, 0, m_img[2].width(), m_img[2].height());
    painter.drawPixmap(target, m_img[2]);

    target = QRect(m_img[0].width() + m_nShowCenterWidth, m_img[2].height(), m_img[3].width(), m_nShowCenterHeight);
    painter.drawPixmap(target, m_img[3]);

    target = QRect(m_img[0].width() + m_nShowCenterWidth, m_img[2].height() + m_nShowCenterHeight, m_img[4].width(), m_img[4].height());
    painter.drawPixmap(target, m_img[4]);

    target = QRect(m_img[0].width(), m_img[2].height() + m_nShowCenterHeight, m_nShowCenterWidth, m_img[4].height());
    painter.drawPixmap(target, m_img[5]);

    target = QRect(0, m_img[2].height() + m_nShowCenterHeight, m_img[6].width(), m_img[6].height());
    painter.drawPixmap(target, m_img[6]);

    target = QRect(0, m_img[2].height(), m_img[7].width(), m_nShowCenterHeight);
    painter.drawPixmap(target, m_img[7]);

    target = QRect(m_img[0].width(), m_img[2].height(), m_nShowCenterWidth, m_nShowCenterHeight);
    painter.drawPixmap(target, m_img[8]);

    return QLabel::paintEvent(event);

}

void CScale9Image::timerEvent(QTimerEvent *event)
{
    if(m_bAnimate)
    {
       m_nCurrentTime++;
       if(m_nCurrentTime > m_fPerFrameTime)
       {
           m_nCurrentTime = 0;

           if(++m_nCurrentFrame >= m_lsPixmap.count())
           {
                m_nCurrentFrame = 0;
           }

       }

       setPixmap(*m_lsPixmap[m_nCurrentFrame]);
    }
}

bool CScale9Image::event(QEvent *e)
{
    if(m_bAnimate)
    {
        switch(e->type())
        {
        case QEvent::Show:
            {
                if(m_nTimerId != -1)
                {
                    killTimer(m_nTimerId);
                    m_nTimerId = -1;
                }
                m_nTimerId = startTimer(30);
            }
            break;
        case QEvent::Hide:
            {
                if(m_nTimerId != -1)
                {
                    killTimer(m_nTimerId);
                    m_nTimerId = -1;
                }
            }
            break;
        }
    }
    return QLabel::event(e);
}

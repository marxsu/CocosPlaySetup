#ifndef CSALE9IMAGE_H
#define CSALE9IMAGE_H

#include <QLabel>
#include "QEvent"

class CScale9Image : public QLabel
{
    Q_OBJECT
public:
    explicit CScale9Image(QWidget *parent = 0);

    void init(const QString& filename, QRectF& rectShow, QRectF& rectNine);

    void initAnimate(QList<QString> lsImgUrl, float fDuration);

protected:
    virtual void paintEvent(QPaintEvent * event);

    void timerEvent(QTimerEvent *event);


    virtual bool event(QEvent *e);

private:
    QPixmap* m_pPixmap;
    QRectF m_rcShow;
    QRectF m_rcNine;
    QPixmap m_img[9];

    int m_nShowCenterWidth;
    int m_nShowCenterHeight;

    bool m_bAnimate;

    QList<QPixmap*> m_lsPixmap;

    int m_nCurrentFrame;

    float m_fPerFrameTime;
    int m_nCurrentTime;

    int m_nTimerId;

signals:

public slots:

};

#endif // CSALE9IMAGE_H

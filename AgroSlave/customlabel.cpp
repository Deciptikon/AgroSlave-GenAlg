#include "customlabel.h"



CustomLabel::CustomLabel(QWidget *parent) : QLabel(parent)
{
    qDebug() << "CustomLabel created !!!";
    state = StateLabel::PLAY_STATELABEL;

}

void CustomLabel::mousePressEvent( QMouseEvent *event)
{
    QLabel::mousePressEvent(event);
    //qDebug() << "Mouse Press Event !!! (label)";
    emit press(*event);
}

void CustomLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);
    //qDebug() << "Mouse Release Event !!! (label)";
    emit release(*event);
}

void CustomLabel::mouseMoveEvent(QMouseEvent *event)
{
    QLabel::mouseMoveEvent(event);
    //qDebug() << "Mouse Move Event !!! (label)";
    emit move(*event);
}

void CustomLabel::setImage(const QImage &img)
{
    imgWidth = img.width();
    imgHeigth = img.height();
    QImage imgScl = img.scaled(
                QSize(originalWidth, originalHeigth)
                ,Qt::KeepAspectRatio);
    //qDebug() << "img.scaled";

    //imgScalWidth = (int*)imgScl.width();
    //imgScalHeigth = (int*)imgScl.height();

    //qDebug() << "imgScl";


    this->setPixmap(QPixmap::fromImage(imgScl));
    //qDebug() << "setPixmap";

    if(this->width() != imgScl.width()) {
        QRect rect = this->geometry();
        rect.setWidth(imgScl.width());
        if(rect.width() < windowGeometry->width()) {//если размер лейбла меньше размеров окна, выставляем лейбл по центру
            int p = (windowGeometry->width() - rect.width())/2;
            rect.setX(p);
        }
        this->setGeometry(rect);

    }
    if(this->height() != imgScl.height()) {
        QRect rect = this->geometry();
        rect.setHeight(imgScl.height());
        this->setGeometry(rect);
    }
    //qDebug() << "--------------------------REPAINT REPAINT REPAINT-------------------------";
}

void CustomLabel::setWindowGeometry(QRect &geometry)
{
    windowGeometry = new QRect(geometry);
    originalHeigth = this->height();
    originalWidth = this->width();
    qDebug() << "setWindowGeometry";

}

int CustomLabel::getImgWidth() const
{
    return imgWidth;
}

int CustomLabel::getImgHeigth() const
{
    return imgHeigth;
}

StateLabel CustomLabel::getState() const
{
    return state;
}

void CustomLabel::setState(StateLabel newState)
{
    state = newState;
}

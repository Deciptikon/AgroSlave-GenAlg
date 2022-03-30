#include "drawtrack.h"

DrawTrack::DrawTrack(QObject *parent) : QObject(parent)
{
    msecDeltaTime = 0;
    width = 10;
    height = 10;
    marginAxis = 5;
    maxData = 100;
    scale = 10;

    colorPath = QColor(255, 0, 0);
    widthPath = 4;

    isCentered = false;

    vecSens = QVector2D(3,1);//=======================================================
}

void DrawTrack::init(const int msecDeltaTime, const int width,  const int height)
{
    if(msecDeltaTime > 1) {
        this->msecDeltaTime = msecDeltaTime;
    } else {
        this->msecDeltaTime = 1;
    }

    if(width > 10) {
        this->width = width;
    } else {
        this->width = 10;
    }

    if(height > 10) {
        this->height = height;
    } else {
        this->height = 10;
    }

    imageGraph = new QImage(this->width, this->height, QImage::Format::Format_RGB32);
    imageGraph->fill(QColor(255,255,255));

    qDebug() << "ui->label_chart->geometry().width() =" << this->width;
    qDebug() << "ui->label_chart->geometry().height() =" << this->height;
    dataApp = new QList<QPointF>;
    path = new QPainterPath();
    //painter = new QPainter(imageGraph);
}

int DrawTrack::getMsecDeltaTime() const
{
    return msecDeltaTime;
}

void DrawTrack::loop()
{
    imageGraph->fill(QColor(255,255,255));
    painter = new QPainter(imageGraph);

    drawAxis();
    drawPath();
    drawVector(vecGPS, QColor(255,0,255));
    drawVector(vecSens, QColor(0,255,0));
    //
    //painter->worldTransform()

    painter->end();
    QImage img = imageGraph->copy();
    emit drawImage(img);

}

void DrawTrack::updateGPS(const double &x, const double &y)
{


    if(!isCentered) {
        if (abs(x) >1 && abs(y) >1) {
            qDebug() << "CENTERED CENTERED CENTERED  x =" << x << " | y =" << y;
            setXYCenter(x, y);
        }
    }

    if(isCentered) {

        //qDebug() << "x =" << x << " | y =" << y;
        //qDebug() << "xCenter =" << xCenter << " | yCenter =" << yCenter;


        double xx = x - xCenter;
        double yy = yCenter - y;

        dX = xx;
        dY = -yy;

        //qDebug() << "x-xCenter =" << xx << " | yCenter-y =" << yy;
        //qDebug() << "dX =" << dX << " | dY =" << dY;

        QPointF p = {(float)(xx), //+ width/2
                     (float)(yy)};//+ height/2
        dataApp->append(p);

        vecGPS = QVector2D(xx - dataApp->last().x(), - yy - dataApp->last().y());
        qDebug() << "vecGPS = =" << vecGPS;

        while(dataApp->size()>maxData) {
            dataApp->removeFirst();
        }

        if(!dataApp->isEmpty() ) {

            path = new QPainterPath();
            path->moveTo(dataApp->first());

            for (auto point : *dataApp ) {
                path->lineTo(point);
            }
        }
    }
}

void DrawTrack::updateSensors()
{
    //
}

void DrawTrack::drawAxis()
{
    if(!imageGraph->isNull()) {
        //qDebug() << "CustomChart::drawAxis()";
        //QPainter painter(imageGraph);
        QPen penLine{Qt::NoBrush, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
        penLine.setColor(QColor(0,0,0));//черные оси

        painter->save();
        painter->translate( width/2, this->height/2 );

        painter->setPen(penLine);

        painter->drawLine(QPoint(marginAxis - this->width/2, 0 ),//X
                          QPoint(this->width/2 - marginAxis, 0));

        painter->drawLine(QPoint(0 , marginAxis - this->height/2 ),//Y
                          QPoint(0 , this->height/2 - marginAxis));

        QPointF n = QPointF(0, 0);
        painter->drawEllipse(n, 10, 10);
        painter->drawEllipse(n, 20, 20);
        painter->drawEllipse(n, 30, 30);
        painter->drawEllipse(n, 50, 50);
        painter->drawEllipse(n, 100, 100);

        painter->restore();
        //painter.end();
    }
}

void DrawTrack::drawPath()
{
    if(!imageGraph->isNull() && !path->isEmpty()) {
        QPen penLine{Qt::NoBrush, widthPath/scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
        penLine.setColor(colorPath);//
        painter->save();
        painter->setPen(penLine);

        painter->translate( width/2 - dX*scale, height/2 + dY*scale);
        painter->scale(scale, scale);

        painter->drawPath(*path);
        painter->restore();
    }
}

void DrawTrack::drawVector(QVector2D vec, QColor color)
{
    QPen penLine{Qt::NoBrush, 1/scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
    penLine.setColor(color);
    painter->save();
    painter->translate( width/2, height/2 );
    painter->scale(scale, scale);

    painter->setPen(penLine);

    painter->drawLine(0      , 0        ,
                      vec.x(), -vec.y());

    painter->restore();
}

void DrawTrack::setXYCenter(double xValue, double yValue)
{
    xCenter = xValue;
    yCenter = yValue;
    isCentered = true;
}

void DrawTrack::setMaxData(int value)
{
    if(value>0) {
        maxData = value;
    } else {
        maxData = 1;
    }
}

void DrawTrack::setMarginAxis(int value)
{
    marginAxis = value;
}

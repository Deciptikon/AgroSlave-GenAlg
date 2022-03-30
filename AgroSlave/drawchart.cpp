#include "drawchart.h"

drawChart::drawChart(QObject *parent) : QObject(parent)
{
    width       = 10;
    height      = 10;
    marginAxis  = 5;
    maxData     = 100;
    scale       = 10;
    step        = 1;

    colorPath   = QColor(255, 0, 0);
    widthPath   = 2;
}

void drawChart::init(const int width, const int height)
{
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

void drawChart::setMarginAxis(int value)
{
    marginAxis = value;
}

void drawChart::setMaxData(int value)
{
    maxData = value;
}

void drawChart::setStep(float value)
{
    step = value;
}

void drawChart::appValueToData(const float &valueX, const float &valueY)
{
    //qDebug() << "void drawChart::appValueToData( , )";
    QPointF p = {valueX, //+ width/2
                 valueY};//+ height/2

    dataApp->append(p);

    while(dataApp->size()>maxData) {
        dataApp->removeFirst();
    }

//    std::sort((*dataApp).begin(), (*dataApp).end(),
//          [](QPointF a,  QPointF b) -> bool { return a.x() < b.x(); });

    maxX=dataApp->first().x();
    minX=dataApp->first().x();
    maxY=dataApp->first().y();
    minY=dataApp->first().y();
    for(auto p: *dataApp) {
        if(p.x()>maxX)maxX=p.x();
        if(p.x()<minX)minX=p.x();
        if(p.y()>maxY)maxY=p.y();
        if(p.y()<minY)minY=p.y();
    }

    if(maxY == minY) minY--;
    if(maxX == minX) maxX++;

    //qDebug() << "maxX =" << maxX << "    minX =" << minX;
    //qDebug() << "maxY =" << maxY << "    minY =" << minY;
    if(!dataApp->isEmpty()) {
        path = new QPainterPath();
        float xf = 0;
        float yf = 0;
        path->moveTo(xf,yf);

        if(maxX != minX && maxY != minY) {
            xf = (width -2*marginAxis)*(dataApp->first().x()-minX)/(maxX-minX);
            yf = (2*marginAxis-height)*(dataApp->first().y()-minY)/(maxY-minY);
            path->moveTo(xf,yf);
            for (auto point : *dataApp ) {
                float x = (width -2*marginAxis)*(point.x()-minX)/(maxX-minX);
                float y = (2*marginAxis-height)*(point.y()-minY)/(maxY-minY);
                //qDebug() << "xpoint =" << x << "    ypoint =" << y;
                path->lineTo(x,y);
            }
        }
    }
    redraw();
}

void drawChart::setData(const ListFloat data)
{
    qDebug() << "void drawChart::setData(const ListFloat data)";

    if(!data.isEmpty()) {
        float x=0;
        dataApp->clear();
        for(float d: data) {
            QPointF p = {x,d};
            dataApp->append(p);
            x++; //x += step;
            //if(x>this->width) break;
            //qDebug() << "point =" << p;
        }
        while(dataApp->size()>maxData) {
            dataApp->removeFirst();
        }

        maxX=x;
        minX=0;
        maxY=1;
        minY=-1;
//        maxX=dataApp->first().x();
//        minX=dataApp->first().x();
//        maxY=dataApp->first().y();
//        minY=dataApp->first().y();
//        for(auto p: *dataApp) {
//            if(p.x()>maxX)maxX=p.x();
//            if(p.x()<minX)minX=p.x();
//            if(p.y()>maxY)maxY=p.y();
//            if(p.y()<minY)minY=p.y();
//        }

        if(!dataApp->isEmpty()) {
            path = new QPainterPath();
            //path->moveTo(0,0);
            path->moveTo(0,
                        (dataApp->first().y()-minY)/(maxY-minY)*(2*marginAxis-height));

            for (auto point : *dataApp ) {
                path->lineTo((point.x()-minX)/(maxX-minX)*(width -2*marginAxis)
                            ,(point.y()-minY)/(maxY-minY)*(2*marginAxis-height));
            }

        }
        redraw();
    }
}

void drawChart::drawAxis()
{
    //qDebug() << "void drawChart::drawAxis()";
    if(!imageGraph->isNull()) {
        //qDebug() << "CustomChart::drawAxis()";
        //QPainter painter(imageGraph);
        QPen penLine{Qt::NoBrush, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
        penLine.setColor(QColor(0,0,0));//черные оси

        painter->save();
        painter->translate( marginAxis, this->height - marginAxis );

        painter->setPen(penLine);

        painter->drawLine(QPoint(0, 0 ),//X
                          QPoint(this->width - 2*marginAxis, 0));

        painter->drawLine(QPoint(0 , 0 ),//Y
                          QPoint(0 , -this->height + 2*marginAxis));

        painter->restore();
        //painter.end();
    }
}

void drawChart::drawPath()
{
    //qDebug() << "void drawChart::drawPath()";
    if(!imageGraph->isNull() && !path->isEmpty()) {
        QPen penLine{Qt::NoBrush, widthPath, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
        penLine.setColor(colorPath);//
        painter->save();
        painter->setPen(penLine);

        painter->translate( marginAxis, this->height - marginAxis );
        //painter->scale(scale, scale);

        painter->drawPath(*path);


        painter->drawText(0,                           0, QString::number(minY));
        painter->drawText(0, 5*marginAxis - this->height, QString::number(maxY));

        painter->restore();
    }
}

void drawChart::redraw()
{
    //qDebug() << "void drawChart::redraw()";
    imageGraph->fill(QColor(255,255,255));
    painter = new QPainter(imageGraph);

    drawAxis();
    drawPath();
    //painter->worldTransform()

    painter->end();
    QImage img = imageGraph->copy();
    //qDebug() << "void drawChart::redraw() --> emit drawImage(img)";
    emit drawImage(img);
}

void drawChart::clearData()
{
    dataApp->clear();
    path->clear();
}

void drawChart::setMinMaxX(float min, float max)
{
    if(max > min) {
        minX = min;
        maxX = max;
    } else {
        minX = max;
        maxX = min;
    }
}

void drawChart::setMinMaxY(float min, float max)
{
    if(max > min) {
        minY = min;
        maxY = max;
    } else {
        minY = max;
        maxY = min;
    }
}

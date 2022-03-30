#ifndef DRAWCHART_H
#define DRAWCHART_H

#include <QObject>
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QVector2D>

typedef QList<float> ListFloat;

class drawChart : public QObject
{
    Q_OBJECT
public:
    explicit drawChart(QObject *parent = nullptr);

    void init(const int width,  const int height);

    void setMarginAxis(int value);

    void setMaxData(int value);

    void setStep(float value);

    void setMinMaxX(float min, float max);

    void setMinMaxY(float min, float max);

signals:
    void drawImage(const QImage &img);

public slots:
    void appValueToData(const float &valueX, const float &valueY);
    void setData(const ListFloat data);
    void redraw();
    void clearData();

private slots:
    //

private:
    void drawAxis();
    void drawPath();


    int width, height;
    int marginAxis;
    int maxData;
    float step;
    double scale;

    float maxX, minX;
    float maxY, minY;


    QPainter *painter;
    QImage *imageGraph;
    QList<QPointF> *dataApp;
    QPainterPath *path;

    QColor colorPath;
    qreal widthPath;
};

#endif // DRAWCHART_H

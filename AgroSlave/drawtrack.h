#ifndef DRAWTRACK_H
#define DRAWTRACK_H

#include <QObject>
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QVector2D>

class DrawTrack : public QObject
{
    Q_OBJECT
public:
    explicit DrawTrack(QObject *parent = nullptr);

    void init(const int secDeltaTime, const int width,  const int height);

    int getMsecDeltaTime() const;

    void setMarginAxis(int value);

    void setMaxData(int value);

    void setXYCenter(double xValue, double yValue);

signals:
    void drawImage(const QImage &img);

public slots:
    void updateGPS(const double &x, const double &y);
    void updateSensors();
    void loop();

private slots:
    //
private:
    void drawAxis();
    void drawPath();
    void drawVector(QVector2D vec, QColor color);

    int msecDeltaTime;
    int width, height;
    int marginAxis;
    int maxData;

    double xCenter, yCenter;
    float dX, dY;
    bool isCentered;
    double scale;

    QVector2D vecGPS, vecSens;

    QPainter *painter;
    QImage *imageGraph;
    QList<QPointF> *dataApp;
    QPainterPath *path;

    QColor colorPath;
    qreal widthPath;
};

#endif // DRAWTRACK_H

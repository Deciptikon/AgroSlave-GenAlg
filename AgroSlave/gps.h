#ifndef GPS_H
#define GPS_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QGeoPositionInfoSource>
#include <QNmeaPositionInfoSource>
#include <QtMath>

//#include "filtrkalmana.h"


const double RADIUS_EARTH = 6371000;
const double MAX_LONGITUDE = 85.0511288;

class GPS : public QObject
{
    Q_OBJECT
public:
    explicit GPS(QObject *parent = nullptr);

    void setMsecUpdate(int msec);

    //bool getKalman() const;
    //void setKalman(bool value);


signals:
    void updatePositionGeo(QGeoCoordinate &coord);
    void updatePositionXY(const double &x, const double &y);
    void updatePositionLatLon(double &Lat, double &Lon);

public slots:
    void init();
    //void onOffFiltrKalmana();
private slots:
    void positionUpdate(QGeoPositionInfo info);

private:
    void latLonToXY(double lat, double lon);
    //void filtrKalmana(QGeoCoordinate coordinate);

    QSerialPort* serial;

    QNmeaPositionInfoSource* source;

    QGeoCoordinate currentCoordinate;

    double x,y;

    int msecUpdate;



    //bool kalman;
    //FiltrKalmana fKalmana;


};

#endif // GPS_H

#ifndef SENSORREADER_H
#define SENSORREADER_H

#include <QObject>

#include "accelerometer.h"
#include "gyroscope.h"

class SensorReader : public QObject
{
    Q_OBJECT
public:
    explicit SensorReader(QObject *parent = nullptr);

    void init(const int msec);

    Accelerometer Accelerometer;
    Gyroscope Gyroscope;

    int getMsecDeltaTime() const;

signals:
    void updateDataSens(const QVector3D &accel, const QVector3D &gyro);
//    void updateDataSens2(const float &xa, const float &ya, const float &za,
//                         const float &xg, const float &yg, const float &zg);

public slots:
    void loop();
    void calibrate();
    void calibrateGyroAxisZ();

private slots:

private:
    int msecDeltaTime;

    bool isCalibrate;
    bool isCalibrateGyroAxisZ;
    const int MAX_COUNT_TIMER_CALIBRATE = 50;
    int countTimerCalibrate;

    float xa, ya, za, xg, yg, zg;
};

#endif // SENSORREADER_H

#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include <QObject>
#include <QDebug>
#include <QVector3D>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <QtMath>

class Gyroscope : public QObject
{
    Q_OBJECT
public:
    explicit Gyroscope(QObject *parent = nullptr);

    void init();

    void updateData();

    QVector3D getData() const;
    QVector3D getCalibrateDataNull();
    QVector3D getCalibrateDataNullAxisZ();

    QVector3D getCalibrateNull() const;
    void setCalibrateNull(const QVector3D &value);

    QVector3D getCalibrateGyroAxisZ() const;
    void setCalibrateGyroAxisZ(const QVector3D &value);

    void setCalibrateNullisDone(bool value);

    void setCalibrateGyroAxisZisDone(bool value);

signals:

private:
    QVector3D data;

    int deviceGyro;

    QVector3D calibrateNull;
    bool calibrateNullisDone;

    QVector3D calibrateGyroAxisZ;
    bool calibrateGyroAxisZisDone;

};

#endif // GYROSCOPE_H

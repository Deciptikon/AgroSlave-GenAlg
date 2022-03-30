#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <QObject>
#include <QDebug>
#include <QVector3D>


#include <wiringPi.h>
#include <wiringPiI2C.h>

class Accelerometer : public QObject
{
    Q_OBJECT
public:
    explicit Accelerometer(QObject *parent = nullptr);

    void init();

    void updateData();

    QVector3D getData() const;

    QVector3D getCalibrateData() const;

    void setCalibrateData(const QVector3D &value);

signals:

public slots:


private:
    QVector3D data;

    int deviceAccel;

    QVector3D calibrateData;
};

#endif // ACCELEROMETER_H

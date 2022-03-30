#ifndef SLAVECONTROLLER_H
#define SLAVECONTROLLER_H

#include <QObject>
#include <QDebug>

#include <wiringPi.h>
#include <wiringPiI2C.h>

class SlaveController : public QObject
{
    Q_OBJECT
public:
    explicit SlaveController(QObject *parent = nullptr);

    void init(int hexAdress);

signals:
    void readFrom(int &data);

public slots:
    void getData();
    void readData();
    void writeData();
    void writeData(int &data);

private:
    int hexAdress;

    int deviceRegAdress;
};

#endif // SLAVECONTROLLER_H

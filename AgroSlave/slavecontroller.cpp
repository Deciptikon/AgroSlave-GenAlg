#include "slavecontroller.h"

SlaveController::SlaveController(QObject *parent) : QObject(parent)
{

}

void SlaveController::init(int hexAdress)
{

    if(hexAdress>0x08 && hexAdress<0x80) {//0-128
        this->hexAdress = hexAdress;
    } else {
        this->hexAdress = 0;
    }

    this->deviceRegAdress = wiringPiI2CSetup(this->hexAdress);
    qDebug() << "Setup deviceRegAdress N" << this->hexAdress << "= " << this->deviceRegAdress;
}

//чтение по команде
void SlaveController::getData()
{
    if (this->deviceRegAdress == -1) {
        qDebug() << "[SlaveController::loop()] deviceRegAdress == -1";
    } else {
        uint8_t g = 'g';
        uint8_t f = 'f';
//        wiringPiI2CWrite(this->deviceRegAdress, g);
//        int received_data = wiringPiI2CRead(this->deviceRegAdress);
//        wiringPiI2CWrite(this->deviceRegAdress, f);
//        int received_data2 = wiringPiI2CRead(this->deviceRegAdress);

        int received_data  = wiringPiI2CReadReg8(this->deviceRegAdress, g);
        int received_data2 = wiringPiI2CReadReg8(this->deviceRegAdress, f);

        int rd=received_data2*100 + received_data;
        emit readFrom(rd);

        qDebug() << "-----------------------------------------------";
        qDebug() << "Slave" << QString::number(this->hexAdress).toLocal8Bit() << "read 1: " << received_data;
        qDebug() << "Slave" << QString::number(this->hexAdress).toLocal8Bit() << "read 2: " << received_data2;
        qDebug() << "Slave" << QString::number(this->hexAdress).toLocal8Bit() << "read summ: " << rd;
    }
}

//просто чтение
void SlaveController::readData()
{

}

//просто запись
void SlaveController::writeData()
{
    uint8_t c = 120;//'c'

    if (this->deviceRegAdress == -1) {
        qDebug() << "[SlaveController::loop()] deviceRegAdress == -1";
    } else {
        wiringPiI2CWrite(this->deviceRegAdress, c);
        qDebug() << "SlaveController::writeData()";
    }
}

void SlaveController::writeData(int &data)
{

    if (this->deviceRegAdress == -1) {
        qDebug() << "[SlaveController::loop()] deviceRegAdress == -1";
    } else {
        wiringPiI2CWrite(this->deviceRegAdress, data);
        qDebug() << "SlaveController::writeData()";
    }
}

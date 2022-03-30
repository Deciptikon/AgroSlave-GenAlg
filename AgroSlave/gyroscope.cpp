#include "gyroscope.h"

Gyroscope::Gyroscope(QObject *parent) : QObject(parent)
{
    // Gyroscope ITG3200--------------------------
    #define GYRO 0x68 //  when AD0 is connected to GND ,gyro address is 0x68.
    //#define GYRO 0x69   when AD0 is connected to VCC ,gyro address is 0x69
    #define GYRO_POWER_CTL 0x1B
    #define GYRO_XOUT_H    0x1D
    #define GYRO_XOUT_L    0x1E
    #define GYRO_YOUT_H    0x1F
    #define GYRO_YOUT_L    0x20
    #define GYRO_ZOUT_H    0x21
    #define GYRO_ZOUT_L    0x22

    #define G_SMPLRT_DIV 0x15
    #define G_DLPF_FS 0x16
    #define G_INT_CFG 0x17
    #define G_PWR_MGM 0x3E
    #define G_TO_READ 8 // 2 bytes for each axis x, y, z
    //------------------------------------------------
    deviceGyro = -1;

    calibrateNull = {0,0,0};
    calibrateNullisDone = false;

    calibrateGyroAxisZ = {0,0,0};
    calibrateGyroAxisZisDone = false;
}

void Gyroscope::init()
{

        deviceGyro = wiringPiI2CSetup(GYRO);
        qDebug() << "Setup deviceGyroRegAdress =" << deviceGyro;

        if (deviceGyro == -1) {
            qDebug() << "[Gyroscope::init()] deviceGyro == -1";
        } else {
            qDebug() << "I2C communication successfully setup.\n";

            wiringPiI2CWriteReg8(GYRO, G_PWR_MGM, 0x00);
            wiringPiI2CWriteReg8(GYRO, G_SMPLRT_DIV, 0x07);
            wiringPiI2CWriteReg8(GYRO, G_DLPF_FS, 0x1E);
            wiringPiI2CWriteReg8(GYRO, G_INT_CFG, 0x00);
        }
}

void Gyroscope::updateData()
{
    if (deviceGyro == -1) {
        qDebug() << "[Gyroscope::loop()] deviceGyro == -1";
    } else {
        int dataXg = wiringPiI2CReadReg16(deviceGyro, GYRO_XOUT_L);
        int dataYg = wiringPiI2CReadReg16(deviceGyro, GYRO_YOUT_L);
        int dataZg = wiringPiI2CReadReg16(deviceGyro, GYRO_ZOUT_L);

        float koeff = 1.0/207000.0;// 1.0/215000.0

        data = QVector3D(
                    -(~(int16_t)dataXg + 1) * koeff,
                    -(~(int16_t)dataYg + 1) * koeff,
                    -(~(int16_t)dataZg + 1) * koeff);
        //qDebug() << "Gyroscope:" << data;
    }
}

QVector3D Gyroscope::getData() const
{
    return data;
}

QVector3D Gyroscope::getCalibrateDataNull()
{
    if(calibrateNullisDone) {
        QVector3D d;
        d = data - calibrateNull;
        //qDebug() << "QVector3D Gyroscope::getCalibrateDataNull(): data - calibrateNull";
        return d;
    } else {
        //qDebug() << "QVector3D Gyroscope::getCalibrateDataNull(): data";
        return data;
    }
}

QVector3D Gyroscope::getCalibrateDataNullAxisZ()
{
    QVector3D d;
    d = getCalibrateDataNull();

    if(calibrateGyroAxisZisDone) {

//        if(calibrateGyroAxisZ.length() == 0) {
//            calibrateGyroAxisZisDone = false;
//            return d;
//        }


        //проекция вектора d на условную ось Z, определенную во время калибровки
        float pseudoZ = QVector3D::dotProduct(d,calibrateGyroAxisZ);

        //оставшаяся часть приходящаяся на прочие оси, пока аписанная в ось Х
        //float pseudoX = sqrt(d.lengthSquared() - pseudoZ*pseudoZ);

        //qDebug() << "pseudoZ =" << pseudoZ << "\t| pseudoX =" << pseudoX;
        QVector3D z;
        z = {0, 0, pseudoZ};//{pseudoX, 0, pseudoZ}

        return z;
    } else {
        return d;
    }
}

QVector3D Gyroscope::getCalibrateNull() const
{
    return calibrateNull;
}

void Gyroscope::setCalibrateNull(const QVector3D &value)
{
    calibrateNull = value;
}

QVector3D Gyroscope::getCalibrateGyroAxisZ() const
{
    return calibrateGyroAxisZ;
}

void Gyroscope::setCalibrateGyroAxisZ(const QVector3D &value)
{
    calibrateGyroAxisZ = value;
}

void Gyroscope::setCalibrateNullisDone(bool value)
{
    qDebug() << "calibrateNullisDone =" << value;
    calibrateNullisDone = value;
}

void Gyroscope::setCalibrateGyroAxisZisDone(bool value)
{
    qDebug() << "calibrateGyroAxisZisDone =" << value;
    calibrateGyroAxisZisDone = value;
}

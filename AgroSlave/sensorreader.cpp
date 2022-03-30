#include "sensorreader.h"

SensorReader::SensorReader(QObject *parent) : QObject(parent)
{
    //
}

void SensorReader::init(const int msec)
{
    if(msec > 1) {
        msecDeltaTime = msec;
    } else {
        msecDeltaTime = 1;
    }
    isCalibrate          = false;
    isCalibrateGyroAxisZ = false;

    Accelerometer.init();
    Gyroscope.init();
}

void SensorReader::loop()
{
    Accelerometer.updateData();
    Gyroscope.updateData();

    //-----------Calibrate sensors-------------------------------------------------------------
    if(isCalibrate) {
        if (countTimerCalibrate < MAX_COUNT_TIMER_CALIBRATE) {
            countTimerCalibrate++;


            QVector3D calibrateDataAccelerometer = (Accelerometer.getCalibrateData() * (countTimerCalibrate - 1) +
                                       Accelerometer.getData())/(float)countTimerCalibrate;
            Accelerometer.setCalibrateData(calibrateDataAccelerometer);
            qDebug() << "Accelerometer::calibrate() " << calibrateDataAccelerometer;

            QVector3D calibrateDataGyroscope = (Gyroscope.getCalibrateNull() * (countTimerCalibrate - 1) +
                                       Gyroscope.getData())/(float)countTimerCalibrate;
            Gyroscope.setCalibrateNull(calibrateDataGyroscope);
            qDebug() << "Gyroscope::calibrate() " << calibrateDataGyroscope;
        } else {
            isCalibrate=false;
            Gyroscope.setCalibrateNullisDone(true);
        }
    }
    //------------------------------------------------------------------------------------------

//qDebug() << "SensorReader::loop()";
    QVector3D accelData = Accelerometer.getData() - Accelerometer.getCalibrateData();
    QVector3D gyrosData = Gyroscope.getCalibrateDataNullAxisZ();//Gyroscope.getData() - Gyroscope.getCalibrateNull();

//    xa=accelData.x();
//    ya=accelData.y();
//    za=accelData.z();
//    xg=gyrosData.x();
//    yg=gyrosData.y();
//    zg=gyrosData.z();

    if(isCalibrateGyroAxisZ) {

        QVector3D v = Gyroscope.getCalibrateGyroAxisZ() + Gyroscope.getCalibrateDataNull();
        qDebug() << "isCalibrate -----> g =" << v;
        Gyroscope.setCalibrateGyroAxisZ(v);
    }

    emit updateDataSens( accelData, gyrosData);
//    emit updateDataSens2( xa, ya, za, zg, yg, zg);

}

void SensorReader::calibrate()
{
    qDebug() << "==========================================CALIBRATE==============================================";
    isCalibrate=true;
    Gyroscope.setCalibrateNullisDone(false);
    countTimerCalibrate=0;
    Accelerometer.setCalibrateData({0,0,0});
    Gyroscope.setCalibrateNull({0,0,0});
}

void SensorReader::calibrateGyroAxisZ()
{
    qDebug() << "================CALIBRATE GYRO AXIS==================";
    isCalibrateGyroAxisZ = !isCalibrateGyroAxisZ;
    if(isCalibrateGyroAxisZ) {
        qDebug() << "isCalibrateGyroAxisZ is true";
        Gyroscope.setCalibrateGyroAxisZisDone(false);
        Gyroscope.setCalibrateGyroAxisZ({0,0,0});
    } else {
        qDebug() << "isCalibrateGyroAxisZ is false";
        Gyroscope.setCalibrateGyroAxisZ(Gyroscope.getCalibrateGyroAxisZ().normalized());
        qDebug() << "ending Calibrate accel  -----> a =" << Accelerometer.getCalibrateData();
        qDebug() << "ending Calibrate vector -----> g =" << Gyroscope.getCalibrateGyroAxisZ();
        Gyroscope.setCalibrateGyroAxisZisDone(true);
    }
}

int SensorReader::getMsecDeltaTime() const
{
    return msecDeltaTime;
}

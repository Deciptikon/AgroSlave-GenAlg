#ifndef AUTOPILOT_H
#define AUTOPILOT_H

#include <QObject>
#include <QDebug>
#include <QtMath>
#include <QVector3D>
#include <QRandomGenerator>

#include "genalg.h"
#include "neuronet.h"

typedef QList<float> ListFloat;

class Autopilot : public QObject
{
    Q_OBJECT
public:
    explicit Autopilot(QObject *parent = nullptr);

    void init(const int secDeltaTime);

    int getMSecDeltaTime() const;


    void calculateNeuroTopology();

    void restateRelay();

signals:
    void sendToAndroid(const QByteArray &mess);
    void sendCommandToSlave14(int &comm);

    void currentEpoha(const int &num);
    void currentCount(const int &num);
    void currentTrain(const int &num);
    void currentError(const float &num);

    void currentLern(const float &x, const float &y);
    void currentNeuroTopology(const ListFloat list);

    void sendInputOutputDataAndNeuroNet(  const ListFloat inputData
                                        , const ListFloat outputData
                                        , const NeuroNet  neuroNet);

public slots:
    void loop();

    void readFromSensorReader(const QVector3D &accel, const QVector3D &gyro);
//    void readFromSensorReader2(const float &xa, const float &ya, const float &za,
//                               const float &xg, const float &yg, const float &zg);
    void readFromBluetoothServer(const QByteArray &mess);
    void readFromGPS(const double &x, const double &y);

    void resetDataSens();

    void startLearn();
    void stopLearn();
    void reverseLearn();


    //TEST ANGLE FOR NEURO NET
    float angleGAMATestForNN(float angle);
    float angleALFATestForNN(float maxAngle, int maxNum, int num);

    void acceptNewMaxValueMutation(const double &value);
    void acceptNewProbalityMutation(const double &value);
    void acceptNewGeneration(const double &value);

    void acceptNeuroNet(const NeuroNet net);

private slots:

private:
    void initV0();
    void runV0();

    void initV1();
    void runV1();


    int msecDeltaTime;

    QVector3D accelerometer, gyroscope;

    QVector3D velocity, translate, angle;

    bool stateRelay; //состояние реле подруливающего устройства

    bool onLearn;//флаг обучения

    //-----------------vtoraja versija------------------------
    int MAX_TRAIN;//максимальное количество тренировок для особи
    int currentTraining;//индекс текущего тренеровочного набора

    QList<float>  inputAngle;
    QList<float> outputAngle;
    int epoha;//индек текущей эпохи
    //---------------------------------------------


    bool isValidNeuroNet;
    NeuroNet neuroNet;



    //---------------pervaja versija--------------------------




    int MAX_COUNT;//максимальное количество особей вычисляющих приспособленность
    int count;//индекс текущей особи

    float err;//сумма квадратов отклонений
    //----------------------------------------------



    int currentTimeDrive;//текущее время водения
    int calculateTimeDrive;//предварительно вычисленное время водения
    QVector3D angleA;//nachalniy ugоl
    QVector3D angleB;//konechniy ugol
    GenAlg gen;//geneticheskiy algorithm
    NeuroNet net;

    float generateMutation;
    float maxValueMutation;
    float probalityMutation;

    //TEST NEURO
    float alfa, beta, gama;
};

#endif // AUTOPILOT_H

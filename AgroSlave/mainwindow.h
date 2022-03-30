#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>

#include "customlabel.h"//============

#include "autopilot.h"
#include "genalg2.h"
#include "gps.h"
#include "bluetoothserver.h"
#include "sensorreader.h"
#include "slavecontroller.h"
#include "drawtrack.h"
#include "drawchart.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void setNewMaxValueMutation(const double &value);
    void setNewProbalityMutation(const double &value);
    void setNewGeneration(const double &value);

    void setGenerateMutation(float value);

    void setMaxValueMutation(float value);

    void setProbalityMutation(float value);

public slots:
    void initDataAndNeuroNet(  const ListFloat inputData
                               , const ListFloat outputData
                               , const NeuroNet  neuroNet);
    void initAndStartGenAlg();

private slots:
    void updateLatLon(QGeoCoordinate &coord);

    void updateDataSens(QVector3D accel, QVector3D gyro);

    void readFromSlave14(int &data);

    void connectedClient(const QString &name);
    void disconnectedClient(const QString &name);

    void acceptNeuroNet(const NeuroNet net);
    //void buttonFiltrKalmana();



    void updateEpoha(const int &num);
    void updateCount(const int &num);
    void updateTrain(const int &num);
    void updateError(const float &num);

    void on_hs_max_mut_valueChanged(int value);

    void on_hs_probality_mutation_valueChanged(int value);

    void on_hs_generation_valueChanged(int value);


private:


    Ui::MainWindow *ui;

    ListFloat inputData;
    ListFloat outputData;
    NeuroNet neuroNet;

    int epoha;

    float generateMutation;

    float maxValueMutation;

    float probalityMutation;


    Autopilot* autopilot;
    QThread* threadAutopilot;
    QTimer* timerAutopilot;

    GenAlg2* genalg2;
    QThread* threadGenAlg2;

    GPS* gps;
    QThread* threadGPS;

    BluetoothServer* bluetoothserver;

    SensorReader* sensorreader;
    QThread* threadSensorReader;
    QTimer* timerSensorReader;

    SlaveController* slavecontroller_14;
    QThread* threadSlaveController_14;

    DrawTrack *drawtrack;
    QThread* threadDrawTrack;
    QTimer* timerDrawTrack;

    drawChart *drawerror;
    QThread* threadDrawError;

    drawChart *drawneuro;
    QThread* threadDrawNeuro;
};
#endif // MAINWINDOW_H

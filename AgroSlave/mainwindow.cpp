#include "mainwindow.h"
#include "ui_mainwindow.h"

typedef QList<float> ListFloat;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRect sizeWindow = MainWindow::geometry();
    ui->lb_draw_track->setWindowGeometry(sizeWindow);
    //qDebug() << "Size ui->lb_draw_track->height() =" << ui->lb_draw_track->height();
    //qDebug() << "Size ui->lb_draw_track->width()  =" << ui->lb_draw_track->width();
    ui->lb_draw_error->setWindowGeometry(sizeWindow);
    ui->lb_draw_neuro->setWindowGeometry(sizeWindow);

    epoha = 0;
    generateMutation  = 10;
    maxValueMutation  = 0.1;
    probalityMutation = 0.1;


    qRegisterMetaType<QList<float>>("ListFloat");
    qRegisterMetaType<NeuroNet>("NeuroNet");

    ///-------Create autopilot and move to thread with timer----------------------------------------
    autopilot = new Autopilot(this);
    autopilot->init(1000);

    threadAutopilot = new QThread(this);

    timerAutopilot = new QTimer(0);
    timerAutopilot->setInterval(autopilot->getMSecDeltaTime());
    timerAutopilot->moveToThread(threadAutopilot);

    autopilot->connect( timerAutopilot, SIGNAL(timeout()), SLOT(loop()), Qt::ConnectionType::DirectConnection);
    timerAutopilot->connect(threadAutopilot, SIGNAL(started()), SLOT(start()));
    connect(ui->bt_learn_neuro_net, SIGNAL(clicked()),
            autopilot, SLOT(reverseLearn()) );

    connect(autopilot, SIGNAL(sendInputOutputDataAndNeuroNet(const ListFloat,
                                                             const ListFloat,
                                                             const NeuroNet )),
            this,    SLOT( initDataAndNeuroNet(const ListFloat ,
                                               const ListFloat ,
                                               const NeuroNet) ));
    ///----------------------------------------------------------------------------------------------



    ///-------Create Genetic Algorithm 2----------------------------------------------------------------------
threadGenAlg2=nullptr;


//    genalg2 = new GenAlg2();
//    //genalg2->init();
//    //genalg2->swapRunning();

//    threadGenAlg2 = new QThread(this);
//    genalg2->moveToThread(threadGenAlg2);

//    connect( threadGenAlg2, SIGNAL(started()), genalg2, SLOT(run()) );

    ///----------------------------------------------------------------------------------------------



    ///-------Create GPS-reader----------------------------------------------------------------------
    gps = new GPS();
    gps->init();
    gps->setMsecUpdate(1000);

    threadGPS = new QThread(this);

    gps->moveToThread(threadGPS);

    //connect(ui->bt_filtr_GPS, SIGNAL(clicked()), gps, SLOT(onOffFiltrKalmana()));
    connect(gps, SIGNAL(updatePositionGeo(QGeoCoordinate&)), this, SLOT(updateLatLon(QGeoCoordinate&)));

    ///----------------------------------------------------------------------------------------------



    ///-------Create Bluetooth server----------------------------------------------------------------
    bluetoothserver = new BluetoothServer(this);
    connect(bluetoothserver, SIGNAL(clientConnected(QString)), this, SLOT(connectedClient(QString)));
    connect(bluetoothserver, SIGNAL(clientDisconnected(QString)), this, SLOT(disconnectedClient(QString)));
    bluetoothserver->startServer();
    ///----------------------------------------------------------------------------------------------



    ///-------Create sensor reader (I2C)-------------------------------------------------------------
    sensorreader = new SensorReader(this);
    sensorreader->init(20);

    threadSensorReader = new QThread(this);

    timerSensorReader = new QTimer(0);
    timerSensorReader->setInterval(sensorreader->getMsecDeltaTime());
    timerSensorReader->moveToThread(threadSensorReader);

    sensorreader->connect(timerSensorReader,  SIGNAL(timeout()), SLOT(loop()), Qt::ConnectionType::DirectConnection);
    timerSensorReader->connect(threadSensorReader, SIGNAL(started()), SLOT(start()));
    connect(sensorreader, SIGNAL(updateDataSens(QVector3D, QVector3D)),
            this, SLOT(updateDataSens(QVector3D, QVector3D)) );
//    connect(sensorreader, SIGNAL(updateDataSens(QVector3D, QVector3D)),
//            autopilot, SLOT(updateDataSens(QVector3D, QVector3D)) );
    connect(ui->bt_calibrate_sensors, SIGNAL(clicked()),
            sensorreader, SLOT(calibrate()) );
    connect(ui->bt_calibrate_gyro_axisZ, SIGNAL(clicked()),
            sensorreader, SLOT(calibrateGyroAxisZ()) );

    connect(ui->bt_calibrate_sensors, SIGNAL(clicked()),
                    autopilot, SLOT(resetDataSens()) );
    connect(ui->bt_calibrate_gyro_axisZ, SIGNAL(clicked()),
                    autopilot, SLOT(resetDataSens()) );
    connect(ui->bt_reset, SIGNAL(clicked()),
                    autopilot, SLOT(resetDataSens()) );
    ///----------------------------------------------------------------------------------------------



    ///-------Create I2C slave device----------------------------------------------------------------
    slavecontroller_14 = new SlaveController();
    slavecontroller_14->init( 0x14 );

    threadSlaveController_14 = new QThread(this);
    slavecontroller_14->moveToThread(threadSlaveController_14);

    connect(ui->bt_send_read_command, SIGNAL(clicked()), slavecontroller_14, SLOT(writeData()));
    connect(autopilot, SIGNAL(sendCommandToSlave14(int&)), slavecontroller_14, SLOT(writeData(int&)));
    connect(slavecontroller_14, SIGNAL(readFrom(int&)), this, SLOT(readFromSlave14(int&)));

    ///----------------------------------------------------------------------------------------------



    ///-------Create DrawTrack-----------------------------------------------------------------------
    drawtrack = new DrawTrack(this);
    drawtrack->init(1000,
                    ui->lb_draw_track->width(),
                    ui->lb_draw_track->height());

    threadDrawTrack = new QThread(this);

    timerDrawTrack = new QTimer(0);
    timerDrawTrack->setInterval(drawtrack->getMsecDeltaTime());
    timerDrawTrack->moveToThread(threadDrawTrack);

    drawtrack->connect(timerDrawTrack,  SIGNAL(timeout()), SLOT(loop()), Qt::ConnectionType::DirectConnection);
    timerDrawTrack->connect(threadDrawTrack, SIGNAL(started()), SLOT(start()));

    connect(gps, SIGNAL(updatePositionXY(const double&,const double&)),
            drawtrack, SLOT(updateGPS(const double&,const double&)) );
    connect(drawtrack, SIGNAL(drawImage(const QImage&)), ui->lb_draw_track, SLOT(setImage(const QImage&)));
    ///----------------------------------------------------------------------------------------------



    ///-------Create DrawError-----------------------------------------------------------------------
    drawerror = new drawChart();
    drawerror->init( ui->lb_draw_error->width(), ui->lb_draw_error->height());

    threadDrawError = new QThread(this);
    drawerror->moveToThread(threadDrawError);

    connect(drawerror, SIGNAL(drawImage(const QImage&)), ui->lb_draw_error, SLOT(setImage(const QImage&)));
    connect(threadDrawError, SIGNAL(started()), drawerror, SLOT(redraw()));
    connect(autopilot, SIGNAL( currentLern(const float&, const float&)),
            drawerror, SLOT(appValueToData(const float&, const float&)));
    ///----------------------------------------------------------------------------------------------



    ///-------Create DrawNeuro-----------------------------------------------------------------------
    drawneuro = new drawChart();
    drawneuro->init( ui->lb_draw_neuro->width(), ui->lb_draw_neuro->height());

    threadDrawNeuro = new QThread(this);
    drawneuro->moveToThread(threadDrawNeuro);

    //qRegisterMetaType<const QList<float>>("ListFloat");

    connect(drawneuro, SIGNAL(drawImage(const QImage&)), ui->lb_draw_neuro, SLOT(setImage(const QImage&)));
    connect(threadDrawNeuro, SIGNAL(started()), drawneuro, SLOT(redraw()));
    connect(autopilot, SIGNAL(currentNeuroTopology(const ListFloat)),
            drawneuro, SLOT(setData(const ListFloat)), Qt::QueuedConnection);
    ///----------------------------------------------------------------------------------------------



    ///-------Connects objects-----------------------------------------------------------------------
    connect(gps, SIGNAL(updatePositionLatLon(double&,double&)), bluetoothserver, SLOT(  sendCoord(double&,double&)) );
    connect(gps, SIGNAL(updatePositionXY(const double&,const double&)),
            autopilot, SLOT(readFromGPS(const double&,const double&)) );
    connect(bluetoothserver, SIGNAL(messageReceived(QByteArray)), autopilot, SLOT(readFromBluetoothServer(QByteArray)) );
    connect(sensorreader,  SIGNAL(updateDataSens(const QVector3D&, const QVector3D& )),
            autopilot, SLOT(readFromSensorReader(const QVector3D&, const QVector3D& )) );

    connect(autopilot, SIGNAL(sendCommandToSlave14(int&)),
            slavecontroller_14, SLOT(writeData(int&)) );
    connect(autopilot, SIGNAL(sendToAndroid(QByteArray)),
            bluetoothserver, SLOT(sendMessage(QByteArray)) );

    connect(autopilot, SIGNAL(currentTrain(const int&)),
            this, SLOT(updateTrain(const int&)) );
//    connect(autopilot, SIGNAL(currentCount(const int&)),
//            this, SLOT(updateCount(const int&)) );
//    connect(autopilot, SIGNAL(currentEpoha(const int&)),
//            this, SLOT(updateEpoha(const int&)) );
//    connect(autopilot, SIGNAL(currentError(const float&)),
//            this, SLOT(updateError(const float&)) );

    connect(this, SIGNAL(setNewMaxValueMutation(const double&)),
            autopilot, SLOT(acceptNewMaxValueMutation(const double&)) );
    connect(this, SIGNAL(setNewProbalityMutation(const double&)),
            autopilot, SLOT(acceptNewProbalityMutation(const double&)) );
    connect(this, SIGNAL(setNewGeneration(const double&)),
            autopilot, SLOT(acceptNewGeneration(const double&)) );
    ///----------------------------------------------------------------------------------------------



    ///-------Start threads--------------------------------------------------------------------------
    qDebug() << "main this->thread()->currentThread()" << this->thread()->currentThread();
    threadAutopilot->start();qDebug() << "threadGenAlg2" << threadGenAlg2;
    //threadGenAlg2->start();
    threadGPS->start();
    threadSensorReader->start();
    threadSlaveController_14->start();
    threadDrawTrack->start();
    threadDrawError->start();
    threadDrawNeuro->start();
    ///----------------------------------------------------------------------------------------------
}

MainWindow::~MainWindow()
{
    delete bluetoothserver;
    delete ui;
}

void MainWindow::initDataAndNeuroNet(  const ListFloat inputData
                                     , const ListFloat outputData
                                     , const NeuroNet neuroNet)
{
    qDebug() << "|||||||||||||||  void MainWindow::initDataAndNeuroNet |||||||||||||||";
    this->neuroNet = neuroNet;
    this->inputData = inputData;
    this->outputData = outputData;
    this->epoha = 0;
    initAndStartGenAlg();
}

void MainWindow::initAndStartGenAlg()
{
    qDebug() << "void MainWindow::initAndStartGenAlg()";
    if(threadGenAlg2!=nullptr && threadGenAlg2->isRunning()) {
        threadGenAlg2->exit();
        //genalg2->blockSignals(true);
        QTimer::singleShot(2000, this, SLOT(initAndStartGenAlg()));
    } else {
        genalg2 = new GenAlg2();
        genalg2->init(this->neuroNet, 50);
        genalg2->setInputOutputData(inputData, outputData);

        genalg2->setGenerateMutation(generateMutation);
        genalg2->setMaxValueMutation(maxValueMutation);
        genalg2->setProbalityMutation(probalityMutation);

        genalg2->swapRunning();

        threadGenAlg2 = new QThread(this);
        genalg2->moveToThread(threadGenAlg2);

        //connect(ui->bt_filtr_GPS, SIGNAL(clicked()), gps, SLOT(onOffFiltrKalmana()));
        connect( threadGenAlg2, SIGNAL(started()), genalg2, SLOT(run()) );
        connect( genalg2, SIGNAL(restartThread()), this, SLOT(initAndStartGenAlg() ));

        connect( genalg2, SIGNAL(sendNeuroNet(const NeuroNet)),
                 this, SLOT(acceptNeuroNet(const NeuroNet) ));
        connect( genalg2, SIGNAL(sendNeuroNet(const NeuroNet)),
                 autopilot, SLOT(acceptNeuroNet(const NeuroNet) ));
        connect( genalg2, SIGNAL(currentError(const float&)),
                 this, SLOT(updateError(const float&)) );
        connect( genalg2, SIGNAL(updateEpoha(const int&)),
                 this, SLOT(updateEpoha(const int&)));
        connect( this, SIGNAL(setGenerateMutation(float)),
                 genalg2, SLOT(setGenerateMutation(float) ));
        connect( this, SIGNAL(setMaxValueMutation(float)),
                 genalg2, SLOT(setMaxValueMutation(float) ));
        connect( this, SIGNAL(setProbalityMutation(float)),
                 genalg2, SLOT(setProbalityMutation(float) ));


        threadGenAlg2->start();
    }
}

void MainWindow::updateLatLon(QGeoCoordinate &coord)
{
    if(coord.isValid()) {
        ui->lb_latitude->setText(QString::number(coord.latitude(), 'g', 9));
        ui->lb_longitude->setText(QString::number(coord.longitude(), 'g', 9));
    } else {
        ui->lb_latitude->setText("Invalid");
        ui->lb_longitude->setText("Invalid");
    }
}

void MainWindow::updateDataSens(QVector3D accel, QVector3D gyro)
{
//    qDebug() << "updateSensors Accelerometer:   " << accel;
//    qDebug() << "updateSensors Gyroscope:       " << gyro ;

    ui->lb_x_accelerometer->setText(QString::number(accel.x()));
    ui->lb_y_accelerometer->setText(QString::number(accel.y()));
    ui->lb_z_accelerometer->setText(QString::number(accel.z()));

    ui->lb_x_gyroscope->setText(QString::number(gyro.x()));
    ui->lb_y_gyroscope->setText(QString::number(gyro.y()));
    ui->lb_z_gyroscope->setText(QString::number(gyro.z()));
}

void MainWindow::readFromSlave14(int &data)
{
    ui->lb_slave_14->setText(QString::number(data));
}

void MainWindow::connectedClient(const QString &name)
{
    qDebug() << "MainWindow::connectedClient()";
    if(!name.isNull() && !name.isEmpty()) {
        ui->lb_btserver_on_off->setText("connected " + name);
    } else {
        ui->lb_btserver_on_off->setText("connected");
    }
}

void MainWindow::disconnectedClient(const QString &name)
{
    qDebug() << "MainWindow::disconnectedClient()";
    if(!name.isNull() && !name.isEmpty()) {
        ui->lb_btserver_on_off->setText("disconnected " + name);
    } else {
        ui->lb_btserver_on_off->setText("disconnected");
    }
}

void MainWindow::acceptNeuroNet(const NeuroNet net)
{
    this->neuroNet = net;
}

void MainWindow::updateEpoha(const int &num)
{
    epoha += num;
    ui->lb_current_epoha->setText(QString::number(epoha));
}

void MainWindow::updateCount(const int &num)
{
    ui->lb_current_count->setText(QString::number(num));
}

void MainWindow::updateTrain(const int &num)
{
    ui->lb_current_train->setText(QString::number(num));
}

void MainWindow::updateError(const float &num)
{
    ui->lb_current_error->setText(QString::number(num));
}

void MainWindow::on_hs_max_mut_valueChanged(int value)
{
    //qDebug() << "void MainWindow::on_hs_max_mut_valueChanged(int value) =" << value;
    maxValueMutation = pow(10,value);
    ui->lb_max_mut->setText(QString::number(value));
    //emit setNewMaxValueMutation(r);
    emit setMaxValueMutation(maxValueMutation);
}

void MainWindow::on_hs_probality_mutation_valueChanged(int value)
{

    probalityMutation = ((double)value)/1000.0l;
    ui->lb_probality_mutation->setText(QString::number(probalityMutation));
    emit setProbalityMutation(probalityMutation);
}

void MainWindow::on_hs_generation_valueChanged(int value)
{
    generateMutation = pow(10,value);
    ui->lb_generation->setText(QString::number(value));
    emit setGenerateMutation(generateMutation );

}




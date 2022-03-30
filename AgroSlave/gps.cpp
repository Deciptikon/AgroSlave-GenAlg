#include "gps.h"

GPS::GPS(QObject *parent) : QObject(parent)
{
    msecUpdate = 1000;
    //kalman = false;

//    double dt = (double)msecUpdate/1000.0; // Time step
//    n=6;
//    m=2;

//    Eigen::MatrixXd A(n, n); // System dynamics matrix
//    Eigen::MatrixXd C(m, n); // Output matrix
//    Eigen::MatrixXd Q(n, n); // Process noise covariance
//    Eigen::MatrixXd R(m, m); // Measurement noise covariance
//    Eigen::MatrixXd P(n, n); // Estimate error covariance
//    Eigen::MatrixXd I(n, n);
//    Eigen::MatrixXd x_hat(n,1);
//    qDebug() << "-------------------------------------------------------TEST TEST TEST--------------------------------";

//    A << 1, 0, dt,  0, dt*dt/2,       0,
//         0, 1,  0, dt,       0, dt*dt/2,
//         0, 0,  1,  0,      dt,       0,
//         0, 0,  0,  1,       0,      dt,
//         0, 0,  0,  0,       1,       0,
//         0, 0,  0,  0,       0,       1;

//    C << 1, 0, 0, 0, 0, 0,
//         0, 1, 0, 0, 0, 0;

//    Q << 0.5, 0.5,   0,   0, 0, 0,
//         0.5, 0.5,   0,   0, 0, 0,
//           0,   0, 0.5, 0.5, 0, 0,
//           0,   0, 0.5, 0.5, 0, 0,
//           0,   0,   0,   0, 1, 0,
//           0,   0,   0,   0, 0, 1;

//    R << 5, 0,
//         0, 5;

//    P << 0.1, 0.1,  0.1,  0.1, 0.1, 0.1,
//         0.1, 0.1,  0.1,  0.1, 0.1, 0.1,
//         0.1, 0.1, 1000,  0.1,  10,  10,
//         0.1, 0.1,  0.1, 1000,  10,  10,
//         0.1, 0.1,   10,   10, 100, 0.1,
//         0.1, 0.1,   10,   10, 0.1, 100;

//    I << 1, 0, 0, 0, 0, 0,
//         0, 1, 0, 0, 0, 0,
//         0, 0, 1, 0, 0, 0,
//         0, 0, 0, 1, 0, 0,
//         0, 0, 0, 0, 1, 0,
//         0, 0, 0, 0, 0, 1;

//    x_hat<<0,0,0,0,0,0;
}

void GPS::setMsecUpdate(int value)
{
    msecUpdate = value;
}

void GPS::init()
{
    serial = new QSerialPort();
    serial->setPortName("ttyUSB0");//"ttyACM0"
    serial->open(QIODevice::ReadWrite);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    //connect(serial, SIGNAL(readyRead()), this, SLOT(readPort()));
    //connect();
    int count = 0;
    while(!serial->isOpen() && count<100) {
        serial->open(QIODevice::ReadWrite);
        count++;
    }
    if(serial->isOpen()) {
        qDebug() << "SerialPort is open";

        source = new QNmeaPositionInfoSource(QNmeaPositionInfoSource::RealTimeMode);
        source->setDevice(serial);

        if(source){
            connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(positionUpdate(QGeoPositionInfo)));
            source->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
            source->setUpdateInterval(msecUpdate);
            source->startUpdates();
            qDebug() << "GPS ready and updating";
        }
    } else {
        qDebug() << "SerialPort is not open";
    }

    //fKalmana.init((double)msecUpdate/1000.0, 6, 2);

}

//void GPS::onOffFiltrKalmana()
//{
//    kalman = !kalman;
//}

void GPS::positionUpdate(QGeoPositionInfo info)
{
    qDebug() << "SIGNAL(positionUpdate)";
    //if(kalman) {
        //filtrKalmana(info.coordinate());
    //} else {
        currentCoordinate = info.coordinate();
        latLonToXY(currentCoordinate.latitude(), currentCoordinate.longitude());
    //}
    qDebug() << "coord.latitude(): " << QString::number(currentCoordinate.latitude(), 'g', 9);
    qDebug() << "coord.longitude(): " << QString::number(currentCoordinate.longitude(), 'g', 9);



    QGeoCoordinate c=info.coordinate();
    double lat = c.latitude();
    double lon = c.longitude();
    emit updatePositionGeo(c);
    emit updatePositionXY(this->x, this->y);
    emit updatePositionLatLon(lat, lon);
}

void GPS::latLonToXY(double lat, double lon)
{
    if(lat>360 || lat<0) {
            qDebug() << "Lattityde not range";
        }
        if(lon>MAX_LONGITUDE || lon<-MAX_LONGITUDE) {
            qDebug() << "Longityde not range";
        }

        this->y = RADIUS_EARTH*(lon + 180.0)*(M_PI/180.0);
        this->x = RADIUS_EARTH*qLn(qTan(M_PI/4 + lat*(M_PI/180.0)/2.0));

}

//void GPS::filtrKalmana(QGeoCoordinate coordinate)
//{
//    qDebug() << "-------------------------------------------------------TEST TEST TEST 2--------------------------------";

//    Eigen::MatrixXd y(2,1);

////    if(x_hat.isZero()) {
////        qDebug() << "x_hat.isZero() = true";

////    }

//    if( coordinate.isValid() && !fKalmana.getX_hat().isZero()) {
//        qDebug() << "-------------------------------------------------------TEST TEST TEST 3--------------------------------";
//        currentCoordinate = coordinate;
//        latLonToXY(coordinate.latitude(), coordinate.longitude());
//        y << this->x , this->y;
//        fKalmana.update(y);

//        qDebug() << "-------------------------------------------------------TEST TEST TEST 4--------------------------------";
//        this->x = fKalmana.getX_hat()(0);
//        this->y = fKalmana.getX_hat()(1);
//        //currentCoordinate.setAltitude(coordinate.altitude());
//    } else {
//        qDebug() << "x_hat.isZero() = true";
//        currentCoordinate = coordinate;
//        qDebug() << "-------------------------------------------------------TEST TEST TEST 5--------------------------------";

//        latLonToXY(currentCoordinate.latitude(), currentCoordinate.longitude());
//        //qDebug() << x_hat(0) << x_hat(1) << x_hat(2) << x_hat(3) << x_hat(4) << x_hat(5);

//        qDebug() << this->x << this->y;
//        Eigen::MatrixXd xnew_hat(6,1);
//        xnew_hat<< (this->x), (this->y), 0, 0, 0, 0;
//        qDebug() << "-------------------------------------------------------TEST TEST TEST 6--------------------------------";
//        fKalmana.setX_hat(xnew_hat);
//        //x_hat << this->x, this->y, 0, 0, 0, 0;
//        //qDebug() << x_hat(0) << x_hat(1) << x_hat(2) << x_hat(3) << x_hat(4) << x_hat(5);
//        qDebug() << "-------------------------------------------------------TEST TEST TEST 7--------------------------------";
//    }
//}

//bool GPS::getKalman() const
//{
//    return kalman;
//}

//void GPS::setKalman(bool value)
//{
//    kalman = value;
//}

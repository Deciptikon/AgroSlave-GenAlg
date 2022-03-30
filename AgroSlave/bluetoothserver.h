#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <qbluetoothserviceinfo.h>
#include <qbluetoothaddress.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QDebug>

#include <QGeoCoordinate>

QT_FORWARD_DECLARE_CLASS(QBluetoothServer)
QT_FORWARD_DECLARE_CLASS(QBluetoothSocket)

QT_USE_NAMESPACE

//! [declaration]
class BluetoothServer : public QObject
{
    Q_OBJECT

public:
    explicit BluetoothServer(QObject *parent = 0);
    ~BluetoothServer();

    void startServer(const QBluetoothAddress &localAdapter = QBluetoothAddress());
    void stopServer();


    char xorSumma(QByteArray &qba);

public slots:
    void sendMessage(const QByteArray &message);
    void sendCoord(double &x, double &y);
    //void sendCoord(QGeoCoordinate &coord);

signals:
    void messageReceived(const QByteArray &message);
    void clientConnected(const QString &name);
    void clientDisconnected(const QString &name);

private slots:
    void clientConnected();
    void clientDisconnected();
    void readSocket();

private:
    QBluetoothServer *rfcommServer;
    QBluetoothServiceInfo serviceInfo;
    QList<QBluetoothSocket *> clientSockets;
};
//! [declaration]

#endif // CHATSERVER_H

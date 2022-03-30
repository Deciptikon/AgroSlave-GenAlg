#include "bluetoothserver.h"

#include <qbluetoothserver.h>
#include <qbluetoothsocket.h>
#include <qbluetoothlocaldevice.h>

//! [Service UUID]
static const QLatin1String serviceUuid("e8e10f95-1170-4b27-9ccf-02010264e9c8");
//! [Service UUID]

BluetoothServer::BluetoothServer(QObject *parent)
:   QObject(parent), rfcommServer(0)
{
}

BluetoothServer::~BluetoothServer()
{
    stopServer();
}

void BluetoothServer::startServer(const QBluetoothAddress& localAdapter)
{
    if (rfcommServer)
        return;

    //! [Create the server]
    rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(rfcommServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));
    bool result = rfcommServer->listen(localAdapter);
    if (!result) {
        qWarning() << "Cannot bind chat server to" << localAdapter.toString();
        return;
    }
    qDebug() << "BluetoothServer:: new Server created";
    //! [Create the server]

    //serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceRecordHandle, (uint)0x00010010);

    //! [Class Uuuid must contain at least 1 entry]
    QBluetoothServiceInfo::Sequence classId;

    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
    serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,
                             classId);

    classId.prepend(QVariant::fromValue(QBluetoothUuid(serviceUuid)));

    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);
    //! [Class Uuuid must contain at least 1 entry]


    //! [Service name, description and provider]
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("Bt Server castom"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             tr("Example bluetooth chat server"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("deciptikon"));
    //! [Service name, description and provider]

    //! [Service UUID set]
    serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));
    //! [Service UUID set]

    //! [Service Discoverability]
    QBluetoothServiceInfo::Sequence publicBrowse;
    publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
    serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             publicBrowse);
    //! [Service Discoverability]

    //! [Protocol descriptor list]
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    //protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
    //protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
             << QVariant::fromValue(quint8(rfcommServer->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);
    //! [Protocol descriptor list]

    //! [Register service]
    serviceInfo.registerService(localAdapter);
    //! [Register service]
}

//! [stopServer]
void BluetoothServer::stopServer()
{
    // Unregister service
    serviceInfo.unregisterService();

    // Close sockets
    qDeleteAll(clientSockets);

    // Close server
    delete rfcommServer;
    rfcommServer = 0;
}

char BluetoothServer::xorSumma(QByteArray &qba)
{
    char s = 0x00;
    for (int i=0; i<qba.length(); i++) {
        s ^= qba.at(i);
    }
    return s;
}
//! [stopServer]

//! [sendMessage]
void BluetoothServer::sendMessage(const QByteArray &message)
{
    //QByteArray text = message.toUtf8() + '\n';

    if(message.length()>0) {
        foreach (QBluetoothSocket *socket, clientSockets)
            socket->write(message);
        if(message.at(message.length()-1) != '\n') {
            QByteArray n;
            n.append('\n');
            foreach (QBluetoothSocket *socket, clientSockets)
                socket->write(n);
        }
        qDebug() << message.toHex() << '\n';
    }

}

void BluetoothServer::sendCoord(double &x, double &y)
{
    QByteArray mes, dat;
    //mes.append('0x24');

    dat.append(QString::number(x,'g',9));
    dat.append("|");
    dat.append(QString::number(y,'g',9));
    dat.append("|");


    char crc = xorSumma(dat);

    mes.append(dat);
    mes.append(crc);
    mes.append('\n');

    qDebug() << "BluetoothServer::sendCoord = " << QString::number(x,'g',9) << "|"
                                                << QString::number(y,'g',9) << "|"
                                                << (int)crc << "\\" << "n";

    sendMessage(mes);
}
//! [sendMessage]

//! [clientConnected]
void BluetoothServer::clientConnected()
{
    QBluetoothSocket *socket = rfcommServer->nextPendingConnection();
    if (!socket)
        return;

    qDebug() << "ChatServer::clientConnected()";
    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    clientSockets.append(socket);
    emit clientConnected(socket->peerName());
}
//! [clientConnected]

//! [clientDisconnected]
void BluetoothServer::clientDisconnected()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;

    qDebug() << "BluetoothServer::clientDisconnected()";
    emit clientDisconnected(socket->peerName());

    clientSockets.removeOne(socket);

    socket->deleteLater();
}
//! [clientDisconnected]

//! [readSocket]
void BluetoothServer::readSocket()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;


    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        emit messageReceived(line);

        qDebug() << "BluetoothServer::readSocket.available = " << line.size();

    }
}
//! [readSocket]

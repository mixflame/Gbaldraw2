#include "server.h"
#include <thread.h>

Server::Server(QObject *parent) : QTcpServer(parent)
{

}

void Server::incomingConnection(qintptr socketDescriptor)
{
//    Thread *thread = new Thread(socketDescriptor, this);
//    thread->points = points;
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//    thread->start();
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
//        emit error(tcpSocket.error());
        return;
    }
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    for(int i = 0; i < this->points->size(); i++) {
        QJsonObject point = this->points->at(i).toObject();
        QJsonDocument doc(point);
        out << doc.toJson() << "\n";
        tcpSocket.write(block);
    }

    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();
}

void Server::startServer() {
    if(!this->listen(QHostAddress::Any, 9999))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server started!";
    }
}

//void Server::newConnection()
//{
//    // need to grab the socket
//    QTcpSocket *socket = server->nextPendingConnection();

//    socket->write("Hello client\r\n");
//    socket->flush();

//    socket->waitForBytesWritten(3000);

//    socket->close();
//}

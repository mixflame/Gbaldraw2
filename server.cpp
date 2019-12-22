#include "server.h"
#include <thread.h>

Server::Server(QObject *parent) : QTcpServer(parent)
{

}

void Server::incomingConnection(qintptr socketDescriptor)
{
    Thread *thread = new Thread(socketDescriptor, this);
    thread->points = points;
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
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

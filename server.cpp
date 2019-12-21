#include "server.h"
#include <thread.h>

Server::Server(QObject *parent) : QTcpServer(parent)
{
    fortunes << tr("You've been leading a dog's life. Stay off the furniture.")
                 << tr("You've got to think about tomorrow.")
                 << tr("You will be surprised by a loud noise.")
                 << tr("You will feel hungry again in another hour.")
                 << tr("You might have mail.")
                 << tr("You cannot kill time without injuring eternity.")
                 << tr("Computers are not intelligent. They only think they are.");
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QString fortune = fortunes.at(QRandomGenerator::global()->bounded(fortunes.size()));
    Thread *thread = new Thread(socketDescriptor, fortune, this);
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

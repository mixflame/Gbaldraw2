#include "thread.h"

Thread::Thread(int socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{
}


void Thread::run()
{
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    for(int i = 0; i < this->points->size(); i++) {
        QJsonObject point = this->points->at(i).toObject();
        QJsonDocument doc(point);
        out << doc.toJson();
        tcpSocket.write(block);
    }

    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();
}

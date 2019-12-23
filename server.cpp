#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent)
{

}

//void Server::incomingConnection(qintptr socketDescriptor)
//{
////    Thread *thread = new Thread(socketDescriptor, this);
////    thread->points = points;
////    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
////    thread->start();
//    QTcpSocket tcpSocket;
//    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
////        emit error(tcpSocket.error());
//        return;
//    }
//    QByteArray block;
//    QDataStream out(&block, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_5_12);

//    for(int i = 0; i < this->points->size(); i++) {
//        QJsonObject point = this->points->at(i).toObject();
//        QJsonDocument doc(point);
//        out << doc.toJson() << "\n";
//        tcpSocket.write(block);
//    }

////    tcpSocket.disconnectFromHost();
////    tcpSocket.waitForDisconnected();
//}

void Server::startServer() {
    if(!this->listen(QHostAddress::Any, 9999))
    {
        logMessage(QStringLiteral("Server could not start."));
    }
    else
    {
        logMessage(QStringLiteral("Server started."));
    }
}

void Server::stopServer() {
    foreach(ServerWorker* worker, m_clients)
        delete worker;
    if(this->isListening()) {
        this->close();
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    // This method will get called every time a client tries to connect.
    // We create an object that will take care of the communication with this client
    ServerWorker *worker = new ServerWorker(this);
    // we attempt to bind the worker to the client
    if (!worker->setSocketDescriptor(socketDescriptor)) {
        // if we fail we clean up
        worker->deleteLater();
        return;
    }
    // connect the signals coming from the object that will take care of the
    // communication with this client to the slots in the central server
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&Server::userDisconnected, this, worker));
    connect(worker, &ServerWorker::error, this, std::bind(&Server::userError, this, worker));
    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&Server::jsonReceived, this, worker, std::placeholders::_1));
    connect(worker, &ServerWorker::logMessage, this, &Server::logMessage);
    // we append the new worker to a list of all the objects that communicate to a single client
    m_clients.append(worker);
    // we log the event
    logMessage(QStringLiteral("New client Connected"));

        for(int i = 0; i < this->scribbleArea->points.size(); i++) {
            QJsonObject point = this->scribbleArea->points.at(i).toObject();
            worker->sendJson(point);
        }
}

void Server::logMessage(const QString &msg) {
    qDebug() << msg;
}

void Server::jsonReceived(ServerWorker *sender, const QJsonObject &doc) {
    // add it to my own canvas and broadcast
    scribbleArea->addClick(doc["x"].toInt(), doc["y"].toInt(), doc["dragging"].toBool(), doc["r"].toInt(), doc["g"].toInt(), doc["b"].toInt(), doc["width"].toInt(), doc["username"].toString());
    scribbleArea->redraw();
    broadcast(doc, sender);
}

void Server::userError(ServerWorker *sender) {
    logMessage(sender->userName() + QStringLiteral(" error"));
}

void Server::userDisconnected(ServerWorker *sender) {
    logMessage(sender->userName() + QStringLiteral(" disconnected"));
    m_clients.remove(m_clients.indexOf(sender));
    delete sender;
}
void Server::sendJson(ServerWorker *destination, const QJsonObject &message)
{
    Q_ASSERT(destination); // make sure destination is not null
    destination->sendJson(message); // call directly the worker method
}

void Server::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    // iterate over all the workers that interact with the clients
    for (ServerWorker *worker : m_clients) {
        if (worker == exclude)
            continue; // skip the worker that should be excluded
        sendJson(worker, message); //send the message to the worker
    }
}

void Server::broadcast(const QJsonObject &message)
{
    // iterate over all the workers that interact with the clients
    for (ServerWorker *worker : m_clients) {
        sendJson(worker, message); //send the message to the worker
    }
}

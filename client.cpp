#include "client.h"

Client::Client(QObject *parent)
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
{
    // Forward the connected and disconnected signals
    connect(m_clientSocket, &QTcpSocket::connected, this, &Client::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &Client::disconnected);
    // connect readyRead() to the slot that will take care of reading the data in
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    // Forward the error signal, QOverload is necessary as error() is overloaded, see the Qt docs
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::error);
    // Reset the m_loggedIn variable when we disconnect. Since the operation is trivial we use a lambda instead of creating another slot
    connect(m_clientSocket, &QTcpSocket::disconnected, this, [this]()->void{m_loggedIn = false;});
}

void Client::sendPoint(const QJsonObject point)
{
    // create a QDataStream operating on the socket
    QDataStream clientStream(m_clientSocket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    clientStream.setVersion(QDataStream::Qt_5_12);
    // Create the JSON we want to send
    // send the JSON using QDataStream
    clientStream << QJsonDocument(point).toJson(QJsonDocument::Compact);
}

void Client::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}
void Client::connectToServer(const QHostAddress &address, quint16 port)
{
    // on client connect, destroy internal image
    QJsonArray points;

    QHash<QString, int> nameHash;
    QStringList layerOrder;
    QJsonObject layers;

    scribbleArea->points = points;
    scribbleArea->nameHash = nameHash;
    scribbleArea->layerOrder = layerOrder;
    scribbleArea->layers = layers;

    m_clientSocket->connectToHost(address, port);
//    bool connected = (m_clientSocket->state() == QTcpSocket::ConnectedState);
//    qDebug() << "Connected: " + QString::number(connected);


}


void Client::jsonReceived(const QJsonObject &obj)
{
    // the JSON is a point, add to canvas
    qDebug() << "JSON received.";
    scribbleArea->addClick(obj["x"].toInt(), obj["y"].toInt(), obj["dragging"].toBool(), obj["r"].toInt(), obj["g"].toInt(), obj["b"].toInt(), obj["width"].toInt(), obj["username"].toString());
    scribbleArea->redraw();
}

void Client::onReadyRead()
{
    // prepare a container to hold the UTF-8 encoded JSON we receive from the socket
    QByteArray jsonData;
    // create a QDataStream operating on the socket
    QDataStream socketStream(m_clientSocket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    socketStream.setVersion(QDataStream::Qt_5_7);
    // start an infinite loop
    for (;;) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();
        // we try to read the JSON data
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            // we successfully read some data
            // we now need to make sure it's in fact a valid JSON
            QJsonParseError parseError;
            // we try to create a json document with the data we received
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                // if the data was indeed valid JSON
                if (jsonDoc.isObject()) // and is a JSON object
                    jsonReceived(jsonDoc.object()); // parse the JSON
            }
            // loop and try to read more JSONs if they are available
        } else {
            // the read failed, the socket goes automatically back to the state it was in before the transaction started
            // we just exit the loop and wait for more data to become available
            break;
        }
    }
}

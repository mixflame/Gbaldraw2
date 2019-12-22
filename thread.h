#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>



class Thread : public QThread
{
    Q_OBJECT

public:
    Thread(int socketDescriptor, QObject *parent);

    void run() override;

    QJsonArray* points;

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    int socketDescriptor;
};

#endif // THREAD_H

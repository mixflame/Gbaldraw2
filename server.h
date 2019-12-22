#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QRandomGenerator>
#include <QJsonArray>
#include <QJsonObject>


class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    QString serverPassword;
    QString serverPort;
    void startServer();

    QJsonArray* points;

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:

};

#endif // SERVER_H

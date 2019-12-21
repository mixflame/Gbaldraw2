#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QRandomGenerator>


class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    QString serverPassword;
    QString serverPort;
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QStringList fortunes;

};

#endif // SERVER_H

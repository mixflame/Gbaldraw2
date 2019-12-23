#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <serverworker.h>
#include <scribblearea.h>


class Server : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(Server)
public:
    explicit Server(QObject *parent = 0);
    void broadcast(const QJsonObject &message);
    ScribbleArea *scribbleArea;
    int serverPort;
protected:
    void incomingConnection(qintptr socketDescriptor) override;
signals:

public slots:
    void startServer();
    void stopServer();
    void logMessage(const QString &msg);
private slots:
    void broadcast(const QJsonObject &message, ServerWorker *exclude);
    void jsonReceived(ServerWorker *sender, const QJsonObject &doc);
    void userDisconnected(ServerWorker *sender);
    void userError(ServerWorker *sender);

private:
    void jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &doc);
    void jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &doc);
    void sendJson(ServerWorker *destination, const QJsonObject &message);
    QVector<ServerWorker *> m_clients;
};

#endif // SERVER_H

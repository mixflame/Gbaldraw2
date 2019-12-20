#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>

class TCPServer : public QObject
{
    Q_OBJECT
public:
    explicit TCPServer(QObject *parent = nullptr);

signals:

};

#endif // TCPSERVER_H

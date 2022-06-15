#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

class TCP_SERVER : public QObject
{
    Q_OBJECT
public:
    explicit TCP_SERVER(QObject *parent = 0);
    QByteArray ReceivedBuffer;
signals:

public slots:
    void newConnection();

private:
    QTcpServer *TCPServer;

};

#endif // TCP_SERVER_H

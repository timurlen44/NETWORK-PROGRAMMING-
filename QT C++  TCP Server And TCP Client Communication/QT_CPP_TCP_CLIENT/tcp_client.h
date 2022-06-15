#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class TCP_CLIENT : public QObject
{
    Q_OBJECT
public:
    explicit TCP_CLIENT(QObject *parent = nullptr);
    void Connect();
    QString SenderBuffer;
    QByteArray ReceivedBuffer;

signals:

public slots:
private:
    QTcpSocket *TCPClientSocket;

};

#endif // TCP_CLIENT_H

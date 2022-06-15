#include "tcp_server.h"

TCP_SERVER::TCP_SERVER(QObject *parent)
    : QObject{parent}
{
    TCPServer = new QTcpServer(this);

    connect(TCPServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!TCPServer->listen(QHostAddress::Any, 54000))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";
    }
}


void TCP_SERVER::newConnection()
{
    qDebug()<<"New connection is available\n";
    QTcpSocket *TCPServerSocket = TCPServer->nextPendingConnection();
    if(TCPServerSocket != nullptr)
    {
        TCPServerSocket->waitForReadyRead(50000);
        //qDebug() <<TCPServerSocket->bytesAvailable();
        ReceivedBuffer = TCPServerSocket->readAll();
        if(ReceivedBuffer.isEmpty()){
            qDebug()<<"There is no data\n";
        }
        else
        {
            qDebug()<<"Received Data: "<<ReceivedBuffer<<"\n";
        }

        TCPServerSocket->write("Hello Client!");
        TCPServerSocket->flush();
        TCPServerSocket->waitForBytesWritten(3000);
        TCPServerSocket->close();
    }

}


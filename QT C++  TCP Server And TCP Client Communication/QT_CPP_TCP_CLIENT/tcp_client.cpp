#include "tcp_client.h"

TCP_CLIENT::TCP_CLIENT(QObject *parent)
    : QObject{parent}
{

}

void TCP_CLIENT::Connect(){
    qDebug()<<"Data: "<<SenderBuffer<<"\n";
    TCPClientSocket = new QTcpSocket(this);
    qDebug()<<"Try To Connect To Server\n";
    TCPClientSocket->connectToHost("127.0.0.1",54000);
    if(TCPClientSocket->waitForConnected(3000)){
        qDebug()<<"Connected To Server !!!\n";
        TCPClientSocket->write(SenderBuffer.toStdString().c_str());
        //TCPClientSocket->write("hello server\r\n\r\n\r\r");
        TCPClientSocket->waitForBytesWritten(1000);
        TCPClientSocket->waitForReadyRead(3000);
        qDebug()<<TCPClientSocket->bytesAvailable();
        ReceivedBuffer = TCPClientSocket->readAll();
        if(ReceivedBuffer.isEmpty()){
            qDebug()<<"There is no data\n";
        }
        else
        {
            qDebug()<<"Received Data: "<<ReceivedBuffer<<"\n";
        }
    }
    else{
        qDebug()<<"Couldn't Connected To Server !!!\n";
    }
    TCPClientSocket->close();

}

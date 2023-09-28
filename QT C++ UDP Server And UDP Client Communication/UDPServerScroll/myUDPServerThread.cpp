#include "myUDPServerThread.h"
#include <QMutex>
myUDPServerThread::myUDPServerThread(QObject *parent)
    : QThread{parent}
{
    m_udpServerSocket = new QUdpSocket(this);
    if(m_udpServerSocket->bind(QHostAddress::Any, 54000))
    {
        qDebug()<<"UDP Socket Bind";
    }
    else
    {
        qDebug()<<"UDP Socket Didn't Bind";
    }
    connect(m_udpServerSocket,SIGNAL(readyRead()), this,SLOT(readyReadSlot()));
}

void myUDPServerThread::run()
{
    while(true)
    {
        if(m_flag)
        {
            m_flag = false;

        }
    }
}

void myUDPServerThread::readyReadSlot()
{

    m_flag = true;

    QByteArray receiverBuffer;
    receiverBuffer.resize(m_udpServerSocket->pendingDatagramSize());

    QHostAddress senderDeviceIPAddr;
    quint16 senderDevicePort;


    m_udpServerSocket->readDatagram(receiverBuffer.data(),receiverBuffer.size(),&senderDeviceIPAddr,&senderDevicePort);
    //qDebug()<<"Message From: "<< senderDeviceIPAddr.toString()<<":"<<senderDevicePort;
    //qDebug()<<"Message: "<< receiverBuffer.toInt();
    emit scrollValueChanged(receiverBuffer.toInt());
}



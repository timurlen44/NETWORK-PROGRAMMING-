#include "myUDPClientThread.h"

myUDPClientThread::myUDPClientThread(QObject *parent)
    : QThread{parent}
{
     m_udpClientSocket = new QUdpSocket(this);
}

void myUDPClientThread::run()
{
    while(true)
    {
        if(m_flag == true)
        {
            m_flag = false;
            QString sendBuffer_qstr = QString::number(m_valueOfScroll);
            QByteArray sendBuffer_qba(sendBuffer_qstr.toUtf8());
            m_udpClientSocket->writeDatagram(sendBuffer_qba, m_serverAddr, m_serverPort);
        }
    }
}

void myUDPClientThread::scrollValueChanged(int value)
{
    m_flag = true;
    m_valueOfScroll = value;
}

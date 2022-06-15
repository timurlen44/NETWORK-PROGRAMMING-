#include <QCoreApplication>
#include "tcp_client.h"
using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TCP_CLIENT TCPClient;
    TCPClient.SenderBuffer = "Hello";
    QTextStream in(stdin);

    while (true)
    {
        qDebug()<<"What do you want to say to server: ";
        in >>TCPClient.SenderBuffer;
        TCPClient.Connect();
    }

    return a.exec();
}

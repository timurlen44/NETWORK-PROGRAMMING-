#include <QCoreApplication>
#include "tcp_server.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TCP_SERVER TCPServer;
    return a.exec();
}

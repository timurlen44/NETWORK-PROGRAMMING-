#include "mainwindow.h"
#include "myUDPServerThread.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    myUDPServerThread udpServer;
    udpServer.start();
    QObject::connect(&udpServer,SIGNAL(scrollValueChanged(int)),&w,SLOT(scrollValueChanged(int)));
    w.show();
    return a.exec();
}

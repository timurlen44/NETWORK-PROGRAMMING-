#include "mainwindow.h"
#include "myUDPClientThread.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    myUDPClientThread udpClient;
    udpClient.start();

    QObject::connect(&w,SIGNAL(scrollValueChanged(int)),&udpClient,SLOT(scrollValueChanged(int)));

    w.show();
    return a.exec();
}

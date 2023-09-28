#ifndef MYUDPCLIENTTHREAD_H
#define MYUDPCLIENTTHREAD_H

#include <QThread>
#include <QObject>
#include <QUdpSocket>

class myUDPClientThread : public QThread
{
    Q_OBJECT
public:
    explicit myUDPClientThread(QObject *parent = nullptr);
    void run() override;

private:
    bool m_flag = false;
    int m_valueOfScroll;
    QHostAddress m_serverAddr{"127.0.0.1"};
    quint16 m_serverPort = 54000;
    QUdpSocket *m_udpClientSocket = nullptr;

public slots:
    void scrollValueChanged(int value);
};

#endif // MYUDPCLIENTTHREAD_H

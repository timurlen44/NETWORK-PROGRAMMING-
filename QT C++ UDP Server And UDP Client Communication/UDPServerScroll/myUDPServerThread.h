#ifndef MYUDPSERVERTHREAD_H
#define MYUDPSERVERTHREAD_H

#include <QThread>
#include <QObject>
#include <QUdpSocket>
class myUDPServerThread : public QThread
{
    Q_OBJECT
public:
    explicit myUDPServerThread(QObject *parent = nullptr);
    void run() override;

private:
    bool m_flag = false;
    int m_scrollValue = 0;
    QUdpSocket *m_udpServerSocket = nullptr;

public slots:
    void readyReadSlot();

signals:
    void scrollValueChanged(int value);

};

#endif // MYUDPSERVERTHREAD_H

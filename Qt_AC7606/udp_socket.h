#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <QHostAddress>
#include <QtNetwork>
#include <QUdpSocket>

#include "image_viewer.h"

class udp_socket : public QObject
{
    Q_OBJECT
public:
    udp_socket(QObject * parent = nullptr);
    ~udp_socket();

    void Send_data(quint32);
    void Init_Socket(QString, QString);
    void Close_Socket();
    QByteArray *Get_data(QString, QString);

    void timerEvent(QTimerEvent *);

    QAbstractSocket::SocketState State();

signals:
    void Ready_Read();
    void Send_Result_Array(QByteArray *);

private:
    std::shared_ptr<QUdpSocket> obj_socket;
    std::shared_ptr<QByteArray> a_main_data;
    quint32 i_size_main_array = 0;
};
#endif // UDP_SOCKET_H

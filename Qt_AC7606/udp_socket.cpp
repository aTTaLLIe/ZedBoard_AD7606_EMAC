#include "udp_socket.h"

udp_socket::udp_socket(QObject *parent) : QObject(parent)
{
    obj_socket = std::make_shared<QUdpSocket>();
    a_main_data = std::make_shared<QByteArray>();

    connect(obj_socket.get(), &QUdpSocket::readyRead, [this](){
        emit Ready_Read();
    });
}

udp_socket::~udp_socket()
{

}

void udp_socket::timerEvent(QTimerEvent *)
{
    Send_data(0);
    emit Send_Result_Array(a_main_data.get());
}


void udp_socket::Init_Socket(QString s_ip = "192.172.0.80", QString s_port = "1024")
{
    obj_socket->connectToHost(QHostAddress(s_ip), s_port.toUInt());
}

void udp_socket::Close_Socket()
{
    obj_socket->disconnect();
}

void udp_socket::Send_data(quint32 i_message)
{
    if(i_message == 2 || i_message == 1)
    {
        a_main_data->clear();
    }
    if(obj_socket->state() == QAbstractSocket::ConnectedState)
    {
           QByteArray a_data;
           QDataStream ds_out(&a_data, QIODevice::WriteOnly);
           ds_out.setVersion(QDataStream::Qt_5_11);

           ds_out << i_message;
           obj_socket->write(a_data);
    }
}

QAbstractSocket::SocketState udp_socket::State()
{
    return obj_socket->state();
}


QByteArray *udp_socket::Get_data(QString s_ip = "192.172.0.80", QString s_port = "1024")
{
    QByteArray a_input_data;
    quint32 i_size_frame = obj_socket->pendingDatagramSize();
    QHostAddress obj_address = QHostAddress(s_ip);
    quint16 i_port = s_port.toUInt();

    a_input_data.resize(i_size_frame);
    obj_socket->readDatagram(a_input_data.data(), i_size_frame, &obj_address, &i_port);
//    QDataStream ds_in(&a_input_data, QIODevice::ReadOnly);
//    ds_in.setVersion(QDataStream::Qt_5_11);
    a_main_data->append(a_input_data);
    if(i_size_frame <= 4)
    {
        return a_main_data.get();
    }
    else
    {
        return nullptr;
    }
}

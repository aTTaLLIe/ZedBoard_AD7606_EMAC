#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    p_socket = std::make_shared<udp_socket>();
    p_timer = std::make_shared<QBasicTimer>();

    ui->IP_edit->setText("192.172.0.80");
    ui->Port_edit->setText("1024");
    ui->one_frame_but->setEnabled(false);
    ui->contin_meas_but->setEnabled(false);
    ui->stop_meas_but->setEnabled(false);

//***********************************************
    ui->graph_lb->addGraph();
    ui->graph_lb->xAxis->setRange(0, 1000);
    ui->graph_lb->yAxis->setRange(-10, 10);
    ui->graph_lb->yAxis->setOffset(-430);
    ui->graph_lb->xAxis->setOffset(-205);
    ui->graph_lb->replot();
//***********************************************
    connect(ui->connect_but, &QPushButton::clicked, [this](){
        p_socket->Init_Socket(ui->IP_edit->text(), ui->Port_edit->text());
        if(p_socket->State() == QUdpSocket::ConnectedState)
        {
            QColor col = QColor(Qt::green);
            if(col.isValid()) {
               QString qss = QString("background-color: %1").arg(col.name());
               ui->connect_but->setStyleSheet(qss);
            }
            ui->one_frame_but->setEnabled(true);
            ui->contin_meas_but->setEnabled(true);
            ui->stop_meas_but->setEnabled(true);
        }
    });

    connect(ui->disconnect_but, &QPushButton::clicked, [this](){
        p_socket->Close_Socket();
        QColor col = QColor(Qt::lightGray);
        if(col.isValid()) {
           QString qss = QString("background-color: %1").arg(col.name());
           ui->connect_but->setStyleSheet(qss);
        }
        ui->one_frame_but->setEnabled(false);
        ui->contin_meas_but->setEnabled(false);
        ui->stop_meas_but->setEnabled(false);
    });


    connect(ui->one_frame_but, &QPushButton::clicked, [this](){
        p_timer->start(5, p_socket.get());
        p_socket->Send_data(1);
    });
    connect(ui->contin_meas_but, &QPushButton::clicked, [this](){
        p_timer->start(100, p_socket.get());
        p_socket->Send_data(2);
    });
    connect(ui->stop_meas_but, &QPushButton::clicked, [this](){
        p_timer->stop();
        p_socket->Send_data(0);
    });

    connect(p_socket.get(), &udp_socket::Send_Result_Array, [this](QByteArray *a_voltage_array){
        p_timer->stop();
        Proccesing_Data(a_voltage_array);
    });
    connect(p_socket.get(), &udp_socket::Ready_Read, [this](){
        p_socket->Get_data(ui->IP_edit->text(), ui->Port_edit->text());
    });

    connect(ui->clear_but, &QPushButton::clicked, [this](){
        ui->graph_lb->clearPlottables();
        ui->graph_lb->replot();
        ui->OneMeas_lb->clear();
    });
    connect(ui->close_but, &QPushButton::clicked, [this](){close();});
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::Build_Graph(QVector<double> *v_y_data, quint32 i_size)
{
    QVector<double> x(i_size);
    x[0] = 0;
    double db_step = 100/double(i_size);
    for(int i = 0; i < i_size-1; i++)
    {
        x[i+1] = x[i] + db_step;
    }
    ui->graph_lb->clearPlottables();
    ui->graph_lb->xAxis->setRange(0, 100);
    QCPGraph *p_graphic = new QCPGraph(ui->graph_lb->xAxis, ui->graph_lb->yAxis);
    p_graphic->setData(x, *v_y_data);
    p_graphic->setPen(QPen(QColor(Qt::red)));
    p_graphic->setAntialiased(false);
    p_graphic->setLineStyle(QCPGraph::lsLine);
    ui->graph_lb->replot();
}


void MainWindow::Proccesing_Data(QByteArray *a_input_data)
{
    if(!(*a_input_data).isEmpty())
    {
        if(a_input_data->size() <= 4)
        {
            float f_voltage = ConvertByteArray(*a_input_data);
            ui->OneMeas_lb->setText(QString::number(f_voltage));
        }
        else
        {
            qint32 i_size = a_input_data->size()/4;
            QVector<double> y_vector(i_size);

            char *p_input_data = a_input_data->data();
            for(int i = 0; i < i_size; i++)
            {
                ch_to_flt.data_char[0] = *(p_input_data + i*4);
                ch_to_flt.data_char[1] = *(p_input_data + i*4 + 1);
                ch_to_flt.data_char[2] = *(p_input_data + i*4 + 2);
                ch_to_flt.data_char[3] = *(p_input_data + i*4 + 3);

                y_vector[i] = ch_to_flt.data_float;
            }

            Build_Graph(&y_vector, y_vector.size());
        }
    }
}

float MainWindow::ConvertByteArray(QByteArray data)
{
    ch_to_flt.data_char[0] = data[0];
    ch_to_flt.data_char[1] = data[1];
    ch_to_flt.data_char[2] = data[2];
    ch_to_flt.data_char[3] = data[3];

    return ch_to_flt.data_float;
}


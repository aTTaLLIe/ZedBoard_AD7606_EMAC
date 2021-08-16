#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>

#include "image_viewer.h"
#include "udp_socket.h"
#include "QCustomPlot.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void Close_Socket();
    void Build_Graph(QVector<double> *, quint32);
    void Proccesing_Data(QByteArray *);
    float ConvertByteArray(QByteArray);


private:
    Ui::MainWindow *ui;
    std::shared_ptr<udp_socket> p_socket;
    std::shared_ptr<QBasicTimer> p_timer;

    QString s_ip;
    QString s_message;

    union convert_data
    {
        char data_char[4];
        float data_float;
        quint32 data_quint32;
    } ch_to_flt;
};
#endif // MAINWINDOW_H

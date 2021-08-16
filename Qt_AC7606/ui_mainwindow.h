/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *IP_edit;
    QLabel *label;
    QLabel *label_2;
    QPushButton *stop_meas_but;
    QPushButton *close_but;
    QLabel *label_3;
    QPushButton *connect_but;
    QPushButton *disconnect_but;
    QPushButton *clear_but;
    QPushButton *one_frame_but;
    QLabel *label_4;
    QLineEdit *Port_edit;
    QLabel *label_6;
    QPushButton *contin_meas_but;
    QCustomPlot *graph_lb;
    QLineEdit *OneMeas_lb;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1221, 651);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        IP_edit = new QLineEdit(centralwidget);
        IP_edit->setObjectName(QStringLiteral("IP_edit"));
        IP_edit->setGeometry(QRect(940, 40, 151, 21));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(1010, 20, 55, 16));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(1030, 70, 61, 16));
        stop_meas_but = new QPushButton(centralwidget);
        stop_meas_but->setObjectName(QStringLiteral("stop_meas_but"));
        stop_meas_but->setGeometry(QRect(940, 180, 101, 28));
        close_but = new QPushButton(centralwidget);
        close_but->setObjectName(QStringLiteral("close_but"));
        close_but->setGeometry(QRect(1072, 180, 101, 28));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 10, 91, 16));
        connect_but = new QPushButton(centralwidget);
        connect_but->setObjectName(QStringLiteral("connect_but"));
        connect_but->setGeometry(QRect(1072, 140, 101, 28));
        disconnect_but = new QPushButton(centralwidget);
        disconnect_but->setObjectName(QStringLiteral("disconnect_but"));
        disconnect_but->setGeometry(QRect(1072, 100, 101, 28));
        clear_but = new QPushButton(centralwidget);
        clear_but->setObjectName(QStringLiteral("clear_but"));
        clear_but->setGeometry(QRect(430, 560, 93, 28));
        one_frame_but = new QPushButton(centralwidget);
        one_frame_but->setObjectName(QStringLiteral("one_frame_but"));
        one_frame_but->setGeometry(QRect(940, 140, 101, 28));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(30, 80, 81, 16));
        Port_edit = new QLineEdit(centralwidget);
        Port_edit->setObjectName(QStringLiteral("Port_edit"));
        Port_edit->setGeometry(QRect(1100, 40, 71, 21));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(1120, 20, 31, 16));
        contin_meas_but = new QPushButton(centralwidget);
        contin_meas_but->setObjectName(QStringLiteral("contin_meas_but"));
        contin_meas_but->setGeometry(QRect(940, 100, 101, 28));
        graph_lb = new QCustomPlot(centralwidget);
        graph_lb->setObjectName(QStringLiteral("graph_lb"));
        graph_lb->setGeometry(QRect(30, 100, 891, 441));
        OneMeas_lb = new QLineEdit(centralwidget);
        OneMeas_lb->setObjectName(QStringLiteral("OneMeas_lb"));
        OneMeas_lb->setGeometry(QRect(30, 30, 101, 21));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QApplication::translate("MainWindow", "Enter IP", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Command", nullptr));
        stop_meas_but->setText(QApplication::translate("MainWindow", "Stop Meas.", nullptr));
        close_but->setText(QApplication::translate("MainWindow", "Close", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "One Meas.", nullptr));
        connect_but->setText(QApplication::translate("MainWindow", "Connect", nullptr));
        disconnect_but->setText(QApplication::translate("MainWindow", "Disconnect", nullptr));
        clear_but->setText(QApplication::translate("MainWindow", "Clear", nullptr));
        one_frame_but->setText(QApplication::translate("MainWindow", "One Meas.", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Graph", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Port", nullptr));
        contin_meas_but->setText(QApplication::translate("MainWindow", "\342\210\236", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

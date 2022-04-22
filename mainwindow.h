#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>

#include <QtCharts>
#include <QLineSeries>
#include <QtMath>
#include <QValueAxis>
QT_CHARTS_USE_NAMESPACE

#include "callout.h"

#include "ajust.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSerialPort *serialPort;
    QTimer timer;
    QChart *chart;
    QLineSeries *serials_pitch;
    QLineSeries *serials_yaw;
    QLineSeries *serials_rol;
    double imu_pitch = 0,imu_yaw = 0,imu_rol = 0;


    QVector<QPointF> oldData_pitch,oldData_yaw,oldData_rol;
    QVector<QPointF> data_pitch,data_yaw,data_rol;
    Ajust *ajust;

private slots:
    void on_openBt_clicked();

    void on_closeBt_clicked();

    void on_clearBt_clicked();

    void on_sendBt_clicked();

    void serialPort_readyRead();

    void on_send16Checkbox_stateChanged(int arg1);

    void on_receive16Checkbox_stateChanged(int arg1);

    void tipSlot(QPointF position, bool isHovering);

    void on_searchPortBt_clicked();

    void adjust_received();

signals:
    void sendPid();

private:
    Ui::MainWindow *ui;
    void wheelEvent(QWheelEvent *event);
    void initChart();
    void initPort();
    void initSlot();
    Callout *tip;
    void updateData();
};

#endif // MAINWINDOW_H

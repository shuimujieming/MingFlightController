#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ajust = new Ajust;


    ajust->show();
    ajust->move(1080,ajust->y());

    //鼠标提示
    tip = nullptr;

    initChart();

    initPort();

    initSlot();

}
typedef struct
{
    int16_t P;
    int16_t I;
    int16_t D;
}Send_Pid;
//发送数据至飞控进行调参
void MainWindow::adjust_received()
{
    Send_Pid pitch_angle,yaw_angle,rol_angle;
    Send_Pid pitch_speed,yaw_speed,rol_speed;

    rol_angle.P = (int16_t)(pid_rol_angle.P * 10);
    rol_angle.I =(int16_t)(pid_rol_angle.I * 10);
    rol_angle.D = (int16_t)(pid_rol_angle.D * 10);

    pitch_angle.P = (int16_t)(pid_pitch_angle.P * 10);
    pitch_angle.I =(int16_t)(pid_pitch_angle.I * 10);
    pitch_angle.D = (int16_t)(pid_pitch_angle.D * 10);

    yaw_angle.P = (int16_t)(pid_yaw_angle.P * 10);
    yaw_angle.I =(int16_t)(pid_yaw_angle.I * 10);
    yaw_angle.D = (int16_t)(pid_yaw_angle.D * 10);

    rol_speed.P = (int16_t)(pid_rol_speed.P * 10);
    rol_speed.I = (int16_t)(pid_rol_speed.I * 10);
    rol_speed.D = (int16_t)(pid_rol_speed.D * 10);

    pitch_speed.P = (int16_t)(pid_pitch_speed.P * 10);
    pitch_speed.I = (int16_t)(pid_pitch_speed.I * 10);
    pitch_speed.D = (int16_t)(pid_pitch_speed.D * 10);

    yaw_speed.P = (int16_t)(pid_yaw_speed.P * 10);
    yaw_speed.I = (int16_t)(pid_yaw_speed.I * 10);
    yaw_speed.D = (int16_t)(pid_yaw_speed.D * 10);

    uint8_t User_send_buf[38];

    memcpy(&User_send_buf[1],&pitch_angle.P,6);
    memcpy(&User_send_buf[7],&yaw_angle.P,6);
    memcpy(&User_send_buf[13],&rol_angle.P,6);

    memcpy(&User_send_buf[19],&pitch_speed.P,6);
    memcpy(&User_send_buf[25],&yaw_speed.P,6);
    memcpy(&User_send_buf[31],&rol_speed.P,6);

    User_send_buf[0] = 0xff;

    User_send_buf[37] = 0xfe;

    serialPort->write((char*)User_send_buf,38);

    qDebug("adjust ok");
}

MainWindow::~MainWindow()
{
    delete ui;
}
//初始化槽函数关联
void MainWindow::initSlot()
{
    //关联串口准备接收数据
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(serialPort_readyRead()));
    //关联定时发送
    connect(&timer,SIGNAL(timeout()),this,SLOT(on_sendBt_clicked()));

    //鼠标提示
    connect(serials_pitch, SIGNAL(hovered(QPointF, bool)), this, SLOT(tipSlot(QPointF,bool)));

    //关联主窗口发送数据至调参串口
    connect(this,SIGNAL(sendPid()),ajust,SLOT(receiveData()));
    //关联调参窗口发送命令进行串口数据发送
    connect(ajust,SIGNAL(adjust_Pid()),this,SLOT(adjust_received()));
}
//初始化串口号
void MainWindow::initPort()
{

    //串口名列表
    QStringList serialPortName;

    serialPort = new QSerialPort(this);
    //清空串口列表
    ui->serialPortCb->clear();

    //添加串口号
    foreach(const QSerialPortInfo &info ,QSerialPortInfo::availablePorts())
    {
        serialPortName<<info.portName();
    }
    //添加串口号至列表
    ui->serialPortCb->addItems(serialPortName);

}
//初始化图表
void MainWindow::initChart()
{

    //初始化图表
    chart = new QChart();
    chart->createDefaultAxes();
    chart->setTitle("波形显示");
    //绑定图表至UI界面
    ui->scopeView->setChart(chart);

    //坐标系列初始化
    serials_yaw = new QLineSeries;
    serials_yaw->setName("Yaw");
    chart->addSeries(serials_yaw);

    serials_pitch = new QLineSeries;
    serials_pitch->setName("Pitch");
    chart->addSeries(serials_pitch);

    serials_rol = new QLineSeries;
    serials_rol->setName("Roll");
    chart->addSeries(serials_rol);

    serials_pitch->setUseOpenGL(true);
    serials_yaw->setUseOpenGL(true);
    serials_rol->setUseOpenGL(true);

    //设置波形颜色
    serials_yaw->setColor(QColor(0,0,255));
    serials_pitch->setColor(QColor(0,255,0));
    serials_rol->setColor(QColor(255,0,0));

    //图表初始化
    chart->createDefaultAxes();
    chart->axisY()->setRange(-180, 180);
    chart->axisX()->setRange(0, 200);

    chart->axisX()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisY()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisX()->setTitleText("div");
    chart->axisY()->setTitleText("角度/°");

    //隐藏背景轴线
    chart->axisX()->setGridLineVisible(false);
    chart->axisY()->setGridLineVisible(false);

    //顶部对齐
    chart->legend()->setAlignment(Qt::AlignTop);

    //防锯齿
    ui->scopeView->setRenderHint(QPainter::Antialiasing);
}

//鼠标滚轮
void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        chart->zoomIn();
    } else {
        chart->zoomOut();
    }
}

//打开串口
void MainWindow::on_openBt_clicked()
{
    QSerialPort::BaudRate baudRates;
    QSerialPort::StopBits stopBits;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity checkBits;

    switch (ui->baudrateCb->currentText().toInt()) {
     case 4800:
        baudRates = QSerialPort::BaudRate::Baud4800;
        break;
    case 9600:
       baudRates = QSerialPort::BaudRate::Baud9600;
       break;
    case 115200:
       baudRates = QSerialPort::BaudRate::Baud115200;
       break;
    default:
       baudRates = QSerialPort::BaudRate::Baud115200;
       break;
    }
    switch (ui->stopCb->currentIndex()) {
     case 0:
        stopBits = QSerialPort::StopBits::OneStop;
        break;
    case 1:
       stopBits = QSerialPort::StopBits::OneAndHalfStop;
       break;
    case 2:
       stopBits = QSerialPort::StopBits::TwoStop;
       break;
    default:
       stopBits = QSerialPort::StopBits::OneStop;
       break;
    }
    switch (ui->dataCb->currentText().toInt()) {
     case 8:
        dataBits = QSerialPort::DataBits::Data8;
        break;
    case 7:
       dataBits = QSerialPort::DataBits::Data7;
       break;
    case 6:
       dataBits = QSerialPort::DataBits::Data6;
       break;
    case 5:
       dataBits = QSerialPort::DataBits::Data5;
       break;
    default:
        dataBits = QSerialPort::DataBits::Data8;
        break;
    }
    switch (ui->parityCb->currentIndex()) {
     case 0:
        checkBits = QSerialPort::Parity::NoParity;
        break;
    case 1:
       checkBits = QSerialPort::Parity::OddParity;
       break;
    case 2:
       checkBits = QSerialPort::Parity::EvenParity;
       break;
    default:
        checkBits = QSerialPort::Parity::NoParity;
        break;
    }

    serialPort->setPortName(ui->serialPortCb->currentText());
    serialPort->setBaudRate(baudRates);
    serialPort->setDataBits(dataBits);
    serialPort->setStopBits(stopBits);
    serialPort->setParity(checkBits);

    if(serialPort->open(QIODevice::ReadWrite)==true)
    {
        QMessageBox::information(this,"提示","成功");
    }
    else {
        QMessageBox::information(this,"提示","失败");
    }
}
//关闭串口
void MainWindow::on_closeBt_clicked()
{
    serialPort->close();
}
//搜索串口
void MainWindow::on_searchPortBt_clicked()
{
    initPort();
}
//清除接收数据
void MainWindow::on_clearBt_clicked()
{
    ui->receivePlainTxt->clear();
}
//发送数据
void MainWindow::on_sendBt_clicked()
{
    qint64 length = 0;
    //字符串形式
    if(ui->send16Checkbox->checkState() == false)
    {
       length = serialPort->write(ui->sendTxt->text().toLocal8Bit().data());
    }
    //16进制
    else
    {
       length = serialPort->write(QByteArray::fromHex(ui->sendTxt->text().toUtf8()));
    }
}
//init16数据处理
int16_t bit8TObit16(uint8_t *change_info)
{
    union
    {
        int16_t f;
        char  byte[2];
    }u16val;
    u16val.byte[0]=change_info[0];
    u16val.byte[1]=change_info[1];
    return u16val.f;
}

char pid_rec_flag = 0;
//接收数据
void MainWindow::serialPort_readyRead()
{
    QString buf;
    QByteArray recbuf;
    int bufNum;

    //接收数据读取
    recbuf = serialPort->readAll();

    uint8_t * pdata = (unsigned char *) recbuf.data();

    //数据首尾校验
    int i=0;
    while(static_cast<uint8_t>(recbuf[i])  == 0xff)
    {

        if(pdata[i+43] == NULL)
        {break;}

        if(static_cast<uint8_t>(recbuf[i+43]) == 0xfe)
        {

            imu_pitch =static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+1]) | static_cast<uint8_t>(recbuf[i+2]) << 8) / 100.0;
            imu_yaw =static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+3]) | static_cast<uint8_t>(recbuf[i+4]) << 8) / 100.0;
            imu_rol =static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+5]) | static_cast<uint8_t>(recbuf[i+6]) << 8) / 100.0;
            

            pid_pitch_angle.P = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+7]) | static_cast<uint8_t>(recbuf[i+8]) << 8) / 10.0;
            pid_pitch_angle.I = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+9]) | static_cast<uint8_t>(recbuf[i+10]) << 8) / 10.0;
            pid_pitch_angle.D = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+11]) | static_cast<uint8_t>(recbuf[i+12]) << 8) / 10.0;

            pid_yaw_angle.P = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+13]) | static_cast<uint8_t>(recbuf[i+14]) << 8) / 10.0;
            pid_yaw_angle.I = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+15]) | static_cast<uint8_t>(recbuf[i+16]) << 8) / 10.0;
            pid_yaw_angle.D = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+17]) | static_cast<uint8_t>(recbuf[i+18]) << 8) / 10.0;

            pid_rol_angle.P = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+19]) | static_cast<uint8_t>(recbuf[i+20]) << 8) / 10.0;
            pid_rol_angle.I = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+21]) | static_cast<uint8_t>(recbuf[i+22]) << 8) / 10.0;
            pid_rol_angle.D = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+23]) | static_cast<uint8_t>(recbuf[i+24]) << 8) / 10.0;

            pid_pitch_speed.P = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+25]) | static_cast<uint8_t>(recbuf[i+26]) << 8) / 10.0;
            pid_pitch_speed.I = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+27]) | static_cast<uint8_t>(recbuf[i+28]) << 8) / 10.0;
            pid_pitch_speed.D = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+29]) | static_cast<uint8_t>(recbuf[i+30]) << 8) / 10.0;

            pid_yaw_speed.P = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+31]) | static_cast<uint8_t>(recbuf[i+32]) << 8) / 10.0;
            pid_yaw_speed.I = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+33]) | static_cast<uint8_t>(recbuf[i+34]) << 8) / 10.0;
            pid_yaw_speed.D = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+35]) | static_cast<uint8_t>(recbuf[i+36]) << 8) / 10.0;

            pid_rol_speed.P = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+37]) | static_cast<uint8_t>(recbuf[i+38]) << 8) / 10.0;
            pid_rol_speed.I = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+39]) | static_cast<uint8_t>(recbuf[i+40]) << 8) / 10.0;
            pid_rol_speed.D = static_cast<int16_t>( static_cast<uint8_t>(recbuf[i+41]) | static_cast<uint8_t>(recbuf[i+42]) << 8) / 10.0;

            //发送信号
            if(pid_rec_flag == 0)
            {emit sendPid();}
            pid_rec_flag = 1;

//            qDebug()<<"p is "<<pid_pitch_speed.P <<" " <<pid_pitch_speed.I <<" "<<pid_pitch_speed.D;

            serials_pitch->setName("Pitch\r\n" + QString::number(imu_pitch));
            serials_yaw->setName("Yaw\r\n" + QString::number(imu_yaw));
            serials_rol->setName("Roll\r\n" + QString::number(imu_rol));

            data_pitch.clear();
            oldData_pitch = serials_pitch->pointsVector();
            if(oldData_pitch.size() < 200)
            {
                data_pitch = serials_pitch->pointsVector();
            }
            else
            {
              for (int j = 1;j<oldData_pitch.size();j++)
              {
                data_pitch.append(QPoint(j-1,oldData_pitch.at(j).y()));
              }
            }

            data_pitch.append(QPointF(data_pitch.size(),imu_pitch));
            serials_pitch->replace(data_pitch);


            data_yaw.clear();
            oldData_yaw = serials_yaw->pointsVector();
            if(oldData_yaw.size() < 200)
            {
                data_yaw = serials_yaw->pointsVector();
            }
            else
            {
              for (int j = 1;j<oldData_yaw.size();j++)
              {
                data_yaw.append(QPoint(j-1,oldData_yaw.at(j).y()));
              }
            }

            data_yaw.append(QPointF(data_yaw.size(),imu_yaw));
            serials_yaw->replace(data_yaw);

            data_rol.clear();
            oldData_rol = serials_rol->pointsVector();
            if(oldData_rol.size() < 200)
            {
                data_rol = serials_rol->pointsVector();
            }
            else
            {
              for (int j = 1;j<oldData_rol.size();j++)
              {
                data_rol.append(QPoint(j-1,oldData_rol.at(j).y()));
              }
            }

            data_rol.append(QPointF(data_rol.size(),imu_rol));
            serials_rol->replace(data_rol);

        }
        //下一帧数据检查
        i+=44;
    }


         //字符串接收
        if(ui->receiveCheckbox->checkState() == false)
        {

           //数组转换字符
            buf = QString::fromLocal8Bit(recbuf);
        }
        //16进制
        else
        {
            recbuf=recbuf.toHex().toUpper();  //转16进制大写
            //转换成16进制
            buf = QString(recbuf);
            bufNum = buf.length();

            while(bufNum - 2 > 0)
            {
                bufNum = bufNum - 2;
                buf.insert(bufNum," ");
            }
        }
        //添加接收数据至接收框中
        ui->receivePlainTxt->appendPlainText(buf);

}
//切换16进制发送
void MainWindow::on_send16Checkbox_stateChanged(int arg1)
{
    QString str;
    int strNum;

    if(arg1 == false)
    {
        ui->sendTxt->setText(QByteArray::fromHex(ui->sendTxt->text().toUtf8().data()));
    }
    else {
        str = ui->sendTxt->text().toUtf8().toHex().data();

        strNum = str.length();
        while (strNum - 2 > 0) {
            strNum = strNum - 2;
            str.insert(strNum," ");
        }
        ui->sendTxt->setText(str);
    }
}

//定时发送触发
void MainWindow::on_receive16Checkbox_stateChanged(int arg1)
{
    //计时
    int counter;

    if(arg1 == false)
    {
        timer.stop();
    }
    //开启定时
    else {
        counter = ui->timeTxt->text().toInt();
        //是否开启定时
        if(counter > 0 && ui->sendTxt->text() != "")
        {
            timer.start(counter);
        }
        else {
            timer.stop();
        }
    }
}
//波形提示
void MainWindow::tipSlot(QPointF position, bool isHovering)
{
    if (tip == nullptr)
        tip = new Callout(chart);

    if (isHovering) {
        tip->setText(QString("X: %1 \nY: %2 ").arg(position.x()).arg(position.y()));
        tip->setAnchor(position);
        tip->setZValue(11);
        tip->updateGeometry();
        tip->show();
    } else {
        tip->hide();
    }
}


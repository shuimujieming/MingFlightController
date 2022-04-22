#include "ajust.h"
#include "ui_ajust.h"

Rec_Pid pid_pitch_angle,pid_yaw_angle,pid_rol_angle;
Rec_Pid pid_pitch_speed,pid_yaw_speed,pid_rol_speed;

Ajust::Ajust(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Ajust)
{
    ui->setupUi(this);

}

Ajust::~Ajust()
{
    delete ui;
}
void Ajust::receiveData()
{

    ui->pitch_angle_p->setText(QString::number(pid_pitch_angle.P));
    ui->pitch_angle_i->setText(QString::number(pid_pitch_angle.I));
    ui->pitch_angle_d->setText(QString::number(pid_pitch_angle.D));

    ui->pitch_speed_p->setText(QString::number(pid_pitch_speed.P));
    ui->pitch_speed_i->setText(QString::number(pid_pitch_speed.I));
    ui->pitch_speed_d->setText(QString::number(pid_pitch_speed.D));

    ui->yaw_angle_p->setText(QString::number(pid_yaw_angle.P));
    ui->yaw_angle_i->setText(QString::number(pid_yaw_angle.I));
    ui->yaw_angle_d->setText(QString::number(pid_yaw_angle.D));

    ui->yaw_speed_p->setText(QString::number(pid_yaw_speed.P));
    ui->yaw_speed_i->setText(QString::number(pid_yaw_speed.I));
    ui->yaw_speed_d->setText(QString::number(pid_yaw_speed.D));

    ui->rol_angle_p->setText(QString::number(pid_rol_angle.P));
    ui->rol_angle_i->setText(QString::number(pid_rol_angle.I));
    ui->rol_angle_d->setText(QString::number(pid_rol_angle.D));

    ui->rol_speed_p->setText(QString::number(pid_rol_speed.P));
    ui->rol_speed_i->setText(QString::number(pid_rol_speed.I));
    ui->rol_speed_d->setText(QString::number(pid_pitch_speed.D));

    qDebug("rec ok");
}

void Ajust::on_pushButton_clicked()
{
    pid_pitch_angle.P = ui->pitch_angle_p->text().toUtf8().toDouble();
    pid_pitch_angle.I = ui->pitch_angle_i->text().toUtf8().toDouble();
    pid_pitch_angle.D = ui->pitch_angle_d->text().toUtf8().toDouble();
    pid_pitch_speed.P = ui->pitch_speed_p->text().toUtf8().toDouble();
    pid_pitch_speed.I = ui->pitch_speed_i->text().toUtf8().toDouble();
    pid_pitch_speed.D = ui->pitch_speed_d->text().toUtf8().toDouble();


    pid_yaw_angle.P = ui->yaw_angle_p->text().toUtf8().toDouble();
    pid_yaw_angle.I = ui->yaw_angle_i->text().toUtf8().toDouble();
    pid_yaw_angle.D = ui->yaw_angle_d->text().toUtf8().toDouble();
    pid_yaw_speed.P = ui->yaw_speed_p->text().toUtf8().toDouble();
    pid_yaw_speed.I = ui->yaw_speed_i->text().toUtf8().toDouble();
    pid_yaw_speed.D = ui->yaw_speed_d->text().toUtf8().toDouble();


    pid_rol_angle.P = ui->rol_angle_p->text().toUtf8().toDouble();
    pid_rol_angle.I = ui->rol_angle_i->text().toUtf8().toDouble();
    pid_rol_angle.D = ui->rol_angle_d->text().toUtf8().toDouble();
    pid_rol_speed.P = ui->rol_speed_p->text().toUtf8().toDouble();
    pid_rol_speed.I = ui->rol_speed_i->text().toUtf8().toDouble();
    pid_rol_speed.D = ui->rol_speed_d->text().toUtf8().toDouble();
    //发送信号调整参数
    emit adjust_Pid();
}

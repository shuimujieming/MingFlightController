#ifndef AJUST_H
#define AJUST_H

#include <QDialog>

typedef struct
{
    double P;
    double I;
    double D;
}Rec_Pid;

extern Rec_Pid pid_pitch_angle,pid_yaw_angle,pid_rol_angle;
extern Rec_Pid pid_pitch_speed,pid_yaw_speed,pid_rol_speed;
namespace Ui {
class Ajust;
}

class Ajust : public QDialog
{
    Q_OBJECT

public:
    explicit Ajust(QWidget *parent = nullptr);
    ~Ajust();
private slots:
    void receiveData();
    void on_pushButton_clicked();
signals:
    void adjust_Pid();
private:
    Ui::Ajust *ui;
};

#endif // AJUST_H

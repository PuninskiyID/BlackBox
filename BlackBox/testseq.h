#ifndef TESTSEQ_H
#define TESTSEQ_H

#include "../common/common.h"
#include <QWidget>
#include <QFileDialog>
#include <QTimer>
#include <QComboBox>

namespace Ui {
    class TestSeq;
}
enum {
    _STOPPED,
    _STARTED,
    _ERROR
};

class TestSeq : public QWidget
{
    Q_OBJECT

    t_emc_control motor_a_spd;
    t_emc_control motor_b_trq;
    int current_row;
    int current_state;
    QTimer *timer;

    void SetPoint(int row);
    void M_Control(QComboBox *combo,t_emc_control *m);

public:
    explicit TestSeq(QWidget *parent = 0);
    ~TestSeq();

    void Addmotor(t_Motor *m);

private slots:

    void update();
    void on_comboMotorASpd_currentIndexChanged(int index);
    void on_comboMotorBTrq_currentIndexChanged(int index);

    void on_pushLoad_clicked();
    void on_pushStart_clicked();
    void on_pushStop_clicked();

private:



    Ui::TestSeq *ui;

signals:
    void StartLog();
    void SavePoint(int);
    void StopLog();

};

#endif // TESTSEQ_H

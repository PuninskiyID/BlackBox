#include "testseq.h"
#include "ui_testseq.h"

TestSeq::TestSeq(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestSeq)
{


    current_state=_STOPPED;

    motor_a_spd.unblock=0;
    motor_b_trq.unblock=0;
    motor_a_spd.start_block_cause=0;
    motor_b_trq.start_block_cause=0;
    motor_a_spd.test_mode=0;
    motor_b_trq.test_mode=0;

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->setSingleShot(true);

    ui->setupUi(this);
    setWindowTitle("Motor test sequence");
    ui->tableList->setColumnCount(3);

    static t_Motor mm;
    mm.id=-1;
    mm.ip.setAddress("0.0.0.0");
    Addmotor(&mm);
}

TestSeq::~TestSeq()
{
    delete ui;
}

void TestSeq::M_Control(QComboBox *combo,t_emc_control *m)
{
    if (combo->currentIndex()!=-1)
    {
        t_Motor mm;
        mm.id=combo->itemData(combo->currentIndex(),Qt::UserRole+0).toInt();
        mm.ip.setAddress(combo->itemData(combo->currentIndex(),Qt::UserRole+1).toInt());
        MotorControl(&mm,m);
    }

}
void TestSeq::SetPoint(int row)
{

    if (ui->tableList->rowCount()<=row)
    {
        current_state=_STOPPED;
        ui->l_state->setText("STOPPED");
        return;
    }

    ui->tableList->setCurrentCell(row,0);
    double speed=ui->tableList->item(row,0)->text().toDouble();
    double torque=ui->tableList->item(row,1)->text().toDouble();
    int time=ui->tableList->item(row,2)->text().toInt();

    motor_a_spd.unblock=0;
    motor_b_trq.unblock=0;

    motor_a_spd.target_speed=speed;
    motor_b_trq.target_torque=torque;

    M_Control(ui->comboMotorASpd,&motor_a_spd);
    M_Control(ui->comboMotorBTrq,&motor_b_trq);


    timer->start(time*1000);

}
void TestSeq::update()
{

    if (current_state!=_STARTED) return;
    current_row++;

    emit SavePoint(current_row);

    SetPoint(current_row);

}
void TestSeq::Addmotor(t_Motor *m)
{
    if (current_state!=_STOPPED) return;

        ui->comboMotorASpd->addItem(QString("%1 #%2").arg(m->ip.toString()).arg(m->id));
        ui->comboMotorASpd->setItemData(ui->comboMotorASpd->count()-1,m->id,Qt::UserRole+0);
        ui->comboMotorASpd->setItemData(ui->comboMotorASpd->count()-1,m->ip.toIPv4Address(),Qt::UserRole+1);

        ui->comboMotorBTrq->addItem(QString("%1 #%2").arg(m->ip.toString()).arg(m->id));
        ui->comboMotorBTrq->setItemData(ui->comboMotorBTrq->count()-1,m->id,Qt::UserRole+0);
        ui->comboMotorBTrq->setItemData(ui->comboMotorBTrq->count()-1,m->ip.toIPv4Address(),Qt::UserRole+1);
}

void TestSeq::on_pushLoad_clicked()
{

    if (current_state!=_STOPPED) return;

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("Open Seq"), QDir().current().absolutePath(), tr("Seq Files (*.csv)"));


    QFile f;
    QByteArray line;

    f.setFileName(fileName);
    f.open(QIODevice::ReadOnly);

    char buf[1024];

    ui->tableList->clear();


    qint64 lineLength = f.readLine(buf, sizeof(buf));
    int row=0;
    while (lineLength != -1)
    {

        ui->tableList->setRowCount(row+1);

        QString str(buf);
        QStringList lst = str.split(",");

        QTableWidgetItem *newItem0 = new QTableWidgetItem(tr("%1").arg(lst.at(0).toDouble()));
        QTableWidgetItem *newItem1 = new QTableWidgetItem(tr("%1").arg(lst.at(1).toDouble()));
        QTableWidgetItem *newItem2 = new QTableWidgetItem(tr("%1").arg(lst.at(2).toInt()));

        ui->tableList->setItem(row,0, newItem0);
        ui->tableList->setItem(row,1, newItem1);
        ui->tableList->setItem(row,2, newItem2);
        row++;

        lineLength = f.readLine(buf, sizeof(buf));
    }
}

void TestSeq::on_pushStart_clicked()
{

    if (current_state!=_STOPPED) return;

    current_state=_STARTED;

    ui->l_state->setText("RUN");

    current_row=0;

    emit StartLog();

    SetPoint(current_row);

}

void TestSeq::on_comboMotorASpd_currentIndexChanged(int)
{
    if (current_state!=_STOPPED) return;

    motor_a_spd.drive_en=1;
    motor_a_spd.mode=CAN_PF_EMC_CNTRL_MODE_SPEED_MOT_GEN;
    motor_a_spd.target_speed=0;
    M_Control(ui->comboMotorASpd,&motor_a_spd);
}

void TestSeq::on_comboMotorBTrq_currentIndexChanged(int)
{

    if (current_state!=_STOPPED) return;

    motor_b_trq.drive_en=1;
    motor_b_trq.mode=CAN_PF_EMC_CNTRL_MODE_TORQUE;
    motor_b_trq.target_torque=0;
    M_Control(ui->comboMotorBTrq,&motor_b_trq);
}

void TestSeq::on_pushStop_clicked()
{

    emit StopLog();

    current_state=_STOPPED;
    ui->l_state->setText("STOPPED");

    timer->stop();

    motor_b_trq.drive_en=0;
    motor_b_trq.unblock=0;
    motor_b_trq.mode=CAN_PF_EMC_CNTRL_MODE_TORQUE;
    motor_b_trq.test_mode=0;
    motor_b_trq.target_torque=0;
    motor_b_trq.target_speed=0;

    motor_a_spd.drive_en=0;
    motor_a_spd.unblock=0;
    motor_a_spd.mode=CAN_PF_EMC_CNTRL_MODE_TORQUE;
    motor_a_spd.test_mode=0;
    motor_a_spd.target_torque=0;
    motor_a_spd.target_speed=0;


    M_Control(ui->comboMotorASpd,&motor_a_spd);
    M_Control(ui->comboMotorBTrq,&motor_b_trq);

}

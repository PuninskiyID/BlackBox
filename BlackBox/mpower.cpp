#include "mpower.h"
#include "ui_mpower.h"

#include "testseq.h"
#include "ui_testseq.h"

#include <qwt_text.h>
#include "math.h"
#include "qwt_plot.h"

mPower::mPower(QWidget *parent) :  QWidget(parent),  ui(new Ui::mPower)
{
    ui->setupUi(this);




    isNorma4000Connected=false;
    x_pos=0;
    memset(&electric_m1,0,sizeof(t_Electric));
    memset(&electric_m2,0,sizeof(t_Electric));
    memset(&mechanic_m1_m2,0,sizeof(t_Mechanic));

    ts = new TestSeq();
    ts->hide();

    connect(ts, SIGNAL(SavePoint(int)),this, SLOT(SavePoint(int)));
    connect(ts, SIGNAL(StartLog()),this, SLOT(StartLog()));
    connect(ts, SIGNAL(StopLog()),this, SLOT(StopLog()));

    int p_limit[PLOT_COUNT][2]= {{-400,400},{0,500},{0,70000},{-220,220},{0,13000},{0,100}};

    char p_name[PLOT_COUNT][20] ={"Current (A)","Voltage (V)","Power (W)","Torque (N*m)","RPM (rpm)","Efficiency (%)"};
    char c_counts[PLOT_COUNT] ={2,2,3,1,1,2};

    ui->mg_label1->setText("M1 - Mot");
    ui->mg_label2->setText("M2 - Gen");

    ui->gridLayout->setContentsMargins(0,0,0,0);

    QFont arial("Arial", 8);

    for(int i=0;i<PLOT_COUNT;i++)
    {
        plots[i]= new t_plot2d;

        int kk=0;
        while(kk<c_counts[i])
        {
            plots[i]->curve[kk].setTitle(QString("%1").arg(0.0,10,'f',2,' '));
            plots[i]->curve[kk].attach(plots[i]);
            kk++;
            //c_counts[i]--;
        }

        plots[i]->setAxisScale(QwtPlot::yLeft,p_limit[i][0],p_limit[i][1],0);
        plots[i]->setAxisScale(QwtPlot::xBottom,0,X_MAX_POWER,0);

        plots[i]->zoomer = new QwtPlotZoomer(plots[i]->canvas());
        plots[i]->zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier);
        plots[i]->zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);
        plots[i]->zoomer->setTrackerMode(QwtPicker::AlwaysOn);
        plots[i]->zoomer->setRubberBandPen( QPen( Qt::red, 1, Qt::DotLine ) );
        plots[i]->zoomer->setTrackerPen( QPen( Qt::red ) );
        plots[i]->zoomer->setZoomBase(QRectF(0,p_limit[i][0],X_MAX_POWER,p_limit[i][1]));


        plots[i]->setAxisFont(QwtPlot::yLeft, arial);
        plots[i]->setAxisFont(QwtPlot::xBottom, arial);
        plots[i]->setCanvasBackground(QColor(Qt::black));
        plots[i]->setAxisTitle(QwtPlot::xBottom, QwtText(p_name[i]));



        plots[i]->grid = new QwtPlotGrid();
        //plots[i]->grid->enableXMin( true );
        plots[i]->grid->setMajorPen( QPen( Qt::darkGray, 0, Qt::DotLine ) );
        plots[i]->grid->setMinorPen( QPen( Qt::darkGray, 0 , Qt::DotLine ) );
        plots[i]->grid->attach(plots[i]);

        plots[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);

        ui->gridLayout->addWidget(plots[i],i/2,i%2);

    }

    tcpNorma4000 = new QTcpSocket(this);
    tcpNorma4000->connectToHost("7.7.7.250",23);
    connect(tcpNorma4000, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
    connect(tcpNorma4000, SIGNAL(connected()),this, SLOT(Connected()));
    connect(tcpNorma4000, SIGNAL(disconnected()),this, SLOT(Disconnected()));


    timer =new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(300);

    connect(timer, SIGNAL(timeout()),this, SLOT(Timeout()));

}
void mPower::Timeout()
{
    if (!isNorma4000Connected) return;

    tcpNorma4000->write("DATA?\n");
}
void mPower::Connected()
{
    isNorma4000Connected=true;
    tcpNorma4000->write("*IDN?\n");
    tcpNorma4000->write("FUNC \"VOLT1\",\"CURR1\",\"POW1:ACT\",\"VOLT2\",\"CURR2\",\"POW2:ACT\",\"SPEED1\",\"TORQ1\",\"POW1:MECH\",\"EFF1\",\"EFF2\" \n");
    tcpNorma4000->write("INIT:CONT ON\n");
    tcpNorma4000->write("SYNC:STATE OFF\n");
    tcpNorma4000->write("TRIGGER:START:SOURCE IMM\n");
    tcpNorma4000->write("APER 0.5\n");
    timer->start();
}
void mPower::Disconnected()
{
    timer->stop();
    isNorma4000Connected=false;
}
void mPower::SavePoint(int n)
{
    int e=0;
    qDebug()<<"SavePoint "<<n;
    if (points_log.isOpen())
    {
        if (points_log.write((char*)&electric_m1.current,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&electric_m2.current,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&electric_m1.voltage,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&electric_m2.voltage,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&electric_m1.power,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&electric_m2.power,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&mechanic_m1_m2.power,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&mechanic_m1_m2.torque,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&electric_m1.efficiency,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&electric_m2.efficiency,sizeof(double))!=sizeof(double)) e++;
        if (points_log.write((char*)&mechanic_m1_m2.rpm,sizeof(double))!=sizeof(double)) e++;

        if (e!=0)
        {
            qDebug()<<"Error writing points log file";
        }

    }
}
void mPower::StartLog()
{
    qDebug()<<"StartLog";

    if (points_log.isOpen()) points_log.close();
    points_log.setFileName(QDateTime::currentDateTime().toString("pp_log_yy.MM.dd_hh:mm:ss"));
    points_log.open(QIODevice::WriteOnly);
}
void mPower::StopLog()
{
    qDebug()<<"StopLog";
    points_log.close();
}
void mPower::readPendingDatagrams()
{
	static bool flag=true;
        char bdatagram[256];

        if (!isNorma4000Connected) return;

        tcpNorma4000->readLine(bdatagram,256);

        QString bs;
        bs.append(bdatagram);


        QStringList list;
        list = bs.split(",");

        if (!(bdatagram[0]=='+' || bdatagram[0]=='-'))
        {
            if (list.count()==4)
            {
                ui->label->setText(list.at(0) + "\n" + list.at(1) + "\n" + list.at(2) + "\n" +list.at(3));
            }
            return;
        }



        if (list.count()!=11)
        {
           qDebug()<<"Pmeter parse error";
           return;
        }

       electric_m1.voltage=list.at(0).toDouble();
       electric_m1.current=list.at(1).toDouble();
       electric_m1.power=fabs(list.at(2).toDouble());

       electric_m2.voltage=list.at(3).toDouble();
       electric_m2.current=list.at(4).toDouble();
       electric_m2.power=fabs(list.at(5).toDouble());

       mechanic_m1_m2.rpm=list.at(6).toDouble();
       mechanic_m1_m2.torque=list.at(7).toDouble();
       mechanic_m1_m2.power=fabs(list.at(8).toDouble());

       electric_m1.efficiency=fabs(list.at(9).toDouble());
       electric_m2.efficiency=fabs(list.at(10).toDouble());

       if (electric_m1.efficiency >100.0) electric_m1.efficiency = 100.0;
       if (electric_m1.efficiency <0.0) electric_m1.efficiency = 0.0;

       if (electric_m2.efficiency >100.0) electric_m2.efficiency = 100.0;
       if (electric_m2.efficiency <0.0) electric_m2.efficiency = 0.0;



      // p_power[0]=abs(e_current[0]*e_voltage[0]);
      // p_power[1]=abs(e_current[1]*e_voltage[1]);


       plots[0]->xydata[0].y[x_pos]= electric_m1.current;
       plots[0]->xydata[0].x[x_pos]= x_pos;
       plots[0]->curve[0].setTitle(QString("I1 %1").arg(electric_m1.current,10,'f',1,' '));
       plots[0]->xydata[1].y[x_pos]= electric_m2.current;
       plots[0]->xydata[1].x[x_pos]= x_pos;
       plots[0]->curve[1].setTitle(QString("I2 %1").arg(electric_m2.current,10,'f',1,' '));

       if (electric_m1.current > electric_m2.current)
       {
	   electric_m1.is_motor = true;
	   electric_m2.is_motor = false;
       }else
       {
	   electric_m1.is_motor = false;
	   electric_m2.is_motor = true;
       }


	if (electric_m1.is_motor!=flag)
	{	
		if (electric_m1.is_motor)
		{
			ui->mg_label1->setText("M1 - Mot");
			ui->mg_label2->setText("M2 - Gen");
			tcpNorma4000->write("SENS2:TYPE1 MOT\n");			
			tcpNorma4000->write("SENS2:TYPE2 GEN\n");
	   
		} else
		{
			ui->mg_label1->setText("M1 - Gen");
			ui->mg_label2->setText("M2 - Mot");
			tcpNorma4000->write("SENS2:TYPE2 MOT\n");   
                        tcpNorma4000->write("SENS2:TYPE1 GEN\n");
	   	}

		flag=electric_m1.is_motor; 
	}


       plots[1]->xydata[0].y[x_pos]= electric_m1.voltage;
       plots[1]->xydata[0].x[x_pos]= x_pos;
       plots[1]->curve[0].setTitle(QString("V1 %1").arg(electric_m1.voltage,10,'f',1,' '));
       plots[1]->xydata[1].y[x_pos]= electric_m2.voltage;
       plots[1]->xydata[1].x[x_pos]= x_pos;
       plots[1]->curve[1].setTitle(QString("V2 %1").arg(electric_m2.voltage,10,'f',1,' '));


       plots[2]->xydata[0].y[x_pos]= electric_m1.power;
       plots[2]->xydata[0].x[x_pos]= x_pos;
       plots[2]->curve[0].setTitle(QString("P1 %1").arg(electric_m1.power,10,'f',1,' '));

       plots[2]->xydata[1].y[x_pos]= electric_m2.power;
       plots[2]->xydata[1].x[x_pos]= x_pos;
       plots[2]->curve[1].setTitle(QString("P2 %1").arg(electric_m2.power,10,'f',1,' '));

       plots[2]->xydata[2].y[x_pos]= mechanic_m1_m2.power;
       plots[2]->xydata[2].x[x_pos]= x_pos;
       plots[2]->curve[2].setTitle(QString("Mech %1").arg(mechanic_m1_m2.power,10,'f',1,' '));


       plots[3]->xydata[0].y[x_pos]= mechanic_m1_m2.torque;
       plots[3]->xydata[0].x[x_pos]= x_pos;
       plots[3]->curve[0].setTitle(QString("Trq %1").arg(mechanic_m1_m2.torque,10,'f',1,' '));


       plots[4]->xydata[0].y[x_pos]= mechanic_m1_m2.rpm;
       plots[4]->xydata[0].x[x_pos]= x_pos;
       plots[4]->curve[0].setTitle(QString("RPM %1").arg(mechanic_m1_m2.rpm,12,'f',0,' '));


      // if ((p_power[0]!=0)&&(m_power!=0)) m1_eff =((m_power<p_power[0]) ? m_power/p_power[0] : p_power[0]/m_power) *100.0;
      //if ((p_power[1]!=0)&&(m_power!=0)) m2_eff =((m_power<p_power[1]) ? m_power/p_power[1] : p_power[1]/m_power) *100.0;



       plots[5]->xydata[0].y[x_pos]=electric_m1.efficiency;
       plots[5]->xydata[0].x[x_pos]= x_pos;
       plots[5]->curve[0].setTitle(QString("Eff1 %1").arg(electric_m1.efficiency,4,'f',1,' '));

       plots[5]->xydata[1].y[x_pos]=electric_m2.efficiency;
       plots[5]->xydata[1].x[x_pos]= x_pos;
       plots[5]->curve[1].setTitle(QString("Eff2 %1").arg(electric_m2.efficiency,4,'f',1,' '));


       //d_spectr->s_data->AddPoint(rpm,abs(m40_torque),m1_eff);
       //d_spectr->replot();


       for (int i=0;i<PLOT_COUNT;i++)
       {
           plots[i]->SetCursorPos(x_pos);
           if (ui->pushAutoScale->isChecked()) plots[i]->setAxisAutoScale(QwtPlot::yLeft,true);
           plots[i]->replot();
       }

       int e=0;

       if (log.isOpen())
       {
           if (log.write((char*)&electric_m1.current,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&electric_m2.current,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&electric_m1.voltage,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&electric_m2.voltage,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&electric_m1.power,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&electric_m2.power,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&mechanic_m1_m2.power,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&mechanic_m1_m2.torque,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&electric_m1.efficiency,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&electric_m2.efficiency,sizeof(double))!=sizeof(double)) e++;
           if (log.write((char*)&mechanic_m1_m2.rpm,sizeof(double))!=sizeof(double)) e++;


           if (e!=0)
           {
               qDebug()<<"Error writing log file";
           }else
           {
               log_size+=10*sizeof(double);
               ui->llog_size->setText(QString("Log Size: %1 Mb").arg(log_size/1024.0/1024.0,0,'f',4));
           }
       }
       x_pos++;
       if (x_pos>=X_MAX_POWER) x_pos=0;

       tcpNorma4000->write("*TRG\n");
}

mPower::~mPower()
{
    delete ui;
}
void mPower::Addmotor(t_Motor *m)
{

	ts->Addmotor(m);

}
void mPower::on_StartLog_clicked(bool checked)
{
    if (checked)
    {
            if (log.isOpen()) log.close();
            log.setFileName(QDateTime::currentDateTime().toString("p_log_yy.MM.dd_hh:mm:ss"));
            log.open(QIODevice::WriteOnly);
            ui->llog_name->setText(log.fileName());

            log_size=0;
    }
    else
    {
       log.close();
    }
}
/*
void mPower::on_pushButton_2_clicked(bool checked)
{
    if (checked)
    {
        plots[0]->hide();
        plots[1]->hide();
        d_spectr->show();

    }
    else
    {
        d_spectr->hide();
        plots[0]->show();
        plots[1]->show();
    }
}
*/
void mPower::on_pushTestSeq_clicked()
{
    ts->show();
}




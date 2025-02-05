#include "ike.h"
#include <QKeyEvent>
#include <QDebug>
#include <QPropertyAnimation>
#include <QPushButton>
#include <math.h>
#include <QApplication>
#include <QParallelAnimationGroup>
#include <QFont>

#define MAX_SPEED 140
#define LOOP_TIME 300
IKE::IKE()
{


    setFrameStyle(0);
    setGeometry(0,0,800,480);

    QDBusConnection bus = QDBusConnection::sessionBus();
    if(bus.registerObject("/", this))   qDebug("dbus register object ok");

    if( !bus.connect("", "", DbusInterface, "CarSpeed", this, SLOT(CarSpeed(int))) ) qDebug() <<" " << "dbus slot=CarSpeed connect error";
    if( !bus.connect("", "", DbusInterface, "CarFuel", this, SLOT(CarFuel(int))) ) qDebug() <<" " << "dbus slot=CarFuel connect error";
    if( !bus.connect("", "", DbusInterface, "CarEnergy", this, SLOT(CarEnergy(int))) ) qDebug() <<" " << "dbus slot=CarEnergy connect error";
    if( !bus.connect("", "", DbusInterface, "CarDriveMode", this, SLOT(CarDriveMode(int))) ) qDebug() <<" " << "dbus slot=CarDriveMode connect error";
    if( !bus.connect("", "", DbusInterface, "CarLampsInfo", this, SLOT(CarLampsInfo(int,int,int,int,int,int))) ) qDebug() <<" " << "dbus slot=CarLampsInfo connect error";
    if( !bus.connect("", "", DbusInterface, "CarMileage", this, SLOT(CarMileage(int))) ) qDebug() <<" " << "dbus slot=CarMileage connect error";
    if( !bus.connect("", "", DbusInterface, "IkeMSG", this, SLOT(IkeMSG(QString,int))) ) qDebug() <<" " << "dbus slot=IkeMSG connect error";

    bg = new QImage("./img/bg.png");

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-400, -240, 800, 480);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::NonCosmeticDefaultPen);
    setRenderHint(QPainter::SmoothPixmapTransform);


    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);

    WSpeedArrow=new ImageWidget();
    WSpeedArrow->AddImage(0,"img/arrow.png");
    scene->addItem(WSpeedArrow);
    WSpeedArrow->setPos(0,95);



    WTurnLeft=new ImageWidget();
    WTurnLeft->AddImage(0,"img/a_l.png");
    WTurnLeft->AddImage(1,"img/a_la.png");
    scene->addItem(WTurnLeft);
    WTurnLeft->setPos(-210,-185);

    WTurnRight=new ImageWidget();
    WTurnRight->AddImage(0,"img/a_r.png");
    WTurnRight->AddImage(1,"img/a_ra.png");
    scene->addItem(WTurnRight);
    WTurnRight->setPos(210,-185);


    WDriveMode=new ImageWidget();
    WDriveMode->AddImage(2,"img/parking.png");
    WDriveMode->AddImage(-1,"img/backward.png");
    WDriveMode->AddImage(1,"img/forward.png");
    WDriveMode->AddImage(0,"img/neutral.png");
    scene->addItem(WDriveMode);
    WDriveMode->setPos(0,60);



    WDriveModeSmall=new ImageWidget();
    WDriveModeSmall->AddImage(2,"img/parking_s.png");
    WDriveModeSmall->AddImage(-1,"img/backward_s.png");
    WDriveModeSmall->AddImage(1,"img/forward_s.png");
    WDriveModeSmall->AddImage(0,"img/neutral_s.png");
    scene->addItem(WDriveModeSmall);
    WDriveModeSmall->setPos(260,160);



    HI=new ImageWidget();
    HI->AddImage(0,"img/hi.png");
    HI->AddImage(1,"img/hi_a.png");
    scene->addItem(HI);
    HI->setPos(-360,160);

    PTF=new ImageWidget();
    PTF->AddImage(0,"img/ptf.png");
    PTF->AddImage(1,"img/ptf_a.png");
    scene->addItem(PTF);
    PTF->setPos(360,160);


    WLEnergy = new ImageWidget();
    WLEnergy->AddImage(0,"img/lev/l0.png");
    WLEnergy->AddImage(1,"img/lev/l1.png");
    WLEnergy->AddImage(2,"img/lev/l2.png");
    WLEnergy->AddImage(3,"img/lev/l3.png");
    WLEnergy->AddImage(4,"img/lev/l4.png");
    WLEnergy->AddImage(5,"img/lev/l5.png");
    WLEnergy->AddImage(6,"img/lev/l6.png");
    WLEnergy->AddImage(7,"img/lev/l7.png");
    WLEnergy->AddImage(8,"img/lev/l8.png");
    WLEnergy->AddImage(9,"img/lev/l9.png");
    WLEnergy->AddImage(10,"img/lev/l10.png");
    WLEnergy->AddImage(11,"img/lev/l11.png");
    WLEnergy->AddImage(12,"img/lev/l12.png");
    WLEnergy->AddImage(13,"img/lev/l13.png");
    WLEnergy->AddImage(14,"img/lev/l14.png");
    scene->addItem(WLEnergy);
    WLEnergy->setPos(-330,-15);


    WLFuel = new ImageWidget();
    WLFuel->AddImage(0,"img/lev/r0.png");
    WLFuel->AddImage(1,"img/lev/r1.png");
    WLFuel->AddImage(2,"img/lev/r2.png");
    WLFuel->AddImage(3,"img/lev/r3.png");
    WLFuel->AddImage(4,"img/lev/r4.png");
    WLFuel->AddImage(5,"img/lev/r5.png");
    WLFuel->AddImage(6,"img/lev/r6.png");
    WLFuel->AddImage(7,"img/lev/r7.png");
    WLFuel->AddImage(8,"img/lev/r8.png");
    WLFuel->AddImage(9,"img/lev/r9.png");
    WLFuel->AddImage(10,"img/lev/r10.png");
    WLFuel->AddImage(11,"img/lev/r11.png");
    WLFuel->AddImage(12,"img/lev/r12.png");
    WLFuel->AddImage(13,"img/lev/r13.png");
    WLFuel->AddImage(14,"img/lev/r14.png");
    scene->addItem(WLFuel);
    WLFuel->setPos(330,-15);




    WLEnergyText = new QGraphicsSimpleTextItem(QString().fromUtf8("0 В"));
    WLEnergyText->setBrush(QColor(255,234,52));
    WLEnergyText->setFont(QFont("Arial", 20, QFont::Bold));
    scene->addItem(WLEnergyText);
    WLEnergyText->setPos(-380,-170);


    WLFuelText = new QGraphicsSimpleTextItem(QString().fromUtf8("0 A"));
    WLFuelText->setBrush(QColor(62,67,255));
    WLFuelText->setFont(QFont("Arial", 20, QFont::Bold));
    scene->addItem(WLFuelText);
    WLFuelText->setPos(380-WLFuelText->boundingRect().width(),-170);


    WDigSpeed = new QGraphicsSimpleTextItem("0");
    WDigSpeed->setBrush(QColor(208,194,248));
    WDigSpeed->setFont(QFont("DS-Digital", 90));
    scene->addItem(WDigSpeed);
    WDigSpeed->setPos(-(WDigSpeed->boundingRect().width()/2),0);
    WDigSpeed->hide();

    WTime = new QGraphicsSimpleTextItem("00:00");
    WTime->setBrush(QColor(0,0,180));
    WTime->setFont(QFont("DS-Digital", 40));
    scene->addItem(WTime);
    WTime->setPos(-(WTime->boundingRect().width()/2),130);


    WDate = new QGraphicsSimpleTextItem("00/00/00");
    WDate->setBrush(QColor(208,194,248));
    WDate->setFont(QFont("DS-Digital", 20));
    scene->addItem(WDate);
    WDate->setPos(width()/2-WDate->boundingRect().width()-20,205);

    WDist = new QGraphicsSimpleTextItem(QString().fromUtf8("0.0 km"));
    WDist->setBrush(QColor(208,194,248));
    WDist->setFont(QFont("DS-Digital", 20));
    scene->addItem(WDist);
    WDist->setPos(-width()/2+20,205);

    WMsg = new QGraphicsSimpleTextItem("");
    WMsg->setBrush(QColor(208,194,248));
    WMsg->setFont(QFont("Arial", 24));
    scene->addItem(WMsg);
    WMsg->setPos(0,200);


    timer_ike_msg=  new QTimer();
    connect(timer_ike_msg,SIGNAL(timeout()),this,SLOT(timer_ike()));

    timer_05s = new QTimer();
    timer_05s->setInterval(500);
    connect(timer_05s,SIGNAL(timeout()),this,SLOT(timer_05_cb()));
    timer_05s->start();


    IkeMSG(QString().fromUtf8("Проверка систем Ок!"),5000);

    rotation=0;

    PrevSpeed=0;
    NewSpeed=0;

}
void IKE::IkeMSG(QString txt,int timeout)
{
    timer_ike_msg->stop();
    WMsg->setText(txt);

    WMsg->setPos(-WMsg->boundingRect().width()/2,200);

    timer_ike_msg->setInterval(timeout);
    timer_ike_msg->start();
}
IKE::~IKE()
{

}
void IKE::timer_05_cb()
{
    if (!isVisible()) return;

    WTime->setText(QTime().currentTime().toString("hh:mm"));
    WTime->setPos(-(WTime->boundingRect().width()/2),130);

    WDate->setText(QDate().currentDate().toString("dd/MM/yy"));
    WDate->setPos(width()/2-WDate->boundingRect().width()-20,205);

}
void IKE::timer_ike()
{
    WMsg->setText("");
}


void IKE::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();
    //painter->fillRect(sceneRect, Qt::black);
    painter->drawImage(-400,-240,*bg);
    painter->setRenderHint(QPainter::Antialiasing);

}
void  IKE::CarEnergy(int val)
{
    Energy(val);
}
void  IKE::CarFuel(int val)
{
    Fuel(val);
}
void  IKE::CarDriveMode(int val)
{
    //static int drive_mode=0;
    //if (drive_mode!=((val>>2)&0x03))
    {
       // drive_mode=(val>>2)&0x03;
        /*
            00 — P (паркинг)
            01 — R (задний ход)
            10 — D (вперёд)
            11 — N (нейтраль ??)
        */
        WDriveMode->SetState(val);
        WDriveModeSmall->SetState(val);
        switch (val)
        {
        case 2://P
            IkeMSG(QString().fromUtf8("Парковка"),3000);
            break;
        case -1:
            IkeMSG(QString().fromUtf8("Движение назад"),3000);
            break;
        case 1:
            IkeMSG(QString().fromUtf8("Движение вперед"),3000);
            break;
        case 0:
            IkeMSG(QString().fromUtf8("Нейтраль"),3000);
            break;
        }
    }

}
void  IKE::CarSpeed(int speed)
{
    static int last_spd=0;

    if (speed>140) speed=140;
    if (speed<0) speed=0;

    if (last_spd!=speed)
    {
       last_spd=speed;
       Speed(speed);
    }
}
void  IKE::CarMileage(int milage)
{
    //qDebug()<<"CarMileage"<<milage;
    WDist->setText(QString().fromUtf8("%1.%2 km").arg(milage/10).arg(milage%10));
}
void  IKE::CarLampsInfo(int d0,int d1,int d2,int d3,int d4,int d5)
{

    if (!isVisible()) return;

    //qDebug()<<"CarLampsInfo"<<d0<<d1<<d2<<d3<<d4<<d5;
    /*D0 — битовое поле (b7-b0)
    • b0-b1 — ближний свет (Л)
    • b2-b3 — ближний свет (П)
    • b4-b5 — дальний свет (Л)
    • b6-b7 — дальний свет (П)*/

    //if (((d0&(3<<0))==1)||((d0&(3<<2))==1)) TurnLamp(0,1);
    //else TurnLamp(0,0);


    /*D1 — битовое поле (b7-b0)
    • b0-b1 — задняя ПТФ (Л)
    • b2-b3 — задняя ПТФ (П)
    • b4-b5 — передняя ПТФ (Л)
    • b6-b7 — передняя ПТФ (П)*/


    /*D2 — битовое поле (b7-b0)
    • b0-b1 — поворот П (Л)
    • b2-b3 — поворот З (Л)
    • b4-b5 — поворот П (П)
    • b6-b7 — поворот З (П)
    */


    //Противотуманка
    if (((d1&(3<<4))==(1<<4))||((d1&(3<<6))==(1<<6))) PTF->SetState(1);
    else PTF->SetState(0);

    //Дальний свет
    if (((d0&(3<<4))==(1<<4))||((d0&(3<<6))==(1<<6))) HI->SetState(1);
    else HI->SetState(0);

    //Левый поворот
    if (((d2&0x3)==1)||((d2&0xC)==0x4)) WTurnLeft->SetState(1);
    else  WTurnLeft->SetState(0);

    //Правый поворот
    if (((d2&0x30)==0x10)||((d2&0xC0)==0x40)) WTurnRight->SetState(1);
    else WTurnRight->SetState(0);

    HI->update();
    PTF->update();
    WTurnRight->update();
    WTurnLeft->update();


}
void IKE::AniCurrentLoopChanged (QVariant)
{
    int txt=PrevSpeed+(((float)rotation->currentLoopTime()/(float)LOOP_TIME)*(NewSpeed-PrevSpeed));
    WDigSpeed->setText(QString().setNum(txt));
    WDigSpeed->setPos(-(WDigSpeed->boundingRect().width()/2),0);
}

void IKE::Speed(int spd)
{
    static int prev_speed=-1;
    int speed;float step,inc_step;
    int inc_val;

    if (!isVisible()) return;



    if (spd!=0)
    {
       WDriveMode->hide();
       WDigSpeed->show();
    }
    else
    {
        WDigSpeed->hide();
        WDriveMode->show();
    }

    if (rotation)
    {
        disconnect(rotation,SIGNAL(valueChanged(QVariant)),this,SLOT(AniCurrentLoopChanged(QVariant)));
        prev_speed=rotation->currentValue().toInt()*10;
        rotation->stop();
        delete rotation;

    }
    rotation = new QPropertyAnimation((QGraphicsWidget*)WSpeedArrow, "rotation",this);
    rotation->setDuration(LOOP_TIME);
    rotation->setEasingCurve(QEasingCurve::InOutSine);

    connect(rotation,SIGNAL(valueChanged(QVariant)),this,SLOT(AniCurrentLoopChanged(QVariant)));

    PrevSpeed=NewSpeed;
    NewSpeed=spd;

    spd-=3;
    speed=spd*1880/MAX_SPEED;

    speed>prev_speed ? inc_val=1 :inc_val=-1;

    step=0;
    inc_step=1.0/(abs(speed-prev_speed)+0.00000000001);

    if (prev_speed==speed)
    {
        inc_val=0;
        rotation->setKeyValueAt(0,prev_speed/10.0);
    }
    while (speed!=prev_speed)
    {
      prev_speed+=inc_val;

      step+=inc_step;
      if ((step>1.0)||(prev_speed+inc_val==speed)) step=1;
      rotation->setKeyValueAt(step,(float)prev_speed/10.0);
    }
    rotation->start();

}
void IKE::Fuel(int val)
{
    if (!isVisible()) return;
    WLFuel->SetState(val*13/0xFF);
    WLFuelText->setText(QString().setNum(val)+QString().fromUtf8(" A"));

}
void IKE::Energy(int val)
{
    if (!isVisible()) return;
    WLEnergy->SetState(val*13/400);
    WLEnergyText->setText(QString().setNum(val)+QString().fromUtf8(" В"));
}


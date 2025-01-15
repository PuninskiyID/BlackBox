#include "mtab.h"
#include <string.h>

mTab::mTab(QWidget *parent) :
    QWidget(parent)
{

      //  QFont arial("Arial", 7);
      //  arial.setBold(true);

        grid = new QGridLayout();
        grid->setMargin(0);
        grid->setHorizontalSpacing(0);
        grid->setVerticalSpacing(0);

        btn_enable = 0;

        /*btn_enable = new QPushButton("En",this);
        btn_enable->setCheckable(true);
        connect(btn_enable,SIGNAL(clicked(bool)),this,SLOT(on_Enable_clicked(bool)));
        grid->addWidget(btn_enable,1,0,1,4);
        */

        for (int i=0;i<PARAMS_IN_PACK;i++)
        {
            lab[i] = 0;
            val[i]= new QLabel("0");
            QFont arial("Arial", 7);
            arial.setBold(true);
            val[i]->setFont(arial);
            val[i]->setMinimumWidth(30);
            //val[i]->setMaximumHeight(30);
            val[i]->setAlignment(Qt::AlignRight|Qt::AlignHCenter);


            chk[i]= new QPushButton(QString("P %1").arg(i),this);
            chk[i]->setCheckable(true);
            //chk[i]->setMaximumHeight(20);
            chk[i]->setMaximumWidth(100);

           // chk[i]->setFont(arial);
            chk[i]->setStyleSheet(QString().sprintf("color : #%02x%02x%02x",col[i].red(),col[i].green(),col[i].blue()));
            connect(chk[i],SIGNAL(clicked(bool)),this,SLOT(chkStateChanged(bool)));

            grid->addWidget( chk[i],2+ i%16,0+2*(i/16));
            grid->addWidget( val[i],2+ i%16,1+2*(i/16));
//            grid->addWidget( chk[i],2+ i,0);
//            grid->addWidget( val[i],2+ i,1);
        }

        setLayout(grid);
}

void mTab::SetParName(int id,QString name)
{
    chk[id]->setText(name);
}

void mTab::on_Enable_clicked(bool /* val*/)
{
    //emit m_enabled(1,val);
}

void mTab::SetLab(int id, double v)
{
    val[id]->setText(QString("%1").arg(v,0,'f',2));

}

bool mTab::IsChecked(int id)
{
    return chk[id]->isChecked();
}

void mTab::chkStateChanged(bool)
{
    QPushButton *s=(QPushButton*)QObject::sender();

    for (int i=0;i<PARAMS_IN_PACK;i++)
    {
        if (chk[i]==s) emit p_selected(i,chk[i]->isChecked());
    }
}

mTab::~mTab()
{

    if(btn_enable)
    {
        delete btn_enable;
    }

    for (int i=0;i<PARAMS_IN_PACK;i++)
    {
        delete val[i];
        delete chk[i];
        if(lab[i])
        {
            delete lab[i];
        }
    }
}

struct ParamNameFileStruct
{
//    uint8_t id;
    char name[32];
};

#define START_MN 0xABCD1234EF567890ULL

void mTab::WriteNamesToFile(QFile& file)
{
    if(!file.isOpen())
    {
        return;
    }

    uint64_t mn;
    struct ParamNameFileStruct s;

    mn = START_MN;
    file.write((char*)&mn, sizeof(mn));

    for(unsigned int i = 0; i < PARAMS_IN_PACK; ++i)
    {
        strncpy(s.name, chk[i]->text().toStdString().c_str(), sizeof(s.name));
        file.write((char*)&s, sizeof(s));
    }

}

unsigned int mTab::ReadNamesFromFile(QFile& file)
{
    uint64_t mn;
    struct ParamNameFileStruct s;

    file.read((char*)&mn, sizeof(mn));
    if(mn != START_MN)
    {
        file.seek(0);
        return 0;
    }

    for(unsigned int i = 0; i < PARAMS_IN_PACK; ++i)
    {
        file.read((char*)&s, sizeof(s));
        chk[i]->setText(s.name);
        qDebug()<<s.name;
    }

    return PARAMS_IN_PACK * sizeof(s) + sizeof(mn);
}























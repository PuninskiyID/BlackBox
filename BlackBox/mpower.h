#ifndef MCONFIG_H
#define MCONFIG_H

#include <QWidget>
#include <QFile>
#include <QDateTime>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QMutex>
#include <QPen>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_legenditem.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>

#include "qwt_slider.h"
#include <qwt_plot_grid.h>

#include "../common/common.h"
//#include "plot.h"

#include "testseq.h"



#define PLOT_COUNT 6

#define P_UDP_POWER_P  60001
#define P_UDP_POWER_L  60000

class LegendItem: public QwtPlotLegendItem
{
public:
    LegendItem()
    {
         QFont arial("Arial", 12);
        setRenderHint( QwtPlotItem::RenderAntialiased );
        QColor color( Qt::white );
        setTextPen( color );
        setFont(arial);
#if 1
        setBorderPen( color );
        QColor c( Qt::gray );
        c.setAlpha( 200 );
        setBackgroundBrush( c );
#endif
    }
};

typedef class plot2d: public QwtPlot
{
    Q_OBJECT


    public:

    QColor        col[3];
    QwtPlotGrid   *grid;
    QwtPlotCurve  curve[3];
    t_xydata_pwr     xydata[3];
    QwtPlotMarker *d_marker0;
    QwtPlotZoomer *zoomer;


    void SetCursorPos(double pos)
    {
        d_marker0->setValue( pos, 0.0 );
    }

    plot2d(QWidget *parent = 0): QwtPlot(parent)
    {


        col[0]=Qt::blue;
        col[1]=Qt::red;
        col[2]=Qt::green;


        d_marker0 = new QwtPlotMarker();
        d_marker0->setValue( 100.0, 0.0 );
        d_marker0->setLineStyle( QwtPlotMarker::VLine );
        d_marker0->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
        d_marker0->setLinePen( QPen( Qt::green, 0, Qt::DashDotLine ) );
        d_marker0->attach( this);

        // this->setMaximumSize(QSize(1000,250));

        LegendItem *d_legendItem = new LegendItem();


        //d_legendItem->setAlignment( Qt::Alignment(Qt::AlignRight | Qt::AlignTop) );
        d_legendItem->setMaxColumns(1);

        d_legendItem->setBorderRadius( 4 );
        d_legendItem->setMargin( 4 );
        d_legendItem->setSpacing( 5 );
        d_legendItem->setItemMargin( 0 );

        QFont arial("Monospace", 9);
        arial.setBold(true);

        d_legendItem->setFont(arial);

        d_legendItem->attach( this );

        for(int j=0; j<3;j++)
        {
            for (int i=0; i<X_MAX_POWER;i++)
            {
                xydata[j].x[i]=i;
                xydata[j].y[i]=0;
            }

            curve[j].setRawSamples(xydata[j].x,xydata[j].y,X_MAX_POWER);
            curve[j].setPen(col[j]);
            curve[j].setStyle( QwtPlotCurve::Lines );
        }

        //setMaximumHeight(220);

    };


} t_plot2d;


namespace Ui {
    class mPower;
}


typedef class par_electric
{
public:
    double voltage;
    double current;
    double power;
    double efficiency;
    bool   is_motor;
    bool   is_gen;

} t_Electric;

typedef class par_mechanic
{
public:
    double torque;
    double rpm;
    double power;

} t_Mechanic;


class mPower : public QWidget
{
    Q_OBJECT

    Ui::mPower *ui;

    int x_pos;
    t_plot2d *plots[PLOT_COUNT];
    QTcpSocket *tcpNorma4000;
    QMutex mutex;

public:

    QFile log;
    QFile points_log;
    int log_size;
    double x_scaled0,x_scaled1;
    void Addmotor(t_Motor *m);

    //Plot *d_spectr;


    explicit mPower(QWidget *parent = 0);
    ~mPower();

private:

    QTimer *timer;
    TestSeq *ts;
    bool isNorma4000Connected;
    t_Mechanic mechanic_m1_m2;
    t_Electric electric_m1,electric_m2;


private slots:

    void StartLog();
    void SavePoint(int);
    void StopLog();

    void readPendingDatagrams();
    void Connected();
    void Disconnected();
    void Timeout();


    void on_StartLog_clicked(bool checked);
    void on_pushTestSeq_clicked();
};

#endif // MCONFIG_H

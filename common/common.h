#ifndef COMMON_H
#define COMMON_H

#include <QHostAddress>
#include <QUdpSocket>
#include <cmath>
#include <QVector>

#include "./dcu_emc.h"
#include "./emc_ethernet.h".h"
#include "qcolor.h"

#ifdef __APPLE__

#include "endian_mac.h"

#else

static uint32_t htobe32(uint32_t v)
{
    return
            (((v >> 24) & 0xff) << 0) |
            (((v >> 16) & 0xff) << 8) |
            (((v >> 8) & 0xff) << 16) |
            (((v >> 0) & 0xff) << 24);
}

static uint32_t be32toh(uint32_t v)
{
    return
            (((v >> 24) & 0xff) << 0) |
            (((v >> 16) & 0xff) << 8) |
            (((v >> 8) & 0xff) << 16) |
            (((v >> 0) & 0xff) << 24);
}

#endif

#define PARAMS_IN_PACK 32
#define PACKS_IN_PACKET 32
#define PACKET_SIZE 1024


#define X_SIZE 20*1024

#define X_MAX_POWER  300
typedef struct xydata
{
    double x[X_SIZE];
    double y[X_SIZE];

} t_xydata_live;

typedef struct one0
{
    double x[X_MAX_POWER];
    double y[X_MAX_POWER];

} t_xydata_pwr;

double SWAPDOUBLE(double src);

typedef class motor
{
public:
  int id;
  QHostAddress ip;
} t_Motor;

class Colors
{
    QColor col[36];
    unsigned colCnt = 0;
    double calcBrightness(double R, double G, double B)
    {
//        return 0.2126*R + 0.7152*G + 0.0722*B;
        return 0.4*R + 0.55*G + 0.05*B;
//        return 0.299*R + 0.587*G + 0.114*B;
//        return sqrt( 0.299*R*R + 0.587*G*G + 0.114*B*B) ;
    }
    void pushColor(double R, double G, double B)
    {
        auto brig = calcBrightness(R,G,B);
        qDebug("Color:%02d.%03d.%03d.%03d BRIG=%.3f", colCnt, (int)R, (int)G, (int)B, brig);
        while (brig < 50)
        {
            R += 8.0;
            G += 10.0;
            brig = calcBrightness(R,G,B);
            qDebug("  BRIG->%.3f", brig);
        }
        col[colCnt++].setRgb(R, G, B);
    }
public:
    Colors()
    {
        int color;
        for (int c=0;c<6;c++)
        {
            color=60+(5-c)*34;
            pushColor(color,0,0);
            pushColor(0,color,0);
            pushColor(0,0,color);
            pushColor(0,color,color);
            pushColor(color,0,color);
            pushColor(color,color,0);
        }
    }
    QColor& operator[](unsigned i)
    {
        return col[i % colCnt];
    }
};


void MotorControl(t_Motor *m ,t_emc_control *mc);

#endif // COMMON_H

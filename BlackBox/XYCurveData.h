#ifndef XYCURVEDATA_H
#define XYCURVEDATA_H
#include "common.h"
#include "qcombobox.h"
#include "qframe.h"
#include "qpushbutton.h"
#include "qwt_plot_advanced.h"
#include "series_data.h"
#include <QRandomGenerator>
#include <QwtPlotMarker>
#include <QLineEdit>
#include <QLabel>

#define buffSize 512


class XYCurve : public QObject
{
    Q_OBJECT

private:



    QColor color;
    QwtPlotCurveAdvanced *curve;
    QLabel *colorShowLabel;
    QComboBox *pX;
    QComboBox *pY;
    QLineEdit *step;
    QPushButton *deleteButton;
    QList<XYCurve*> *parentDataList;
    QList<QFrame*> *parentFrameList;
    QwtPlotAdvanced *plot;
    t_xydata_live *xydata;
    double x[buffSize];
    double y[buffSize];
    double xNorm[buffSize];
    double yNorm[buffSize];
    int lastPosCycled = 0;
    bool logState = 0;
    //QwtPlotMarker *lastPoint;

public slots:

    void XYParamPickWrapper(int id);
    void Delete();

public:
    XYCurve(QLabel *colorLabel, QComboBox *newPX, QComboBox *newPY, QPushButton *newDelButton, QLineEdit *newStep, QwtPlotAdvanced *newPlot, bool newLogState)
    {
        this->color = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 255, 255);
        this->colorShowLabel = colorLabel;
        this->colorShowLabel->setStyleSheet(QString("background-color: %1").arg(this->color.name()));
        this->curve = new QwtPlotCurveAdvanced;
        this->plot = newPlot;
        this->pX = newPX;
        this->pY = newPY;
        this->step = newStep;
        this->deleteButton = newDelButton;
        this->logState = newLogState;
        //this->lastPoint = new QwtPlotMarker;

        if (this->pX && this->pY && this->deleteButton)
        {
            QObject::connect(this->pX, SIGNAL(currentIndexChanged(int)), this, SLOT(XYParamPickWrapper(int)));
            QObject::connect(this->pY, SIGNAL(currentIndexChanged(int)), this, SLOT(XYParamPickWrapper(int)));
            QObject::connect(this->deleteButton, SIGNAL(clicked()), this, SLOT(Delete()));
        }
        else
            qDebug()<<"no some widget ----- XYCurve constructor failed";
    }

    ~XYCurve()
    {
        if(this->parentDataList)
        {
            this->parentFrameList->at(this->parentDataList->indexOf(this))->hide();
            if(this->parentFrameList)
            {
                delete this->parentFrameList->at(this->parentDataList->indexOf(this));
                this->parentFrameList->removeAt(this->parentDataList->indexOf(this));
                this->parentDataList->removeOne(this);
            }
        }
        else
            qDebug()<<"no list to delete";

        if (this->curve)
            {
                this->curve->detach();
                this->plot->replot();
                delete curve;
                curve = nullptr;
            }
    }

    void SetParentDataList(QList<XYCurve*> *list);
    void SetParentFrameList(QList<QFrame *> *list);
    void SetData(t_xydata_live* newData);

    void StepPickedData(int idx, int idy, int curX, int samplesToCheck);
    void RollData();
    QColor GetColor();
    int GetXId();
    int GetYId();

    double FindDistace(double x1, double y1, double x2, double y2);
    void AppendMarker(double x, double y);

    void XYParamPick(int curX, int samplesToCheck = 128);

};

class XYCurveData
{
private:
    QList <XYCurve*> data;
    QList <QFrame*> dataWindow;


public:
    void AddCurve(XYCurve *curve, QFrame *dataFW);
    void DeleteCurve(int id);
    void SetData(int id, double* x[X_SIZE], double* y[X_SIZE]);
    XYCurve GetCurve(int id);
    int GetXId(int id);
    int GetYId(int id);
    int Size();
    QColor GetColor(int id);
    QFrame* GetWindow(int id);
    XYCurve* at( int id );
};



#endif // XYCURVEDATA_H

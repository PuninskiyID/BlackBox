#ifndef PARAMVIEW_H
#define PARAMVIEW_H


#include "qwt_plot_advanced.h"
#include <QtCore>
#include <QtWidgets>

class ParamView
{
    QwtPlotAdvanced *plotPer;
    QwtPlotAdvanced *plotTc;

    QLineEdit *frameHeight;
    QLineEdit *frameWidth;

    QComboBox *sourceSelect;
    QComboBox *triggerSelect;

    QLineEdit *topTrigger;
    QLineEdit *bottomTrigger;

    QToolButton *lockTrigger;

    QRadioButton *tcCenter;
    QRadioButton *sampleUpdateCenter;

public:
    ParamView();


};

#endif // PARAMVIEW_H

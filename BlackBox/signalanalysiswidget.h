#ifndef SIGNALANALYSISWIDGET_H
#define SIGNALANALYSISWIDGET_H

#include "qcombobox.h"
#include <QWidget>

class SignalAnalysisWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalAnalysisWidget(QWidget *parent = nullptr);
    QComboBox* signalSelectBox;

signals:

};

#endif // SIGNALANALYSISWIDGET_H

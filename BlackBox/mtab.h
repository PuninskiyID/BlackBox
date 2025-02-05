#ifndef MTAB_H
#define MTAB_H

#include <QWidget>
#include <QFile>
#include <QLabel>
#include <QHash>

#include <QList>
#include <QString>
#include <QGroupBox>
#include <QCheckBox>
#include <QMainWindow>
#include <QPushButton>
#include <QUdpSocket>
#include <QGridLayout>
#include <QDir>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>
#include <QUdpSocket>

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include "../common/common.h"


class mTab : public QWidget
{
    Q_OBJECT
    Colors col;

    QLabel *lab[PARAMS_IN_PACK];
    QGridLayout *grid;
    QPushButton *btn_enable;

public:
    QLabel *val[PARAMS_IN_PACK];
    QPushButton *chk[PARAMS_IN_PACK];
    QLineEdit *signal_scale[PARAMS_IN_PACK];
    void SetLab(int id, double val);
    bool IsChecked(int id);

    void SetParName(int id,QString name);
    void WriteNamesToFile(QFile& file);
    unsigned int ReadNamesFromFile(QFile& file);

    explicit mTab();
    ~mTab();

signals:

    void p_selected(int,int);
    void EnableMotorDebug(int, bool);

public slots:
private slots:

     void on_Enable_clicked(bool );
     void chkStateChanged(bool);

};

#endif // MTAB_H

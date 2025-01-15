#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QLabel>
#include <QHash>
#include <QList>
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
#include <QVector>
#include <QPair>


#include <QTimer>
#include <qwt.h>

#include "mpower.h"
#include "mtab.h"
#include "tctab.h"

#include "../common/qwt_plot_advanced.h"
#include "XYCurveData.h"


typedef class MotorHndl
{
public:


  t_Motor motorAddress;
  QFile logFile;
  unsigned int logFileOffset;
  int logSize;
  t_xydata_live xydata[PARAMS_IN_PACK];
  ///////////////////////////state check
  bool highState[PARAMS_IN_PACK];
  QVector<QwtPlotMarker*>* triggerPointsHigh[PARAMS_IN_PACK];
  QVector<QwtPlotMarker*>* triggerPointsLow[PARAMS_IN_PACK];
  qint32 currentOnlineTriggerPosHigh[PARAMS_IN_PACK];
  qint32 currentOnlineTriggerPosLow[PARAMS_IN_PACK];
  QwtPlotCurveAdvanced* curveCopyTc[PARAMS_IN_PACK];
  t_xydata_live xydataCycled[PARAMS_IN_PACK];  //используеться в plotTC для UpdateCurveToInf
  ///////////////////////////
  QwtPlotCurveAdvanced  *m_curve[PARAMS_IN_PACK];

  qint32 currentXPos;
  double motorRpm;
  double motorVdc;

} MotorHndls;



namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();


private:

    Ui::MainWindow *ui;
    QUdpSocket *udpDebug;
    QUdpSocket *udpID;
    QUdpSocket *udpConfig;
    QUdpSocket *udpDbgNames;

    mPower *pwr;

    QHash <int, MotorHndls*> idMotorHash;
    QList<MotorHndls*> motors;

    MotorHndls* currentMotor;
    MotorHndls* previousMotor;
    mTab *ctrl;

    QwtPlotAdvanced *qwtPlotPer, *qwtPlotTc, *qwtPlotXy;
    QwtPlotCurveAdvanced *triggerWindowCenterCurve;
    QwtPlotCurveAdvanced *lastUpdateCurve;
    QVector<double> triggerWindowCenterX;
    QVector<double> triggerWindowCenterY;
    XYCurveData cXYData;

    Colors col;

    double x_scaled0,x_scaled1;

    int AddMotor(QHostAddress *address,int id);
    int  isExistMotor(QHostAddress *address);

    QString ResolveError(int start_block);
    double SWAPDOUBLE(double src);

    void DrawTriggerLines(QwtPlotAdvanced* plot, QwtPlotCurveAdvanced* triggerLineTop, QwtPlotCurveAdvanced* triggerLineBottom);
    void SetTriggerLines(QwtPlotCurveAdvanced* triggerLineTop, QwtPlotCurveAdvanced* triggerLineBottom);
    void FindTriggerPoints( MotorHndls* motor, int id);
    void UpdateTriggerPoints( MotorHndls* motor, int id);
    void UpdateParNamesCombo();
    void RedrawCapture( int index );
    void OnlineRedrawCapture(int id);
    void OnlineTriggerCapture();
    void UpdateTriggerPointsOnline( MotorHndls* motor, int id );
    bool SortAndAddTriggersOnline(MotorHndls* motor, int id, int i);
    void UpdateScrolCapture( int index );
    void UpdateCurvesToInf( int index );
    void AddOutBoundedPointsToCurves( double windowCenter, double shift );
    void UpdateCurvesToDef();
    void ReAttachTriggerPoints(int id);
    void DrawTriggerWindowCenter();
    bool IsPointInWindow(double pointInWindow, double pointToCheck);
    QRectF createRectAroundCenter(const QPointF& center, double width, double height);
    void resetPlotTcZoom( QPointF center );
    void DrawLastSampleUpdate();
    void AddXYFunc(QLabel *cLabel,  QFrame *frame, QComboBox *px, QComboBox *py, QLineEdit *step, QPushButton *delButton);


private slots:

    void p_selected(int,int);
    void MotorDebug(int motor_id, bool enable);

    void ParcePacket( MotorHndls *motor, quint32 *data, bool useScaledOverrun );

    void readPendingDatagramsDebug();
    void readPendingDatagramsID();
    void readPendingDatagramsConfig();
    void readPendingDatagramsDbgName();
    void zoomed  (const QRectF &);
    void stop();

    void fileScrollValueChanged(int value);

    void on_pushLoggingOn_clicked(bool checked);
    void on_pushResetScale_clicked();
    void on_pushAutoscale_clicked(bool checked);
    void on_pushOpenLog_clicked();
    void on_comboMotors_currentIndexChanged(int index);
    void on_tabWidget_currentChanged(int index);
    void on_spinBox_valueChanged(int arg1);
    void on_comboMotors_activated(int index);

    //trigger
    void p_selected_tc(int,int);
    void on_lineEdit_TopTrigger_editingFinished();
    void on_lineEdit_BottomTrigger_editingFinished();
    void on_pushButton_TopTrigger_clicked(bool checked);
    void on_pushButton_BottomTrigger_clicked(bool checked);
    //void on_comboBox_SourceSelect_activated(int index);
    void on_comboBox_SourceSelect_currentIndexChanged(int index);
    void on_comboBox_TriggerSelect_currentIndexChanged(int index);
    void on_lineEdit_FrameWidth_editingFinished();
    void on_lineEdit_FrameHeight_editingFinished();
    void on_verticalScrollBar_Capture_valueChanged(int value);
    void on_toolButton_clicked();
    void on_pushButton_addxy_clicked();

private:

};

#endif // MAINWINDOW_H

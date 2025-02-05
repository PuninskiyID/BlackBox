#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QGoodWindow>
#include <QGoodCentralWidget>

#include "XYCurveData.h"
#include "mpower.h"
#include "mtab.h"
#include "common.h"
#include "series_data.h"
#include "qwt_plot_advanced.h"
#include "ui_centralwidget.h"

typedef class MotorHndl
{
public:


  t_Motor motorAddress;
  QFile logFile;
  unsigned int logFileOffset;
  int logSize;
  t_xydata_live xydata[PARAMS_IN_PACK];

  bool highState[PARAMS_IN_PACK];
  QVector<QwtPlotMarker*>* triggerPointsHigh[PARAMS_IN_PACK];
  QVector<QwtPlotMarker*>* triggerPointsLow[PARAMS_IN_PACK];
  qint32 currentOnlineTriggerPosHigh[PARAMS_IN_PACK];
  qint32 currentOnlineTriggerPosLow[PARAMS_IN_PACK];
  QwtPlotCurveAdvanced* curveCopyTc[PARAMS_IN_PACK];
  t_xydata_live xydataCycled[PARAMS_IN_PACK];

  int logScrollValuePer = 0;
  int logScrollValueXy = 0;

  QwtPlotCurveAdvanced  *m_curve[PARAMS_IN_PACK];

  qint32 currentXPos;
  double motorRpm;
  double motorVdc;

} MotorHndls;







class CentralWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr) : QMainWindow(parent), ui(new Ui::CentralWidget)
    {
        ui->setupUi(this);
        init();
    }

    ~CentralWidget()
    {
        delete ui;
    }

    Ui::CentralWidget *ui;

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
    void SetFileScrollValuePer();
    void SetFileScrollValueXy();


private slots:
    void p_selected(int,int);
    void p_selected_tc(int,int);
    void MotorDebug(int motor_id, bool enable);

    void ParcePacket( MotorHndls *motor, quint32 *data, bool useScaledOverrun );

    void readPendingDatagramsDebug();
    void readPendingDatagramsID();
    void readPendingDatagramsConfig();
    void readPendingDatagramsDbgName();
    void zoomed  (const QRectF &);

    void fileScrollValueChanged(int value);

    void on_pushOpenLog_clicked();
    void on_pushLoggingOn_clicked();
    void on_pushDrawMode_clicked();
    void on_pushAutoscale_clicked(bool checked);
    void on_pushResetScale_clicked();
    void on_comboMotors_currentIndexChanged(int index);

    void on_comboBox_SourceSelect_currentIndexChanged(int index);

    void on_comboBox_TriggerSelect_currentIndexChanged(int index);

    void on_lineEdit_FrameWidth_editingFinished();

    void on_lineEdit_FrameHeight_editingFinished();

    void on_verticalScrollBar_Capture_valueChanged(int value);

    void on_spinBox_valueChanged(int arg1);

    void on_toolButton_TriggerLock_clicked(bool checked);

    void on_pushButton_TopTrigger_clicked(bool checked);

    void on_pushButton_BottomTrigger_clicked(bool checked);

    void on_lineEdit_TopTrigger_editingFinished();

    void on_lineEdit_BottomTrigger_editingFinished();

    void on_pushButton_addxy_clicked();

    void on_pushButton_clicked();

    void on_tabWidget_Charts_currentChanged(int index);

private:
    void init();
};











class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //Functions
    bool nativeEvent(const QByteArray &eventType, void *message, qgoodintptr *result);
    void closeEvent(QCloseEvent *event);

    //Variables
    QGoodCentralWidget *m_good_central_widget;
    CentralWidget *m_central_widget;
};

#endif // MAINWINDOW_H

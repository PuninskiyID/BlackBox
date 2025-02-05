#ifdef _WIN32

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <windows.h>
#include <dwmapi.h>

#endif

#include "mainwindow.h"
#include "ui_centralwidget.h"
#include "common.h"
#include "zipper.h"

#define PER 0.0000625
#define MAX_TRIGGERS 32

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent)
{
    m_central_widget = new CentralWidget(this);
    m_central_widget->setWindowFlags(Qt::Widget);

    m_good_central_widget = new QGoodCentralWidget(this);

    QMenuBar *menu_bar = m_central_widget->menuBar();

#ifdef QGOODWINDOW
    //macOS uses global menu bar
#ifndef Q_OS_MAC
    //Set font of menu bar
    QFont font = menu_bar->font();
    font.setPixelSize(12);
#ifdef Q_OS_WIN
    font.setFamily("Segoe UI");
#else
    font.setFamily(qApp->font().family());
#endif
    menu_bar->setFont(font);

    QTimer::singleShot(0, this, [=]{
        menu_bar->setFixedWidth(menu_bar->sizeHint().width());
        const int title_bar_height = m_good_central_widget->titleBarHeight();
        menu_bar->setStyleSheet(QString("QMenuBar {height: %0px;}").arg(title_bar_height));
    });

    m_good_central_widget->setLeftTitleBarWidget(menu_bar);
#endif

    QWidget *widget = new QWidget();

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QPushButton *button = new QPushButton("Toggle All!", widget);

    connect(button, &QPushButton::clicked, this, [=]{
        m_good_central_widget->setTitleVisible(!m_good_central_widget->isTitleVisible());

        if (m_good_central_widget->iconVisibility() == QGoodCentralWidget::IconVisibilityType::IconHidden)
        {
#ifndef Q_OS_MAC
            m_good_central_widget->setIconVisibility(QGoodCentralWidget::IconVisibilityType::IconOnLeftOfWindow);
#else
            m_good_central_widget->setIconVisibility(QGoodCentralWidget::IconVisibilityType::IconOnLeftOfTitle);
#endif
        }
        else
        {
            m_good_central_widget->setIconVisibility(QGoodCentralWidget::IconVisibilityType::IconHidden);
        }

        m_good_central_widget->setTitleBarColor(!m_good_central_widget->isTitleVisible() ? Qt::red : QColor());
        m_good_central_widget->setActiveBorderColor(!m_good_central_widget->isTitleVisible() ? Qt::red : QColor());
    });

    QLabel *label = new QLabel("Label", widget);
    label->setAttribute(Qt::WA_TransparentForMouseEvents);

    layout->addWidget(button);
    layout->addWidget(label);

    m_good_central_widget->setRightTitleBarWidget(widget, true);
#endif

    QMenu *about_menu = new QMenu("About", menu_bar);
    QAction *action_about_qt = about_menu->addAction("About Qt");
    QAction *action_about_qgoodwindow = about_menu->addAction("About QGoodWindow");

    connect(action_about_qt, &QAction::triggered, this, [=]{QMessageBox::aboutQt(this);});
    connect(action_about_qgoodwindow, &QAction::triggered, this, [=]{QGoodWindow::aboutQGoodWindow(this);});

    menu_bar->addMenu(about_menu);

    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, [=]{
        if (qGoodStateHolder->isCurrentThemeDark())
        {
            QGoodWindow::setAppDarkTheme();
            setNativeDarkModeEnabledOnWindows(true);
        }
        else
        {
            QGoodWindow::setAppLightTheme();
            setNativeDarkModeEnabledOnWindows(false);
        }
    });

    connect(this, &QGoodWindow::systemThemeChanged, this, [=]{
        qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());
    });

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);

    connect(shortcut1, &QShortcut::activated, this, [=]{
        qGoodStateHolder->setCurrentThemeDark(!qGoodStateHolder->isCurrentThemeDark());
    });

    QShortcut *shortcut2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this);

    connect(shortcut2, &QShortcut::activated, this, [=]{
        m_good_central_widget->setTitleBarVisible(!m_good_central_widget->isTitleBarVisible());
    });

    qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());

    m_good_central_widget->setCentralWidget(m_central_widget);
    setCentralWidget(m_good_central_widget);

    setWindowIcon(qApp->style()->standardIcon(QStyle::SP_DesktopIcon));
    setWindowTitle("Good Window - CTRL+S toggle theme - CTRL+T toggle title bar!");

    resize(800, 600);
    move(qApp->primaryScreen()->availableGeometry().center() - rect().center());

#ifdef QGOODWINDOW
    m_good_central_widget->setTitleAlignment(Qt::AlignCenter);
#endif
}

MainWindow::~MainWindow()
{

}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qgoodintptr *result)
{
#ifdef QGOODWINDOW
#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
    MSG *msg = static_cast<MSG*>(message);

    switch (msg->message)
    {
    case WM_THEMECHANGED:
    case WM_DWMCOMPOSITIONCHANGED:
    {
        //Keep window theme on Windows theme change events.
        QTimer::singleShot(100, this, [=]{
            if (qGoodStateHolder->isCurrentThemeDark())
                QGoodWindow::setAppDarkTheme();
            else
                QGoodWindow::setAppLightTheme();
        });

        break;
    }
    default:
        break;
    }
#endif
#endif
#endif
    return QGoodWindow::nativeEvent(eventType, message, result);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgbox(this);
    msgbox.setIcon(QMessageBox::Question);
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::No);
    msgbox.setText("Are you sure to close?");

    int result = QGoodCentralWidget::execDialogWithWindow(&msgbox, this, m_good_central_widget);

    if (result != QMessageBox::Yes)
    {
        event->ignore();
        return;
    }
}

void CentralWidget::on_pushOpenLog_clicked()
{
    QMovie *loadingGif = new QMovie("D:\\cpp\\work\\tms570_ethernet_tools\\Textures\\copium-cat.gif");

        ui->label_load->setMovie(loadingGif);
        ui->label_load->setEnabled(true);
        ui->label_load->show();
        loadingGif->start();


         QString fileName;
         fileName = QFileDialog::getOpenFileName(this,tr("Open Log"),QCoreApplication::applicationDirPath() + "\\log-files",
                                                 tr("Log Files (*.*);;Log Files (*.zip);;Log Files (*.rar);;Log Files (*.bin)"));
         if( fileName.isNull() || fileName.isEmpty() || !QFile::exists(fileName) )
         {
             ui->label_load->setDisabled(true);
             ui->label_load->hide();
             return;
         }

         if(fileName.contains(".zip"))
         {
             Zipper::uncompressZipToFolder(fileName, QCoreApplication::applicationDirPath() + "/log-files/unziped");

             fileName.remove(QCoreApplication::applicationDirPath() + "/log-files/");
             fileName.remove(".zip");
             fileName = QCoreApplication::applicationDirPath() + "/log-files/unziped/" + fileName + ".bin";
         }
         else if(fileName.contains(".rar"))
         {
             Zipper::uncompressRarToFolder(fileName, QCoreApplication::applicationDirPath() + "/log-files/unziped");

             fileName.remove(QCoreApplication::applicationDirPath() + "/log-files/");
             fileName.remove(".rar");
             fileName = QCoreApplication::applicationDirPath() + "/log-files/unziped/" + fileName + ".bin";
         }



         if( fileName.isNull() || fileName.isEmpty() || !QFile::exists(fileName) )
         {
             ui->label_load->setEnabled(false);
             ui->label_load->hide();
             return;
         }

          QHostAddress a;
          a.setAddress("0.0.0.0");

          AddMotor(&a,-1);

          MotorHndls *motor = idMotorHash[-1];

          if (motor->logFile.isOpen()) motor->logFile.close();

          motor->logFile.setFileName(fileName);
          motor->logFile.open(QIODevice::ReadOnly);

          if( motor->logFile.isOpen() )
          {
              motor->logFileOffset = ctrl->ReadNamesFromFile(motor->logFile);
              UpdateParNamesCombo();
          }

          QDir fn(fileName);
          ui->horizontalSlider->setMaximum(motor->logFile.size()/(sizeof(float)*PARAMS_IN_PACK)-X_SIZE);
          ui->horizontalSlider->setSingleStep( (X_SIZE + 9) / 10 );
          ui->horizontalSlider->setValue(0);
          ui->horizontalSlider->setEnabled(true);

          fileScrollValueChanged(0);
          ui->label_load->hide();
          ui->label_load->setEnabled(false);
}

void CentralWidget::fileScrollValueChanged(int position)
{
    static char data[X_SIZE*PARAMS_IN_PACK*sizeof(float)];

    MotorHndls *motor=idMotorHash[-1];

    if (motor==0) return;

    if (!motor->logFile.isOpen()) return;

    for(int i = 0; i < PARAMS_IN_PACK; i++)
        motor->highState[i] = false;

    motor->logFile.seek((qint64)position*(qint64)PARAMS_IN_PACK*(qint64)sizeof(float) + (qint64)motor->logFileOffset);

    int lineSize = motor->logFile.read((char*)data, X_SIZE*PARAMS_IN_PACK*sizeof(float));

    motor->currentXPos=0;
    int counter = 0;

    for(int i=0;i<lineSize/PACKET_SIZE;i++)
    {
        ParcePacket( motor,(quint32*)(data + i*PACKET_SIZE ), 0 );
    }
    if(ui->tabWidget_Charts->currentIndex() == 0)
    {
        for (int i = 0; i < PARAMS_IN_PACK; ++i)
            if(ctrl->IsChecked(i))
            {
                ReAttachTriggerPoints(i);
                if(ui->comboBox_SourceSelect->currentIndex() > 0 && ui->comboBox_TriggerSelect->currentIndex() > 0)
                {
                    UpdateScrolCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
                    RedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
                    UpdateCurvesToInf(ui->comboBox_SourceSelect->currentIndex() - 1);
                }
            }
        qwtPlotPer->replot();
        qwtPlotTc->replot();
    }
    else
    {
        if(cXYData.Size() > 0)
        {
            for(int index = 0; index < cXYData.Size(); index++)
            {
                cXYData.at(index)->XYParamPick( motor->currentXPos, (X_SIZE) );
            }
        }
        qwtPlotXy->replot();
    }
}


void CentralWidget::on_pushLoggingOn_clicked()
{

}


void CentralWidget::on_pushDrawMode_clicked()
{

}


void CentralWidget::on_pushAutoscale_clicked(bool checked)
{
    qwtPlotPer->setAxisAutoScale(QwtPlot::yLeft,checked);
    qwtPlotPer->replot();
}


void CentralWidget::on_pushResetScale_clicked()
{
    qDebug()<<"Reset";
        qwtPlotPer->resetZoom( QRectF( 0.0, -5000.0, X_SIZE * PER, 10000.0 ) );
    //    qwtPlotPer->setAxisScale(QwtPlot::yLeft,-5000,5000,0);
    //    qwtPlotPer->setAxisScale(QwtPlot::xBottom,0,X_SIZE * 0.0000625,0);
        qwtPlotPer->replot();
     //   x_scaled0=qwtPlotPer->canvasMap(QwtPlot::xBottom).s1();
     //   x_scaled1=qwtPlotPer->canvasMap(QwtPlot::xBottom).s2();
}

void CentralWidget::UpdateCurvesToInf( int index )
{
        double windowCenter = qwtPlotTc->zoomBase().center().x();
        double shift = ui->lineEdit_FrameWidth->text().toDouble() / 2;
        AddOutBoundedPointsToCurves( windowCenter, shift );
}

void CentralWidget::UpdateCurvesToDef()
{

    for(int i = 0; i < PARAMS_IN_PACK; i++)
        if(ctrl->IsChecked(i))
            currentMotor->curveCopyTc[i]->setRawSamples( currentMotor->xydata[i].x, currentMotor->xydata[i].y, (X_SIZE) );
    ui->verticalScrollBar_Capture->setMinimum(0);
    ui->verticalScrollBar_Capture->setEnabled(false);
    qwtPlotTc->resetZoom( QRectF( 0.0, -5000.0, X_SIZE * PER, 10000.0 ) );

}

void CentralWidget::AddOutBoundedPointsToCurves( double windowCenter, double shift )
{
    int shiftedPoints = shift / PER;

    for(int i = 0; i < PARAMS_IN_PACK; i++)
    {
        if(ctrl->IsChecked(i))
        {
            for(int j = 0; j < X_SIZE; j++)
            {
                if( windowCenter + shift >  currentMotor->xydata[i].x[X_SIZE - 1])
                {
                    currentMotor->xydataCycled[i].x[j] =  currentMotor->xydata[i].x[((X_SIZE) + j + shiftedPoints ) % (X_SIZE)];
                    if( j + shiftedPoints >= (X_SIZE) )
                        currentMotor->xydataCycled[i].x[j] =  currentMotor->xydata[i].x[((X_SIZE) + j + shiftedPoints ) % (X_SIZE)]
                            + currentMotor->xydata[i].x[(X_SIZE) - 1] + PER;
                    currentMotor->xydataCycled[i].y[j] =  currentMotor->xydata[i].y[((X_SIZE) + j + shiftedPoints ) % (X_SIZE)];

                }
                else if( windowCenter - shift <  currentMotor->xydata[i].x[0] )
                {

                    if( j - shiftedPoints < 0 )
                        currentMotor->xydataCycled[i].x[j] =  currentMotor->xydata[i].x[((X_SIZE) + j - shiftedPoints) % (X_SIZE)]
                            - currentMotor->xydata[i].x[(X_SIZE) - 1] - PER;
                    else
                        currentMotor->xydataCycled[i].x[j] =  currentMotor->xydata[i].x[((X_SIZE) + j - shiftedPoints) % (X_SIZE)];
                    currentMotor->xydataCycled[i].y[j] =  currentMotor->xydata[i].y[((X_SIZE) + j - shiftedPoints) % (X_SIZE)];
                }
                else
                {
                    currentMotor->xydataCycled[i].x[j] =  currentMotor->xydata[i].x[((X_SIZE) + j ) % (X_SIZE)];
                    currentMotor->xydataCycled[i].y[j] =  currentMotor->xydata[i].y[((X_SIZE) + j) % (X_SIZE)];
                }
            }

            currentMotor->curveCopyTc[i]->setRawSamples( currentMotor->xydataCycled[i].x,
                    currentMotor->xydataCycled[i].y, (X_SIZE) );
        }
    }
}

void CentralWidget::RedrawCapture(int index)
{
    if(index <= 0)
    {
        qDebug()<<"1";
        UpdateCurvesToDef();
        qwtPlotTc->resetZoom( QRectF( 0.0, -5000.0, X_SIZE * PER, 10000.0 ) );
        DrawTriggerWindowCenter();
        return;
    }

    if(ui->comboBox_TriggerSelect->currentIndex() == 1 )
    {
        qDebug()<<"2";
        if(currentMotor->triggerPointsHigh[index]->size() == 0 )
        {
            UpdateCurvesToDef();
            qwtPlotTc->replot();
            DrawTriggerWindowCenter();
            return;
        }
        QPointF centerPoint = currentMotor->triggerPointsHigh[index]->at( ui->verticalScrollBar_Capture->value( ) )->value();
        //centerPoint.setY(ui->lineEdit_TopTrigger->text().toDouble());
        centerPoint.setY(0);
        QRectF rec = createRectAroundCenter( centerPoint, ui->lineEdit_FrameWidth->text().toFloat(), ui->lineEdit_FrameHeight->text().toFloat() );

        qwtPlotTc->resetZoom(rec);
        qwtPlotTc->replot();
        DrawTriggerWindowCenter();
    }

    else if(ui->comboBox_TriggerSelect->currentIndex() == 2 )
    {
        qDebug()<<"3";
        if( currentMotor->triggerPointsLow[index]->size() == 0 )
        {
            UpdateCurvesToDef();
            qwtPlotTc->replot();
            DrawTriggerWindowCenter();
            return;
        }

        QPointF centerPoint = currentMotor->triggerPointsLow[index]->at( ui->verticalScrollBar_Capture->value() )->value();
        centerPoint.setY(ui->lineEdit_BottomTrigger->text().toDouble());
        QRectF rec = createRectAroundCenter( centerPoint, ui->lineEdit_FrameWidth->text().toFloat(), ui->lineEdit_FrameHeight->text().toFloat() );

        qwtPlotTc->resetZoom(rec);
        qwtPlotTc->replot();
        DrawTriggerWindowCenter();
    }

    else
    {
        qDebug()<<"4";
        UpdateCurvesToDef();
        qwtPlotTc->resetZoom( QRectF( 0.0, -5000.0, X_SIZE * PER, 10000.0 ) );
//        qwtPlotTc->resetZoom( QRectF( 0.0, -(ui->lineEdit_FrameHeight->text().toFloat() / 2) ,
//                                      ui->lineEdit_FrameWidth->text().toFloat(), ui->lineEdit_FrameHeight->text().toFloat() ) );
        DrawTriggerWindowCenter();
    }
}

void CentralWidget::UpdateScrolCapture(int index)
{
    if ( ui->comboBox_TriggerSelect->currentIndex() == 1 )
    {
        ui->verticalScrollBar_Capture->setEnabled(true);
        ui->verticalScrollBar_Capture->setMaximum(fmax(0, currentMotor->triggerPointsHigh[index]->size() - 1));
        ui->verticalScrollBar_Capture->setSingleStep(1);
        ui->verticalScrollBar_Capture->setValue(currentMotor->triggerPointsHigh[index]->size() - 1);
    }
    else if ( ui->comboBox_TriggerSelect->currentIndex() == 2 )
    {
        ui->verticalScrollBar_Capture->setEnabled(true);
        ui->verticalScrollBar_Capture->setMaximum(fmax(0,currentMotor->triggerPointsLow[index]->size() - 1));
        ui->verticalScrollBar_Capture->setSingleStep(1);
        ui->verticalScrollBar_Capture->setValue(currentMotor->triggerPointsLow[index]->size() - 1);
    }
    else
    {
        ui->verticalScrollBar_Capture->setMinimum(0);
        ui->verticalScrollBar_Capture->setEnabled(false);
    }
}

void CentralWidget::on_comboMotors_currentIndexChanged(int index)
{
    if (ui->comboMotors->count()==0) return;

        for(int i = 0; i < cXYData.Size(); i++)
        {
            cXYData.DeleteCurve(i);
        }

        for(int id = 0; id < PARAMS_IN_PACK; id++)
            if(ctrl->chk[id]->isChecked())
            {
                if( ui->pushButton_TopTrigger->isChecked() )
                    for (int i = 0; i < currentMotor->triggerPointsHigh[id]->size(); i++)
                        currentMotor->triggerPointsHigh[id]->at(i)->detach();
                if( ui->pushButton_BottomTrigger->isChecked() )
                    for (int i = 0; i < currentMotor->triggerPointsLow[id]->size(); i++)
                        currentMotor->triggerPointsLow[id]->at(i)->detach();
            }

        ui->verticalScrollBar_Capture->setEnabled(false);

        MotorDebug(currentMotor->motorAddress.id,false);

        currentMotor = idMotorHash[ui->comboMotors->itemData(index).toInt()];

        MotorDebug(currentMotor->motorAddress.id,true);

        qwtPlotPer->detachItems(QwtPlotItem::Rtti_PlotCurve, false);
        qwtPlotTc->detachItems(QwtPlotItem::Rtti_PlotCurve, false);


        for (int j=0;j<motors.size();j++)
        {
    //        for (int i=0;i<PARAMS_IN_PACK;i++)
    //        {
    //            previousMotor->m_curve[i]->detach();
    //        }
            for (int i=0;i<PARAMS_IN_PACK;i++)
            {
                if (ctrl->IsChecked(i))
                {
                    currentMotor->m_curve[i]->attach(qwtPlotPer, 1);
                    currentMotor->curveCopyTc[i]->attach(qwtPlotTc, 1);
                    ReAttachTriggerPoints(i);
                }
            }
        }

        if(currentMotor->logFile.isOpen())
        {
            ui->verticalScrollBar_Capture->setEnabled(true);
            ui->horizontalSlider->setEnabled(true);
        }
        else
        {
            ui->horizontalSlider->setEnabled(false);
        }

        qwtPlotPer->replot();
        qwtPlotTc->replot();

        previousMotor = currentMotor;

        const char v=(char)ui->spinBox->value();
        qDebug()<<ui->spinBox->value();
        QUdpSocket().writeDatagram(&v,currentMotor->motorAddress.ip,P_UDP_MIN_DBG_NAMES_M+currentMotor->motorAddress.id);
}

int NormCycledPos( int pos, int cycleSize )
{
    return ((pos + cycleSize) % cycleSize);
}

void AddNewMarker(QPointF point, QVector<QwtPlotMarker*>* points[PARAMS_IN_PACK], int id)
{
    points[id]->push_back(new QwtPlotMarker);
    points[id]->back()->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::gray), QPen(Qt::cyan), QSize(4, 4)));
    points[id]->back()->setValue(point);
}

void SetValueToExistMarker(QPointF point, QVector<QwtPlotMarker*>* points[PARAMS_IN_PACK], int id, int pos[32])
{
    points[id]->at(pos[id])->detach();
    points[id]->at(pos[id])->setValue(point);

}

void CentralWidget::p_selected(int id,int cheked)
{
    if (!currentMotor) return;
    if (cheked) currentMotor->m_curve[id]->attach(qwtPlotPer, 1);

    else currentMotor->m_curve[id]->detach();

    qwtPlotPer->replot();
}

void CentralWidget::MotorDebug( int motor_id, bool enable )
{
    MotorHndls *motor = idMotorHash[motor_id];

    if (enable) QUdpSocket().writeDatagram( "\1", motor->motorAddress.ip,P_UDP_MIN_DEBUG_M + motor->motorAddress.id );
    else QUdpSocket().writeDatagram( "\0", motor->motorAddress.ip,P_UDP_MIN_DEBUG_M + motor->motorAddress.id );
}

void CentralWidget::zoomed  (const QRectF & rect)
{
//    int flag=0;
//    x_scaled0=rect.x();
//    x_scaled1=rect.x()+rect.width();

//    if (x_scaled0<0)
//    {
//        x_scaled0=0;
//        flag=1;
//    }
//    if (x_scaled1>X_SIZE * PER)
//    {
//        x_scaled1=X_SIZE * PER;
//        flag=1;
//    }
//    if (flag)
//    {
//        qwtPlotPer->setAxisScale(QwtPlot::xBottom,x_scaled0,x_scaled1,0);
//        qwtPlotPer->replot();
//    }
//    ui->pushAutoscale->setChecked(qwtPlotPer->axisAutoScale(QwtPlot::yLeft));
}

void CentralWidget::readPendingDatagramsID()
{
    char bdatagram[128];
    QHostAddress address;
    quint16 port;
    int motor_id;

    while (udpID->hasPendingDatagrams())
    {
       udpID->readDatagram(bdatagram,128,&address,&port);

       motor_id=port-P_UDP_MIN_ALIVE_M;
       if ((motor_id<0)||(motor_id>10)) continue;

       AddMotor(&address,motor_id);

    }
}

void CentralWidget::readPendingDatagramsConfig()
{


}

void CentralWidget::readPendingDatagramsDbgName()
{
    char bdatagram[128];
    QHostAddress address;
    quint16 port;

    while (udpDbgNames->hasPendingDatagrams())
    {
       udpDbgNames->readDatagram(bdatagram,128,&address,&port);

       QString bs;
       bs.append(bdatagram);

       QStringList list;
       list = bs.split(",");

       if (list.count()!=2)
       {
           qDebug()<<"Parameter name parse error";
           continue;
       }
       ctrl->SetParName(list.at(0).toInt(),list.at(1));
    }
    UpdateParNamesCombo();
}

void CentralWidget::readPendingDatagramsDebug()
{
    static char udpData[PACKET_SIZE];

    MotorHndls * motor;
    QHostAddress address;
    quint16 port;

    static quint64 counter=0;

    while (udpDebug->hasPendingDatagrams())
    {

       udpDebug->readDatagram((char*)udpData,PACKET_SIZE,&address,&port);
       int motor_id=port-P_UDP_MIN_DEBUG_M;

       motor=idMotorHash[motor_id];

       if (motor==0 && address.toIPv4Address() == 0x7f000001U)
       {
           AddMotor(&address, 0);
       }

       if (motor==0) continue;

       if (motor->logFile.isOpen())
       {
           const char *w_data=(const char*)udpData;
           if (motor->logFile.write(w_data,PACKET_SIZE)!=PACKET_SIZE)
           {
               qDebug()<<"Error writing log file";
           }
       }

       if (motor!=currentMotor) continue;

       /* online color loop */ {
           static int s = 0;
           static unsigned int cnt = 10000;
           cnt++;
           if( cnt > 1000 )
           {
               s = !s;
               cnt = 0;
//               ui->tab->setStyleSheet( s? "color: red" : "color: green" );
//               ui->tabWidget->currentWidget()->setStyleSheet( s? "color: red" : "color: green" );
               ui->tabWidget->tabBar()->setStyleSheet(s? "color: red" : "color: green");
           }
       }

       ParcePacket( motor,( uint32_t* )udpData, 1);


       counter++;
       if (counter%128==0) //было 200 //128
       {
           //OnlineTriggerCapture();
           //DrawLastSampleUpdate();

           qwtPlotTc->replot();
           qwtPlotPer->replot();
           qwtPlotXy->replot();

           if ( !ui->pushDrawMode->isChecked() )
           {
               for (int i=0;i<PARAMS_IN_PACK;i++)
               {
                   ctrl->SetLab(i,motor->xydata[i].y[motor->currentXPos-1]);
               }
           }
       }

    }
}

int  CentralWidget::AddMotor(QHostAddress *address,int id)
{
    if (isExistMotor(address)) return 1;

    MotorHndls *motor = new MotorHndls();

    motor->motorRpm=0;
    motor->motorVdc=0;
    motor->motorAddress.ip=*address;
    motor->motorAddress.id=id;

    for (int i = 0; i < PARAMS_IN_PACK; ++i)
    {
        motor->triggerPointsHigh[i] = new QVector<QwtPlotMarker*>;
        motor->triggerPointsLow[i] = new QVector<QwtPlotMarker*>;
    }

    for ( int i = 0; i < PARAMS_IN_PACK; i++ )
    {
        auto curve = qwtPlotPer->addCurve(QString().asprintf("%s:%d-%d", address->toString().toUtf8().data(), id, i));
        motor->m_curve[i] = curve;
        motor->m_curve[i]->setRawSamples( motor->xydata[i].x, motor->xydata[i].y, X_SIZE );
        motor->m_curve[i]->setPen( col[i] );
        //motor->curveCopyTc[i] = motor->m_curve[i];

        auto curveTc = qwtPlotTc->addCurve(QString().asprintf("%s:%d-%d", address->toString().toUtf8().data(), id, i));
        motor->curveCopyTc[i] = curveTc;
        motor->curveCopyTc[i]->setRawSamples( motor->xydata[i].x, motor->xydata[i].y, X_SIZE );
        motor->curveCopyTc[i]->setPen( col[i] );

    }

    idMotorHash.insert(motor->motorAddress.id, motor);

    pwr->Addmotor(&motor->motorAddress);

    currentMotor=motor;
    previousMotor=motor;

    motors.append(motor);

    ui->comboMotors->addItem(motor->motorAddress.ip.toString()+" #"+QString().setNum(motor->motorAddress.id),motor->motorAddress.id);
    ui->comboMotors->setCurrentIndex(ui->comboMotors->findText(motor->motorAddress.ip.toString()+" #"+QString().setNum(motor->motorAddress.id)));

    on_comboMotors_currentIndexChanged(motors.size()-1);

    return 0;
}

void CentralWidget::UpdateParNamesCombo()
{
    ui->comboBox_SourceSelect->clear();
    ui->comboBox_SourceSelect->addItem( "None" );
    for( int i = 0; i < PARAMS_IN_PACK; i++ )
    {
        ui->comboBox_SourceSelect->addItem( ctrl->chk[i]->text() );
    }
}

int  CentralWidget::isExistMotor(QHostAddress *address)
{
    for (int i=0;i<motors.size();i++)
    {
        if (motors[i]->motorAddress.ip==*address) return 1;
    }
    return 0;
}

void CentralWidget::ParcePacket( MotorHndls *motor, quint32 *data, bool useScaledOverrun = 1 )
{
    //qDebug()<<"parsePack";
    quint32 hostByteOrder;
    float *hostFloatValue;
    static qint32 window_size=20000;

    for(unsigned int i=0;i<PACKET_SIZE/sizeof(float)/PACKS_IN_PACKET;i++)
    {

        for(int j=0;j<PARAMS_IN_PACK;j++)
        {
            int idx=i*PARAMS_IN_PACK+j;

            if(motor->motorAddress.ip.toIPv4Address() != 0x7f000001U)
                hostByteOrder=be32toh(*(data+idx));
            else
                hostByteOrder=*(data+idx);

            hostFloatValue=(float*)&hostByteOrder;

            motor->xydata[j].x[motor->currentXPos]=motor->currentXPos * PER;
            motor->xydata[j].y[motor->currentXPos]=*hostFloatValue;

        }


        if( (!motor->logFile.isOpen()) && motor->currentXPos%128==0)
        {
            OnlineTriggerCapture();
            DrawLastSampleUpdate();
            if(cXYData.Size() > 0)
            {
                for(int index = 0; index < cXYData.Size(); index++)
                {
                    //qDebug()<<"FromParcePacket "<< motor->currentXPos;
                    cXYData.at(index)->XYParamPick( motor->currentXPos );
                }
            }
        }

        motor->currentXPos++;



        if (useScaledOverrun && motor->currentXPos >= (x_scaled1 / PER))
        {
            motor->currentXPos = x_scaled0 / PER;
        }

        motor->currentXPos = motor->currentXPos % (X_SIZE); //здесь нормализуентся значение для циклического обращения
    }

}

void CentralWidget::OnlineTriggerCapture()
{
    for(int id = 0; id < PARAMS_IN_PACK; id++)
    {
        if( !ctrl->IsChecked(id)) continue;
        ReAttachTriggerPoints(id);
    }
}

void CentralWidget::ReAttachTriggerPoints(int id)
{
    if( ui->pushButton_TopTrigger->isChecked() )
        for (int i = 0; i < currentMotor->triggerPointsHigh[id]->size(); i++)
            currentMotor->triggerPointsHigh[id]->at(i)->detach();
    if( ui->pushButton_BottomTrigger->isChecked() )
        for (int i = 0; i < currentMotor->triggerPointsLow[id]->size(); i++)
            currentMotor->triggerPointsLow[id]->at(i)->detach();

    if( currentMotor->logFile.isOpen() )
        UpdateTriggerPoints(currentMotor, id);
    else
    {
        if(ctrl->chk[id]->isChecked())
            UpdateTriggerPointsOnline(currentMotor, id);
    }

    if( ui->pushButton_TopTrigger->isChecked() )
        for (int i = 0; i < currentMotor->triggerPointsHigh[id]->size(); i++)
            currentMotor->triggerPointsHigh[id]->at(i)->attach(qwtPlotTc);
    if( ui->pushButton_BottomTrigger->isChecked() )
        for (int i = 0; i < currentMotor->triggerPointsLow[id]->size(); i++)
            currentMotor->triggerPointsLow[id]->at(i)->attach(qwtPlotTc);
}

bool CentralWidget::SortAndAddTriggersOnline(MotorHndls* motor, int id, int i)
{
    bool out = false;

    if( motor->xydata[id].y[i] > ui->lineEdit_TopTrigger->text().toDouble() && motor->highState[id] == false )
    {
        if( !IsPointInWindow(motor->triggerPointsHigh[id]->at( NormCycledPos(motor->currentOnlineTriggerPosHigh[id] - 1, MAX_TRIGGERS) )->xValue(), motor->xydata[id].x[i]) )
        {
            if( motor->triggerPointsHigh[id]->size() < MAX_TRIGGERS )
                AddNewMarker( QPointF(currentMotor->xydata[id].x[i], motor->xydata[id].y[i]), motor->triggerPointsHigh, id);
            else
                SetValueToExistMarker(QPointF(currentMotor->xydata[id].x[i], motor->xydata[id].y[i]), motor->triggerPointsHigh,id, motor->currentOnlineTriggerPosHigh);

            motor->currentOnlineTriggerPosHigh[id] = NormCycledPos(motor->currentOnlineTriggerPosHigh[id] + 1, MAX_TRIGGERS);
            out = true;
        }
        motor->highState[id] = true;
    }
    else if( motor->xydata[id].y[i] < ui->lineEdit_BottomTrigger->text().toDouble() && motor->highState[id] == true )
    {
        if( !IsPointInWindow(motor->triggerPointsLow[id]->at( NormCycledPos(motor->currentOnlineTriggerPosLow[id] - 1, MAX_TRIGGERS) )->xValue(), motor->xydata[id].x[i]) )
        {
            if( motor->triggerPointsLow[id]->size() < MAX_TRIGGERS )
                AddNewMarker( QPointF(currentMotor->xydata[id].x[i], motor->xydata[id].y[i]), motor->triggerPointsLow, id);
            else
            {
                SetValueToExistMarker(QPointF(currentMotor->xydata[id].x[i], motor->xydata[id].y[i]), motor->triggerPointsLow,id, motor->currentOnlineTriggerPosLow);

            }

            motor->currentOnlineTriggerPosLow[id] = NormCycledPos(motor->currentOnlineTriggerPosLow[id] + 1, MAX_TRIGGERS);
            out = true;
        }

        motor->highState[id] = false;
    }
    return out;
}

bool CentralWidget::IsPointInWindow(double pointInWindow, double pointToCheck)
{
    if (pointInWindow + ui->lineEdit_FrameWidth->text().toDouble() < pointToCheck)
        return false;
    else if(pointInWindow > pointToCheck)
        return false;
    else
        return true;
}

void CentralWidget::DrawTriggerWindowCenter()
{

    if( triggerWindowCenterCurve->isAttached() )
    {
        triggerWindowCenterCurve->detach();
    }

    if(!ui->radioButton_TcCenter->isChecked())
    {
        qwtPlotPer->replot();
        return;
    }

    triggerWindowCenterX = { qwtPlotTc->zoomRect().center().x(), qwtPlotTc->zoomRect().center().x() };
    //triggerWindowCenterY = { ui->lineEdit_FrameHeight->text().toDouble(), -ui->lineEdit_FrameHeight->text().toDouble() };
    triggerWindowCenterY = { 5000 , -5000 };
    triggerWindowCenterCurve->setPen( QPen( Qt::green, 1, Qt::SolidLine ) );
    triggerWindowCenterCurve->setSamples( triggerWindowCenterX, triggerWindowCenterY );

    triggerWindowCenterCurve->attach( qwtPlotPer, 1 );
    qwtPlotPer->replot();
}

void CentralWidget::resetPlotTcZoom( QPointF center )
{
    qwtPlotTc->resetZoom( createRectAroundCenter( center, ui->lineEdit_FrameWidth->text().toDouble(), ui->lineEdit_FrameHeight->text().toDouble() ) );
}

QRectF CentralWidget::createRectAroundCenter(const QPointF& center, double width, double height)
{
    double halfWidth = width / 2.0;
    double halfHeight = height / 2.0;

    double topLeftX = center.x() - halfWidth;
    double topLeftY = center.y() - halfHeight;

    return QRectF(topLeftX, topLeftY, width, height);
}

void CentralWidget::OnlineRedrawCapture(int id)
{
    if( ui->comboBox_TriggerSelect->currentIndex() == 0 || ui->comboBox_SourceSelect->currentIndex() == 0)
    {
        qwtPlotTc->resetZoom( QRectF( 0.0, -5000.0, X_SIZE * PER, 10000.0 ) );
        return;
    }

    if( ui->comboBox_TriggerSelect->currentIndex() == 1 )
    {
        for(int i = 0; i < MAX_TRIGGERS; ++i)
        {
            if(currentMotor->triggerPointsHigh[id]->size() < MAX_TRIGGERS) return;
            if(IsPointInWindow(currentMotor->triggerPointsHigh[id]->at(NormCycledPos(currentMotor->currentOnlineTriggerPosHigh[id] - i,MAX_TRIGGERS))->xValue(),
                               currentMotor->xydata[id].x[currentMotor->currentXPos]))
            {
                resetPlotTcZoom(QPointF(currentMotor->triggerPointsHigh[id]->at(NormCycledPos(currentMotor->currentOnlineTriggerPosHigh[id] - i - 1,MAX_TRIGGERS))->xValue(),0));
                DrawTriggerWindowCenter();
                break;
            }
        }
        qwtPlotTc->replot();
    }
    else if( ui->comboBox_TriggerSelect->currentIndex() == 2 )
    {

        for(int i = 0; i < MAX_TRIGGERS; ++i)
        {
            if(currentMotor->triggerPointsLow[id]->size() < MAX_TRIGGERS) return;
            if(IsPointInWindow(currentMotor->triggerPointsLow[id]->at(NormCycledPos(currentMotor->currentOnlineTriggerPosLow[id] - i,MAX_TRIGGERS))->xValue(),
                               currentMotor->xydata[id].x[currentMotor->currentXPos]))
            {
                resetPlotTcZoom(QPointF(currentMotor->triggerPointsLow[id]->at(NormCycledPos(currentMotor->currentOnlineTriggerPosLow[id] - i - 1,MAX_TRIGGERS))->xValue(),0));
                DrawTriggerWindowCenter();
                break;
            }
        }
        qwtPlotTc->replot();
    }
}

void CentralWidget::UpdateTriggerPointsOnline( MotorHndls* motor, int id )
{
    for(int i = motor->currentXPos - 128; i < motor->currentXPos; i++)
    {
        int realXPos = NormCycledPos(i, X_SIZE);

        if(motor->triggerPointsHigh[id]->size() == 0 )
        {
            AddNewMarker( QPointF(0,0), motor->triggerPointsHigh, id);
            motor->currentOnlineTriggerPosHigh[id] += 1;
        }
        if(motor->triggerPointsLow[id]->size() == 0 )
        {
            AddNewMarker( QPointF(0,0), motor->triggerPointsLow, id);
            motor->currentOnlineTriggerPosLow[id] += 1;
        }
        if(SortAndAddTriggersOnline( currentMotor, id, realXPos )){
            //qDebug()<<currentMotor->currentOnlineTriggerPos[id];
            OnlineRedrawCapture( ui->comboBox_SourceSelect->currentIndex() - 1 );
        }
    }
}

void CentralWidget::UpdateTriggerPoints( MotorHndls* motor, int id )
{

    for(int i = 0; i < motor->triggerPointsHigh[id]->size(); i++)
        delete motor->triggerPointsHigh[id]->at(i);
    for(int i = 0; i < motor->triggerPointsLow[id]->size(); i++)
        delete motor->triggerPointsLow[id]->at(i);

    motor->triggerPointsHigh[id]->clear();
    motor->triggerPointsLow[id]->clear();
    motor->highState[id] = false;

    double windowWidth = ui->lineEdit_FrameWidth->text().toDouble();

    for(unsigned long long i = 0; i < (X_SIZE); i++)
    {
        if(motor->xydata[id].y[i] >= ui->lineEdit_TopTrigger->text().toInt() && motor->highState[id] == false)
        {
            if(     motor->triggerPointsHigh[ id ]->size() == 0
                    || ( motor->triggerPointsHigh[ id ]->size() > 0
                    && ( motor->triggerPointsHigh[ id ]->at( motor->triggerPointsHigh[ id ]->size() - 1 )->xValue() + windowWidth < motor->xydata[id].x[i] ) ) )
            {
                QPointF point(motor->xydata[id].x[i], motor->xydata[id].y[i]);

                motor->triggerPointsHigh[id]->push_back(new QwtPlotMarker);
                motor->triggerPointsHigh[id]->back()->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::green), QPen(Qt::green), QSize(4, 4)));
                motor->triggerPointsHigh[id]->back()->setValue(point);
            }
            motor->highState[id] = true;

        }
        else if (motor->xydata[id].y[i] <= ui->lineEdit_BottomTrigger->text().toInt() && motor->highState[id] == true)
        {
            if(     motor->triggerPointsLow[ id ]->size() == 0
                    || ( motor->triggerPointsLow[ id ]->size() > 0
                    && ( motor->triggerPointsLow[ id ]->at( motor->triggerPointsLow[ id ]->size() - 1 )->xValue() + windowWidth < motor->xydata[id].x[i] ) ) )
            {
                QPointF point(motor->xydata[id].x[i], motor->xydata[id].y[i]);

                motor->triggerPointsLow[id]->push_back(new QwtPlotMarker);
                motor->triggerPointsLow[id]->back()->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::cyan), QPen(Qt::cyan), QSize(4, 4)));
                motor->triggerPointsLow[id]->back()->setValue(point);

            }
            motor->highState[id] = false;
        }
    }
}


void CentralWidget::DrawLastSampleUpdate()
{

    if( lastUpdateCurve->isAttached() )
    {
        lastUpdateCurve->detach();
    }

    if(!ui->radioButton_sampleUpdate->isChecked())
    {
        qwtPlotPer->replot();
        return;
    }

    lastUpdateCurve->setPen( QPen( QColor(98, 3, 252), 1, Qt::SolidLine ) );
    lastUpdateCurve->setSamples( QVector<double>{currentMotor->xydata[6].x[currentMotor->currentXPos], currentMotor->xydata[6].x[currentMotor->currentXPos]},
                                          QVector<double>{ 5000, -5000 } );
    lastUpdateCurve->attach( qwtPlotPer, 1 );
    qwtPlotPer->replot();
}

void CentralWidget::p_selected_tc(int id, int cheked)
{
    if (!currentMotor) return;
    if (cheked)
    {
        currentMotor->curveCopyTc[id]->attach(qwtPlotTc, 1);

        if(currentMotor->logFile.isOpen())
            UpdateTriggerPoints(currentMotor, id);

        if( ui->pushButton_TopTrigger->isChecked() )
            for (int i = 0; i < currentMotor->triggerPointsHigh[id]->size(); i++)
                currentMotor->triggerPointsHigh[id]->at(i)->attach(qwtPlotTc);
        if( ui->pushButton_BottomTrigger->isChecked() )
            for (int i = 0; i < currentMotor->triggerPointsLow[id]->size(); i++)
                currentMotor->triggerPointsLow[id]->at(i)->attach(qwtPlotTc);

        if(currentMotor->logFile.isOpen())
            if( ( ui->comboBox_SourceSelect->currentIndex() > 0 && ui->comboBox_TriggerSelect->currentIndex() ) > 0 ){
                        UpdateCurvesToInf(id);
                    }

    }
    else
    {
        currentMotor->curveCopyTc[id]->detach();
        if( ui->pushButton_TopTrigger->isChecked() )
            for (int i = 0; i < currentMotor->triggerPointsHigh[id]->size(); i++)
                currentMotor->triggerPointsHigh[id]->at(i)->detach();
        if( ui->pushButton_BottomTrigger->isChecked() )
            for (int i = 0; i < currentMotor->triggerPointsLow[id]->size(); i++)
                currentMotor->triggerPointsLow[id]->at(i)->detach();

    }

    qwtPlotTc->replot();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void CentralWidget::init()
{
//    ui->setupUi(this);

    currentMotor=0;

    QLabel *lb = new QLabel;


    QHBoxLayout *pwrLay = new QHBoxLayout;
    QWidget *wpwr = new QWidget;
    wpwr->setLayout(pwrLay);
    pwr = new mPower(ui->tabWidget);
    pwrLay->addWidget(pwr);
    ui->tabWidget->addTab(wpwr,"Power Meter");

    ui->tabWidget;
    //ctrl = new mTab(ui->scrollArea_paramButtons);
    ctrl = new mTab();
    ui->scrollArea_paramButtons->setWidget(ctrl);
    //ui->verticalLayout_buttons->horizontalLayout_motor->addWidget(ctrl);
    connect(ctrl, SIGNAL(p_selected(int,int)),this, SLOT(p_selected(int,int)));

    connect(ctrl, &mTab::p_selected,this, [=]( int id, int checked ){ p_selected_tc(id,checked); });

    //    connect(ctrl, SIGNAL(MotorDebug(int, bool)),this, SLOT(MotorDebug(int, bool)));
    connect(ctrl, &mTab::EnableMotorDebug, this, &CentralWidget::MotorDebug);

    udpDebug = new QUdpSocket(this);
    udpDebug->bind(P_UDP_DEBUG_L, QUdpSocket::ShareAddress);
    connect(udpDebug, SIGNAL(readyRead()),this, SLOT(readPendingDatagramsDebug()));

    udpID = new QUdpSocket(this);
    udpID->bind(P_UDP_ALIVE_L, QUdpSocket::ShareAddress);
    connect(udpID, SIGNAL(readyRead()),this, SLOT(readPendingDatagramsID()));

    udpConfig = new QUdpSocket(this);
    udpConfig->bind(P_UDP_CONFIG_L, QUdpSocket::ShareAddress);
    connect(udpConfig, SIGNAL(readyRead()),this, SLOT(readPendingDatagramsConfig()));

    udpDbgNames = new QUdpSocket(this);
    udpDbgNames->bind(P_UDP_DBG_NAMES_L, QUdpSocket::ShareAddress);
    connect(udpDbgNames, SIGNAL(readyRead()),this, SLOT(readPendingDatagramsDbgName()));

    connect( ui->horizontalSlider, &QScrollBar::valueChanged, this, &CentralWidget::fileScrollValueChanged );

    x_scaled0=0;
    x_scaled1=X_SIZE * PER;

    auto arial8 = QFont("arial", 8);


//    /////////////////////////////////
    UpdateParNamesCombo();
//    /// ///////////////////////////////////


    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setHandleWidth(10);

    qwtPlotPer = new QwtPlotAdvanced();
    qwtPlotPer->setAxisFont(QwtPlot::yLeft, arial8);
    qwtPlotPer->setAxisFont(QwtPlot::xBottom, arial8);
    qwtPlotPer->grid()->enableXMin(1);
    qwtPlotPer->resetZoom( QRectF( 0.0, -5000.0, X_SIZE * PER, 10000.0 ) );
    qwtPlotPer->attachGrid();
    qwtPlotPer->setCurveScalePickerEnable(1);
    qwtPlotPer->setCurveResetPickerEnable(1);
    qwtPlotPer->setCurveDragPickerEnable(1);
    qwtPlotPer->setMinimumSize(100,100);
    splitter->addWidget(qwtPlotPer);

    qwtPlotTc = new QwtPlotAdvanced();
    qwtPlotTc->setAxisFont(QwtPlot::yLeft, arial8);
    qwtPlotTc->setAxisFont(QwtPlot::xBottom, arial8);
    qwtPlotTc->grid()->enableXMin(1);
    qwtPlotTc->resetZoom( QRectF( 0.0, -5000.0, X_SIZE * PER, 10000.0 ) );
    qwtPlotTc->attachGrid();
    qwtPlotTc->setCurveScalePickerEnable(1);
    qwtPlotTc->setCurveResetPickerEnable(1);
    qwtPlotTc->setCurveDragPickerEnable(1);
    qwtPlotTc->setMinimumSize(100,100);
    splitter->addWidget(qwtPlotTc);
    ui->tab_TriggerCapruteChart->layout()->addWidget(splitter);

    qwtPlotXy = new QwtPlotAdvanced();
    qwtPlotXy->setAxisFont(QwtPlot::yLeft, arial8);
    qwtPlotXy->setAxisFont(QwtPlot::xBottom, arial8);
    qwtPlotXy->grid()->enableXMin(1);
    qwtPlotXy->resetZoom( QRectF( -5000.0, -5000.0, 10000.0, 10000.0 ) );
    qwtPlotXy->attachGrid();
    qwtPlotXy->setCurveScalePickerEnable(1);
    qwtPlotXy->setCurveResetPickerEnable(1);
    qwtPlotXy->setCurveDragPickerEnable(1);
    ui->tab_XYChart->layout()->addWidget(qwtPlotXy);

    triggerWindowCenterCurve = new QwtPlotCurveAdvanced();
    lastUpdateCurve = new QwtPlotCurveAdvanced();

    ui->tabWidget->setMinimumSize( QSize(400, 400) );

    cXYData = XYCurveData();

    QDir dir;
    QString dirPath = QCoreApplication::applicationDirPath() + "\\log-files";
    if(!dir.exists(dirPath))
    {
        dir.mkdir(dirPath);
    }
}


void CentralWidget::on_comboBox_SourceSelect_currentIndexChanged(int index)
{
    if(!currentMotor) return;
        if(currentMotor->logFile.isOpen())
        {
            if(index > 0
                    && ui->comboBox_TriggerSelect->currentIndex() > 0
                    && currentMotor->triggerPointsHigh[index - 1]->size() > 0
                    && currentMotor->triggerPointsLow[index - 1]->size() > 0)
            {
                UpdateScrolCapture(index - 1);
            }


            RedrawCapture(index - 1);
            if(index - 1 > 0)
                UpdateCurvesToInf(index - 1);
        }
        else
        {
            OnlineRedrawCapture(index - 1);
        }
}


void CentralWidget::on_comboBox_TriggerSelect_currentIndexChanged(int index)
{
    if(!currentMotor) return;
        if(currentMotor->logFile.isOpen())
        {
            if(ui->comboBox_SourceSelect->currentIndex() > 0
                    && index > 0
                    && currentMotor->triggerPointsHigh[ui->comboBox_SourceSelect->currentIndex() - 1]->size() > 0
                    && currentMotor->triggerPointsLow[ui->comboBox_SourceSelect->currentIndex() - 1]->size() > 0)
            {
                UpdateScrolCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            }

            RedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            if(ui->comboBox_SourceSelect->currentIndex() - 1 > 0)
                UpdateCurvesToInf(index - 1);
        }
        else
        {
            if(ctrl->chk[ui->comboBox_SourceSelect->currentIndex() - 1]->isChecked())
                OnlineRedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
        }
}


void CentralWidget::on_lineEdit_FrameWidth_editingFinished()
{
    if(currentMotor)
        {
            if(currentMotor->logFile.isOpen())
            {
                for (int i = 0; i < PARAMS_IN_PACK; i++)
                    if(ctrl->IsChecked(i))
                    {
                        ReAttachTriggerPoints(i);
                        if(ui->comboBox_SourceSelect->currentIndex() - 1 == i)
                            UpdateScrolCapture(i);
                    }
                RedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            }
            else
            {
                for (int i = 0; i < PARAMS_IN_PACK; i++)
                    if(ctrl->IsChecked(i))
                    {
                        ReAttachTriggerPoints(i);
                    }
                OnlineRedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            }
        }
}


void CentralWidget::on_lineEdit_FrameHeight_editingFinished()
{
    if(currentMotor)
        {
            if(currentMotor->logFile.isOpen())
                RedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            else
                OnlineRedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
        }
}


void CentralWidget::on_verticalScrollBar_Capture_valueChanged(int value)
{
    RedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
    UpdateCurvesToInf(ui->comboBox_SourceSelect->currentIndex() - 1);
}


void CentralWidget::on_spinBox_valueChanged(int arg1)
{
    if (!currentMotor) return;
    const char v=(char)arg1;
    QUdpSocket().writeDatagram(&v,currentMotor->motorAddress.ip,P_UDP_MIN_DBG_NAMES_M+currentMotor->motorAddress.id);
}


void CentralWidget::on_toolButton_TriggerLock_clicked(bool checked)
{
    if(ui->toolButton_TriggerLock->isChecked())
    {
        ui->lineEdit_BottomTrigger->setEnabled(false);
        ui->lineEdit_BottomTrigger->setText(ui->lineEdit_TopTrigger->text());

        if(!currentMotor) return;

        on_lineEdit_BottomTrigger_editingFinished();
    }else
    {
        ui->lineEdit_BottomTrigger->setEnabled(true);
    }
}


void CentralWidget::on_pushButton_TopTrigger_clicked(bool checked)
{
    if( currentMotor )
    {
        if( checked )
        {

            for( int id = 0; id < PARAMS_IN_PACK; id++ ){
                if( ctrl->IsChecked(id) )
                {
                    for ( int i = 0; i < currentMotor->triggerPointsHigh[id]->size(); i++ )
                        currentMotor->triggerPointsHigh[id]->at(i)->attach( qwtPlotTc );
                }
            }
            qwtPlotTc->replot();
        }
        else
        {
            for( int id = 0; id < PARAMS_IN_PACK; id++ ){
                for ( int i = 0; i < currentMotor->triggerPointsHigh[id]->size(); i++ )
                    currentMotor->triggerPointsHigh[id]->at(i)->detach();
            }
            qwtPlotTc->replot();
        }
    }
}


void CentralWidget::on_pushButton_BottomTrigger_clicked(bool checked)
{
    if( currentMotor )
    {
        if( checked )
        {

            for(int id = 0; id < PARAMS_IN_PACK; id++){
                if( ctrl->IsChecked(id) )
                {
                    for ( int i = 0; i < currentMotor->triggerPointsLow[id]->size(); i++ )
                        currentMotor->triggerPointsLow[id]->at(i)->attach( qwtPlotTc );
                }
            }
            qwtPlotTc->replot();
        }
        else
        {
            for( int id = 0; id < PARAMS_IN_PACK; id++ ){
                for ( int i = 0; i < currentMotor->triggerPointsLow[id]->size(); i++ )
                    currentMotor->triggerPointsLow[id]->at(i)->detach();
            }
            qwtPlotTc->replot();
        }
    }
}


void CentralWidget::on_lineEdit_TopTrigger_editingFinished()
{
    if(ui->toolButton_TriggerLock->isChecked()){
        ui->lineEdit_BottomTrigger->setText(ui->lineEdit_TopTrigger->text());
    }
    if( currentMotor )
    {

        for( int id = 0; id < PARAMS_IN_PACK; id++ )
        {
            if(ctrl->IsChecked(id))
                ReAttachTriggerPoints(id);
        }
        if(currentMotor->logFile.isOpen())
        {
            UpdateScrolCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            RedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            UpdateCurvesToInf(ui->comboBox_SourceSelect->currentIndex() - 1);
        }
        qwtPlotTc->replot();
    }
}


void CentralWidget::on_lineEdit_BottomTrigger_editingFinished()
{
    if( currentMotor )
    {
        for( int id = 0; id < PARAMS_IN_PACK; id++ )
        {

            if(ctrl->IsChecked(id))
                ReAttachTriggerPoints(id);
        }
        if(currentMotor->logFile.isOpen())
        {
            UpdateScrolCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            RedrawCapture(ui->comboBox_SourceSelect->currentIndex() - 1);
            UpdateCurvesToInf(ui->comboBox_SourceSelect->currentIndex() - 1);
        }
        qwtPlotTc->replot();
    }
}


void CentralWidget::on_pushButton_addxy_clicked()
{
    if(!currentMotor) return;

        QFrame *paramFunc = new QFrame();
        paramFunc->setFrameShape(QFrame::StyledPanel);
        paramFunc->setFrameShadow(QFrame::Sunken);
        paramFunc->setLineWidth(1);
        paramFunc->resize(309,paramFunc->height());

        QHBoxLayout *layout = new QHBoxLayout;
        layout->setMargin(4);
        layout->setSpacing(2);

        QLabel *colorLabel = new QLabel();
        colorLabel->setFixedHeight(25);
        colorLabel->setFixedWidth(5);

        QComboBox *param1 = new QComboBox();
        param1->setObjectName("param1");
        param1->setFixedHeight(25);
        param1->setMinimumWidth(100);

        QComboBox *param2 = new QComboBox();
        param1->setObjectName("param2");
        param2->setFixedHeight(25);
        param2->setMinimumWidth(100);

        QLineEdit *stepLineEdit = new QLineEdit;
        stepLineEdit->setFixedHeight(25);
        stepLineEdit->setMinimumWidth(45);
        stepLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        stepLineEdit->setText("0.01");

        QPushButton *delButton = new QPushButton();
        delButton->setText("X");
        delButton->setFixedHeight(25);
        delButton->setFixedWidth(25);

        param1->addItem( "None" );
        param2->addItem( "None" );
        for( int i = 0; i < PARAMS_IN_PACK; i++ )
        {
            param1->addItem( ctrl->chk[i]->text() );
            param2->addItem( ctrl->chk[i]->text() );
        }

        layout->addWidget(colorLabel);

        layout->addWidget(param1);
        layout->addWidget(param2);

        //layout->addWidget(updButton);
        layout->addWidget(stepLineEdit);
        layout->addWidget(delButton);

        paramFunc->setLayout(layout);

        ui->verticalLayout_xyparams->addWidget(paramFunc);

        AddXYFunc(colorLabel, paramFunc, param1, param2, stepLineEdit ,delButton);  //connect должен быть
        qwtPlotXy->replot();
}

void  CentralWidget::AddXYFunc(QLabel *cLabel, QFrame *frame, QComboBox *px, QComboBox *py, QLineEdit *step, QPushButton *delButton)   //подумать над генерацией цветов (есть готовый класс colors)
{
    qDebug()<<px<<py;
    XYCurve *curveXY = new XYCurve(cLabel, px, py, delButton, step, qwtPlotXy, currentMotor->logFile.isOpen());
    curveXY->SetData(currentMotor->xydata);
    cXYData.AddCurve(curveXY, frame);
}


void CentralWidget::on_tabWidget_Charts_currentChanged(int index)
{
    if(motors.isEmpty()) return;
    if(index == 0)
    {
        SetFileScrollValuePer();
    }
    else
    {
        SetFileScrollValueXy();
    }
}

void CentralWidget::SetFileScrollValuePer()
{
    currentMotor->logScrollValueXy = ui->horizontalSlider->value();
    ui->horizontalSlider->setValue(currentMotor->logScrollValuePer);
}

void CentralWidget::SetFileScrollValueXy()
{
    currentMotor->logScrollValuePer = ui->horizontalSlider->value();
    ui->horizontalSlider->setValue(currentMotor->logScrollValueXy);
}



void CentralWidget::on_pushButton_clicked()
{
    int size = cXYData.Size();
    for(int i = size - 1; i >= 0; i--)
    {
        cXYData.DeleteCurve(i);
        qDebug()<<"iter "<<i<<"  size "<< cXYData.Size();;
    }
}


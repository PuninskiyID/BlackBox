//#include <qprintdialog.h>

#include "plot.h"



class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap( Qt::darkCyan, Qt::red )
    {
        addColorStop( 0.1, Qt::blue );
        addColorStop( 0.6, Qt::green );
        addColorStop( 0.95, Qt::yellow );
    }
};

Plot::Plot( QWidget *parent ):
    QwtPlot( parent )
{
    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount( 0 ); // use system specific thread count

    d_spectrogram->setColorMap( new ColorMap() );
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );



    s_data = new SpectrogramData();

    d_spectrogram->setData(s_data);
    d_spectrogram->attach( this );

    QList<double> contourLevels;
    for ( double level = 0.5; level < 10.0; level += 1.0 )
        contourLevels += level;

    d_spectrogram->setContourLevels( contourLevels );



    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );
    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yLeft );
    rightAxis->setTitle( "Torque" );
    //rightAxis->setColorBarEnabled( true );
    //rightAxis->setColorMap( zInterval, new ColorMap() );





    QFont arial("Arial", 8);
    setAxisFont(QwtPlot::yLeft, arial);
    setAxisFont(QwtPlot::xBottom, arial);
    //axisWidget( QwtPlot::xBottom)->setAxisFont(QwtPlot::xBottom, arial);


    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    enableAxis( QwtPlot::yLeft);

    plotLayout()->setAlignCanvasToScales( true );
    replot();

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

   // QwtPlotZoomer* zoomer = new MyZoomer( canvas() );
   // zoomer->setMousePattern( QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier );
   // zoomer->setMousePattern( QwtEventPattern::MouseSelect3, Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MidButton );

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "100.00" ) );

    const QColor c( Qt::darkBlue );
    //zoomer->setRubberBandPen( c );
    //zoomer->setTrackerPen( c );

    setMaximumHeight(220);
}

void Plot::showContour( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    replot();
}

void Plot::showSpectrogram( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen( on ? QPen() : QPen( Qt::NoPen ) );
    replot();
}

void Plot::setAlpha( int alpha )
{
    d_spectrogram->setAlpha( alpha );
    replot();
}

#ifndef QT_NO_PRINTER

void Plot::printPlot()
{

}

#endif

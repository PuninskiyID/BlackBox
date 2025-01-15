#ifndef QWTPLOTCOMPLETE_H
#define QWTPLOTCOMPLETE_H

#include <cmath>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <QMouseEvent>
#include <qwt_picker_machine.h>
#include <qwt_painter.h>
#include <QStack>
#include <QwtText>
#include "series_data.h"
#include "key-map.h"

//#define PLOT_ZOOMER_ADV_DBG 1

//template<typename T>
//static inline T clip(T x, T l, T h)
//{
//    return min(max(x, l), h);
//}

template<typename T> static T clip(const T &x, const T &lo, const T &hi)
{
    return x < lo? lo : (x > hi? hi : x);
}

class PlotZoomerAdv : public QwtPlotZoomer
{
    double off, sca;
public:
    PlotZoomerAdv(QWidget *w, bool doReplot = true) : QwtPlotZoomer(w, doReplot), off(0.0), sca(1.0) { }
    void setTrackerOffset(double off) { this->off = off; }
    double getTrackerOffset(void) { return off; }
    void setTrackerScale(double sca) { this->sca = sca; }
    double getTrackerScale(void) { return sca; }
    void widgetKeyPressEvent( QKeyEvent *ke ) override;
    void zoom(int offset) override
    {
#if PLOT_ZOOMER_ADV_DBG
        qDebug() << "ZOOM" << offset;
#endif /*PLOT_ZOOMER_ADV_DBG*/
        QwtPlotZoomer::zoom(offset);
    }
    QwtText trackerTextF( const QPointF &pos ) const override
    {
        QString text;

        switch ( rubberBand() )
        {
            case HLineRubberBand:
                text = QString().asprintf( "%.4f", (pos.y() + off) * sca );
                break;
            case VLineRubberBand:
                text = QString().asprintf( "%.4f", pos.x() );
                break;
            default:
                text = QString().asprintf( "%.4f, %.4f", pos.x(), (pos.y() + off) * sca );
        }
        return QwtText( text );
    }
};



class QwtPlotAdvanced : public QwtPlot
{
    Q_OBJECT
    int curveDragState, curveScaleState;
signals:
    
    void curveDeleted( QwtPlotCurveAdvanced* curve );

private slots:
    void curveDragAppended( const QPointF& pos );
    void curveDragMoved( const QPointF& pos );
    void curveScaleAppended( const QPointF& pos );
    void curveScaleMoved( const QPointF& pos );
    void curveDelete( const QPointF& pos );
    void curveReset( const QPointF& pos );
    void curveDragActivated( bool on )
    {
        if(on)
        {
            curveDragState = 1;
        }
        else
        {
            curveDragState = 0;
        }
    }
    void curveScaleActivated( bool on )
    {
        if(on)
        {
            curveScaleState = 1;
        }
        else
        {
            curveScaleState = 0;
        }
    }
    QMap<QString, QwtPlotCurveAdvanced>::Iterator findClosestCurve( QPointF p );
    
public:
    QwtPlotAdvanced();
    void setCurveDragPickerEnable( bool on )
    {
        pickerCurveDrag->setEnabled( on );
    }
    void setCurveScalePickerEnable( bool on )
    {
        pickerCurveScale->setEnabled( on );
    }
    void setCurveDeletePickerEnable( bool on )
    {
        pickerCurveDelete->setEnabled( on );
    }
    void setCurveResetPickerEnable( bool on )
    {
        pickerCurveReset->setEnabled( on );
    }
    void attachGrid(void)
    {
        _grid.attach( this );
    }
    void attachLegend(void)
    {
        legend.attach( this );
    }
    void resetZoom(const QRectF& rect)
    {
        QStack<QRectF> stack;
        stack.push(rect);
        zoomer->setZoomStack(stack, 0);
        replot();
    }
    void resetZoom(const QRectF& rect0, const QRectF& rect)
    {
        QStack<QRectF> stack;
        stack.push(rect0);
        stack.push(rect);
        zoomer->setZoomStack(stack, 1);
        replot();
    }
    void setZoomBase(const QRectF& rect)
    {
        QStack<QRectF> stack = this->zoomStack();
        stack[0] = rect;
        zoomer->setZoomStack(stack, this->zoomRectIndex());
        replot();
    }
    QRectF zoomRect(void) const
    {
        return zoomer->zoomRect();
    }
    QRectF zoomBase(void) const
    {
        return zoomer->zoomBase();
    }
    QStack<QRectF> zoomStack(void) const
    {
        return zoomer->zoomStack();
    }
    void setZoomStack(const QStack<QRectF>& stack, int zoomRectIndex = -1)
    {
        zoomer->setZoomStack(stack, zoomRectIndex);
    }
    int zoomRectIndex() const
    {
        return zoomer->zoomRectIndex();
    }
    void setTrackerOffset(double off)
    {
        zoomer->setTrackerOffset(off);
    }
    void setTrackerScale(double sca)
    {
        zoomer->setTrackerScale(sca);
    }
    void setTrackerColor(const QColor& col)
    {
        zoomer->setTrackerPen(QPen(col));
    }
    void setXAxisTitle(const QString& tit)
    {
        QwtText xTitle;
        xTitle.setText(tit);
        xTitle.setRenderFlags( Qt::AlignRight | Qt::AlignVCenter );
        xTitle.setFont(this->axisFont(QwtPlot::xBottom));
        this->setAxisTitle(QwtPlot::xBottom, xTitle);
    }
    bool containsCurve( const QString& name)
    {
        return seriesData.contains(name);
    }
    QwtPlotGrid* grid()
    {
        return &_grid;
    }
    QwtPlotCurveAdvanced* addCurve(const QString& name, bool setCurveTitle = 0)
    {
        if(!containsCurve(name))
        {
            seriesData.insert(name, QwtPlotCurveAdvanced());
        }
        if( setCurveTitle )
        {
            seriesData[name].setTitle(name);
        }
        return &seriesData[name];
    }
    QwtPlotCurveAdvanced* getCurve(const QString& name)
    {
        if(!containsCurve(name))
        {
            return 0;
        }
        return &seriesData[name];
    }
    void delCurve( const QString& name )
    {
        auto seli = seriesData.find(name);

        if(seli != seriesData.end())
        {
            if(seli->isAttached())
                seli->detach();
            seriesData.erase(seli);
            this->replot();
        }
    }
    void deleteCurves()
    {
        for( auto& curve : seriesData )
        {
            curve.detach();
        }
        seriesData.clear();
    }
    unsigned int curvesCount()
    {
        return seriesData.size(); 
    }
    void setLegendMargin( int marg )
    {
        legend.setMargin( marg );
    }
    void setLegendMaxColumns( unsigned int cnt )
    {
        legend.setMaxColumns( cnt );
    }
    void setLegendFont( QFont font )
    {
        legend.setFont( font );
    }
    void setLegentBorderRadius( double bordRad )
    {
        legend.setBorderRadius( bordRad );
    }
    void setLegendSpacing( int spacing )
    {
        legend.setSpacing( spacing );
    }
    void setLegendItemMargin( int marg )
    {
        legend.setItemMargin( marg );
    }
    ~QwtPlotAdvanced()
    {
        delete zoomer;
        delete pickerCurveDrag;
        delete pickerCurveScale;
        delete pickerCurveDelete;
        delete pickerCurveReset;
    }
private:

    QMap<QString, QwtPlotCurveAdvanced> seriesData;
    QwtPlotMarker middleMarker;
    
    QwtPlotCurveAdvanced *curveToDrag;
    QPointF curveDragLastPos;
    QwtPlotCurveAdvanced *curveToScale;
    double curveScaleOff, curveScaleSca, curveScaleY;
    
    PlotZoomerAdv *zoomer;
    QwtPlotGrid _grid;
    QwtPlotLegendItem legend;
    QwtPlotPicker *pickerCurveDrag;
    QwtPlotPicker *pickerCurveScale;
    QwtPlotPicker *pickerCurveDelete;
    QwtPlotPicker *pickerCurveReset;
};

#endif // QWTPLOTCOMPLETE_H

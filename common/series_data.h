#ifndef SERIES_DATA_H
#define SERIES_DATA_H

#include <cmath>
#include "dbc-types.h"
#include "can-trace.h"
#include <qwt_series_data.h>
#include <qwt_plot_curve.h>
#include <stdint.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <QDebug>

template< typename T > T sq( const T& x )
{
    return x * x;
}

class SeriesData : public QwtSeriesData<QPointF>
{
    const double* x, *y;
    unsigned int len;
    double off, k;
    double midy;
    QRectF _boundingRect;
public:
    SeriesData(): x(0), y(0), len(0), off(0.0), k(1.0), _boundingRect(0.0, 0.0, 0.0, 0.0) { }
    void setRawSamples(const double* x, const double* y, unsigned int len)
    {        
        this->x = x;
        this->y = y;
        this->len = len;

        midy = 0.0;
        double left = 1.0e300, right = -1.0e300, top = -1.0e300, bot = 1.0e300;
        for(unsigned int i = 0; i < len; ++i)
        {
            midy += y[i];
            if(x[i] < left) left = x[i];
            if(x[i] > right) right = x[i];
            if(y[i] < bot) bot = y[i];
            if(y[i] > top) top = y[i];
        }
        midy /= (double)len;
        _boundingRect = QRectF(left, bot, (right - left), (top - bot));
    }
    double getOffset()
    {
        return off;
    }
    double getScale()
    {
        return k;
    }
    void setOffset(double off)
    {
        this->off = off;
    }
    void setScale(double k)
    {
        this->k = k;
    }
    size_t size() const override
    {
        return len;
    }
    QPointF sample( size_t index ) const override
    {
        return QPointF(x[index], y[index] * k + off);
    }
    QRectF boundingRect() const override
    {
        return _boundingRect;
    }
};

#define QWT_PLOT_CURVE_ADV_USE_SYM 0

class QwtPlotCurveAdvanced : public QwtPlotCurve
{
    double* x_data, *y_data;
    QwtPlotMarker marker;
    bool isMarkerAttached = 0, _isAttached = 0;
#if QWT_PLOT_CURVE_ADV_USE_SYM
    QwtSymbol *sym;
#endif /*QWT_PLOT_CURVE_ADV_USE_SYM*/
public:
    QwtPlotCurveAdvanced(): QwtPlotCurve(), x_data(0), y_data(0)
    {
        this->setSamples(new SeriesData());
        marker.setValue( 0.0, 0.0 );
        marker.setLineStyle( QwtPlotMarker::HLine );
        marker.setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
        marker.setLinePen( QPen( Qt::green, 0, Qt::DashDotLine ) );
        this->setStyle(QwtPlotCurve::Lines);
#if QWT_PLOT_CURVE_ADV_USE_SYM
        sym = new QwtSymbol();
        sym->setStyle(QwtSymbol::XCross);
        sym->setSize(2, 2);
        this->setSymbol(sym);
#endif /*QWT_PLOT_CURVE_ADV_USE_SYM*/
    }
    ~QwtPlotCurveAdvanced()
    {
        if(x_data) delete[] x_data;
        if(y_data) delete[] y_data;
    }
    QwtPlotCurveAdvanced( const QwtPlotCurveAdvanced& curve ): QwtPlotCurveAdvanced()
    {
        *this = curve;
    }
    QwtPlotCurveAdvanced& operator= ( const QwtPlotCurveAdvanced& curve )
    {
        unsigned int len = ((SeriesData*)curve.data())->size();
        if(len)
        {
            x_data = new double[len];
            y_data = new double[len];
            memcpy(x_data, curve.x_data, len * sizeof(x_data[0]));
            memcpy(y_data, curve.y_data, len * sizeof(y_data[0]));
            ((SeriesData*)this->data())->setRawSamples(x_data, y_data, len);
        }
        this->setTitle(curve.title());

        return *this;
    }
    double distToClosestPoint(const QPointF& p, double xDomFrom = -INFINITY, double xDomTo = +INFINITY, double xScaleCorr = 1.0, QPointF* closestPoint = 0, bool doPrintDebug = 0) const
    {
        unsigned int len = ((SeriesData*)this->data())->size();
        if( len <= 0 ) return NAN;
        double mind = 1.0e300;
        double x, y, d;
        QPointF s, cp;
        unsigned int i = 0;

        do
        {
            s = this->data()->sample(i);
        } while( s.x() < xDomFrom && ++i < len );

        mind = sqrt( sq(s.x() - p.x()) * xScaleCorr + sq(s.y() - p.y()) );

        if( i >= len )
        {
            if(closestPoint)
            {
                *closestPoint = s;
            }
            return mind;
        }

        for( ; i < len; ++i )
        {
            s = this->data()->sample(i);
            if( s.x() > xDomTo )
            {
                return mind;
            }
            x = s.x() - p.x();
            y = s.y() - p.y();
            d = sqrt(x * x * xScaleCorr + y * y);
            if(doPrintDebug) qDebug() << "    " << "    " << s << d;
            if(d < mind)
            {
                cp = s;
                mind = d;
                if(doPrintDebug) qDebug() << "    " << "    " << "Closer" << mind;
            }
        }
        if(closestPoint) *closestPoint = cp;
        return mind;
    }
    void setRawSamples( double* x, double* y, unsigned len )
    {
        if(this->data())
            ((SeriesData*)this->data())->setRawSamples(x, y, len);
    }
    void assignSignalCurve( const Dbc::Signal& sgn, const std::deque<CanTrace::Message>& canData )
    {
        unsigned int len = canData.size();
        if(x_data) delete[] x_data;
        if(y_data) delete[] y_data;
        x_data = new double[len];
        y_data = new double[len];

        this->setTitle(QString(sgn.getName().c_str()));

        for(unsigned int i = 0; i < len; ++i)
        {
            x_data[i] = canData[i].time;
            y_data[i] = sgn.parseFromCanData(canData[i].data);
        }

        ((SeriesData*)this->data())->setRawSamples(x_data, y_data, len);
    }
    void assignPeriodCurve( const std::deque<CanTrace::Message>& canData, const QString curveName )
    {
        unsigned int len = canData.size();
        if(x_data) delete[] x_data;
        if(y_data) delete[] y_data;
        x_data = new double[len];
        y_data = new double[len];

        this->setTitle(curveName);

        x_data[0] = canData[0].time;
        y_data[0] = canData[1].time - canData[0].time;

        for(unsigned int i = 1; i < len; ++i)
        {
            x_data[i] = canData[i].time;
            y_data[i] = canData[i].time - canData[i - 1].time;
        }

        ((SeriesData*)this->data())->setRawSamples(x_data, y_data, len);
    }
    void attach( QwtPlot* plot, bool attachMarker = 0 )
    {
        if(attachMarker)
        {
            marker.attach(plot);
            isMarkerAttached = 1;
        }
        QwtPlotCurve::attach(plot);
        _isAttached = 1;
    }
    void detach()
    {
        if(isMarkerAttached)
        {
            marker.detach();
            isMarkerAttached = 0;
        }
        QwtPlotCurve::detach();
        _isAttached = 0;
    }
    bool isAttached()
    {
        return _isAttached;
    }
    void setPen( const QPen & pen )
    {
        marker.setLinePen( QPen( pen.color(), 0, Qt::DashLine ) );
#if QWT_PLOT_CURVE_ADV_USE_SYM
        sym->setColor(pen.color());
#endif /*QWT_PLOT_CURVE_ADV_USE_SYM*/
        QwtPlotCurve::setPen(pen);
    }
    double getOffset()
    {
        return ((SeriesData*)this->data())->getOffset();
    }
    void setOffset(double off)
    {
        marker.setValue(0.0, off);
        ((SeriesData*)this->data())->setOffset(off);
    }
    void setScale(double k)
    {
        ((SeriesData*)this->data())->setScale(k);
    }
    double getScale()
    {
        return ((SeriesData*)this->data())->getScale();
    }
    QRectF boundingRect(void)
    {
        return ((SeriesData*)this->data())->boundingRect();
    }
    QRectF boundingRect(double yMargin, double xMargin = 0.0)
    {
        auto rect = ((SeriesData*)this->data())->boundingRect();
        return QRectF(rect.left() - xMargin, rect.top() - yMargin, rect.width() + 2.0 * xMargin, (rect.height() + 2.0 * yMargin));
    }
};

#endif // SERIES_DATA_H

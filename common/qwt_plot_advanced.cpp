#include "qwt_plot_advanced.h"

QwtPlotAdvanced::QwtPlotAdvanced() : QwtPlot(), curveDragState(0)
{
    this->setCanvasBackground( QColor( Qt::black) );
    this->setAxisAutoScale( QwtPlot::yLeft, false );
    this->setForegroundRole( QPalette::Dark );

    curveToDrag = 0;
    curveToScale = 0;
    
    zoomer = new PlotZoomerAdv(this->canvas(), 1);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton); /*zoom on left*/
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier); /*zoom 0 on right + ctrl*/
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton); /*zoom -1 on right*/
    zoomer->setTrackerMode(QwtPicker::AlwaysOn);
    zoomer->setRubberBandPen( QPen( Qt::red, 1, Qt::DotLine ) );
    zoomer->setTrackerPen( QPen( Qt::red ) );
    resetZoom(QRectF(0.0, 0.0, 1.0, 1.0));
    
    _grid.enableXMin( true );
    _grid.setMajorPen( QPen( Qt::darkGray, 0, Qt::DotLine ) );
    _grid.setMinorPen( QPen( Qt::darkGray, 0, Qt::DotLine ) );
    
    QFont arial("Arial", 12);
    arial.setBold(true);
    QFont arial2("Monospace", 7);
    
    legend.setRenderHint( QwtPlotItem::RenderAntialiased );
    legend.setTextPen( QColor(Qt::white) );
    legend.setFont(arial);
    legend.setBorderPen( QColor(Qt::white) );
    legend.setBackgroundBrush( QColor(qRgb(100, 100, 100)) );
#if QWT_VERSION == 0x010605
    legend.setAlignment( Qt::Alignment(Qt::AlignRight | Qt::AlignTop) );
#elif QWT_VERSION == 0x060200
    legend.setAlignmentInCanvas(Qt::Alignment(Qt::AlignRight | Qt::AlignTop));

#endif
    legend.setMaxColumns( 1 );
    legend.setBorderRadius( 4 );
    legend.setMargin( 4 );
    legend.setSpacing( 5 );
    legend.setItemMargin( 0 );
    legend.setFont(arial2);

    middleMarker.setValue( 0.0, 0.0 );
    middleMarker.setLineStyle( QwtPlotMarker::HLine );
    middleMarker.setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    middleMarker.setLinePen( QPen( Qt::green, 0, Qt::DashDotDotLine ) );
    
    pickerCurveDrag = new QwtPlotPicker(this->canvas());
    pickerCurveDrag->setStateMachine(new QwtPickerDragPointMachine);
    pickerCurveDrag->setMousePattern( QwtEventPattern::MouseSelect1,  Qt::LeftButton, Qt::ShiftModifier);
    pickerCurveDrag->setKeyPattern( QwtEventPattern::KeySelect1, Qt::NoButton);
    pickerCurveDrag->setRubberBand(QwtPicker::NoRubberBand);

    #if QWT_VERSION == 0x010605
    connect( pickerCurveDrag, &QwtPlotPicker::appended, this, &QwtPlotAdvanced::curveDragAppended );
    connect( pickerCurveDrag, &QwtPlotPicker::moved, this, &QwtPlotAdvanced::curveDragMoved );
    connect( pickerCurveDrag, &QwtPlotPicker::activated, this, &QwtPlotAdvanced::curveDragActivated );
    #elif QWT_VERSION == 0x060200
    connect( pickerCurveDrag, SIGNAL(appended(QPointF)), this, SLOT(curveDragAppended(QPointF)) );
    connect( pickerCurveDrag, SIGNAL(moved(QPointF)), this, SLOT(curveDragMoved(QPointF)) );
    connect( pickerCurveDrag, SIGNAL(activated(bool)), this, SLOT(curveDragActivated(bool)) );
    #endif
    pickerCurveDrag->setEnabled(0);
    
    pickerCurveScale = new QwtPlotPicker(this->canvas());
    pickerCurveScale->setStateMachine(new QwtPickerDragPointMachine);
    pickerCurveScale->setMousePattern( QwtEventPattern::MouseSelect1,  Qt::LeftButton, Qt::ControlModifier);
    pickerCurveScale->setKeyPattern( QwtEventPattern::KeySelect1, Qt::NoButton);
    pickerCurveScale->setRubberBand(QwtPicker::NoRubberBand);
    #if QWT_VERSION == 0x010605
    connect( pickerCurveScale, &QwtPlotPicker::appended, this, &QwtPlotAdvanced::curveScaleAppended );
    connect( pickerCurveScale, &QwtPlotPicker::moved, this, &QwtPlotAdvanced::curveScaleMoved );
    connect( pickerCurveScale, &QwtPlotPicker::activated, this, &QwtPlotAdvanced::curveScaleActivated );
    #elif QWT_VERSION == 0x060200
    connect( pickerCurveScale, SIGNAL(appended(QPointF)), this, SLOT(curveScaleAppended(QPointF)) );
    connect( pickerCurveScale, SIGNAL(moved(QPointF)), this, SLOT(curveScaleMoved(QPointF)) );
    connect( pickerCurveScale, SIGNAL(activated(bool)), this, SLOT(curveScaleActivated(bool)) );
    #endif
    pickerCurveScale->setEnabled(0);

    pickerCurveDelete = new QwtPlotPicker(this->canvas());
    pickerCurveDelete->setStateMachine(new QwtPickerClickPointMachine);
    pickerCurveDelete->setMousePattern( QwtEventPattern::MouseSelect1,  Qt::NoButton);
    pickerCurveDelete->setKeyPattern( QwtEventPattern::KeySelect1,  Qt::Key_D, Qt::ControlModifier );
    pickerCurveDelete->setRubberBand(QwtPicker::NoRubberBand);
    #if QWT_VERSION == 0x010605
    connect( pickerCurveDelete, &QwtPlotPicker::appended, this, &QwtPlotAdvanced::curveDelete );
    #elif QWT_VERSION == 0x060200
    connect( pickerCurveDelete, SIGNAL(appended(QPointF)), this, SLOT(curveDelete(QPointF)) );
    #endif
    pickerCurveDelete->setEnabled(0);

    pickerCurveReset = new QwtPlotPicker(this->canvas());
    pickerCurveReset->setStateMachine(new QwtPickerClickPointMachine);
    pickerCurveReset->setMousePattern( QwtEventPattern::MouseSelect1,  Qt::NoButton);
    pickerCurveReset->setKeyPattern( QwtEventPattern::KeySelect1,  Qt::Key_R, Qt::ControlModifier );
    pickerCurveReset->setRubberBand(QwtPicker::NoRubberBand);
    #if QWT_VERSION == 0x010605
    connect( pickerCurveReset, &QwtPlotPicker::appended, this, &QwtPlotAdvanced::curveReset );
    #elif QWT_VERSION == 0x060200
    connect( pickerCurveScale, SIGNAL(appended(QPointF)), this, SLOT(curveReset(QPointF)) );
    #endif
    pickerCurveReset->setEnabled(0);
}

QMap<QString, QwtPlotCurveAdvanced>::Iterator QwtPlotAdvanced::findClosestCurve( QPointF p )
{
    auto i = seriesData.begin();
    double d;
    for( ; !i->isAttached() && i != seriesData.end() ; ++i) { }
    if( !i->isAttached() || i == seriesData.end() )
    {
        return i;
    }

    auto rect = this->zoomRect();
    double xDomFrom = rect.left(), xDomTo = rect.right(), xScaleCorr = sq(rect.height() / rect.width());

    double mind = i->distToClosestPoint( p, xDomFrom, xDomTo, xScaleCorr );
    auto closestCurve = i;

    for( ++i; i != seriesData.end() ; ++i )
    {
        if(!i->isAttached())
        {
            continue;
        }
        d = i->distToClosestPoint( p, xDomFrom, xDomTo, xScaleCorr );
        if(d < mind)
        {
            mind = d;
            closestCurve = i;
        }
    }

    return closestCurve;
}

void QwtPlotAdvanced::curveDragAppended( const QPointF& pos )
{
    qDebug()<<"appendeed";
    if(curveDragState != 1) return;
    curveDragState = 2;
    
    if( this->seriesData.size() == 0 )
    {
        return;
    }

    curveDragLastPos = pos;

    auto curveIterator = findClosestCurve( pos );
    if(curveIterator == seriesData.end())
    {
        return;
    }
    curveToDrag = &*curveIterator;

    this->setTrackerColor(curveToDrag->pen().color());
    this->setTrackerOffset(-curveToDrag->getOffset());
    this->setTrackerScale( 1.0 / curveToDrag->getScale() );
}

void QwtPlotAdvanced::curveDragMoved( const QPointF& pos )
{
    if(curveDragState != 2 || !curveToDrag || !curveToDrag->data()) return;
    curveToDrag->setOffset(curveToDrag->getOffset() + pos.y() - curveDragLastPos.y());
    curveDragLastPos = pos;
    this->setTrackerOffset(-curveToDrag->getOffset());
    this->replot();
}

void QwtPlotAdvanced::curveScaleAppended( const QPointF& pos )
{
    if(curveScaleState != 1) return;
    curveScaleState = 2;
    
    if( this->seriesData.size() == 0 )
    {
        return;
    }

    auto curveIterator = findClosestCurve( pos );
    if(curveIterator == seriesData.end())
    {
        return;
    }
    curveToScale = &*curveIterator;

    curveScaleY = pos.y();
    curveScaleOff = curveToScale->getOffset();
    curveScaleSca = curveToScale->getScale();

    this->setTrackerColor(curveToScale->pen().color());
    this->setTrackerOffset(-curveToScale->getOffset());
    this->setTrackerScale(1.0 / curveToScale->getScale());
}

void QwtPlotAdvanced::curveScaleMoved( const QPointF& pos )
{
    if(curveScaleState != 2 || !curveToScale || !curveToScale->data()) return;
    
    double y, k;
    y = (curveScaleY - curveScaleOff) / curveScaleSca;
    if(y == 0.0) return;
    k = (pos.y() - curveScaleOff) / y;
    if(k < 0.0001)
    {
        k = 0.0001;
    }
    curveToScale->setScale(k);
    this->setTrackerScale(1.0 / curveToScale->getScale());
    this->replot();
}

void QwtPlotAdvanced::curveDelete( const QPointF& pos )
{
    if( this->seriesData.size() == 0 )
    {
        return;
    }

    auto curveIterator = findClosestCurve( pos );
    if(curveIterator == seriesData.end())
    {
        return;
    }
    auto seli = curveIterator;
    
    emit curveDeleted(&*seli);
    
    seli->detach();
    seriesData.erase(seli);
    this->replot();
}

void QwtPlotAdvanced::curveReset( const QPointF& pos )
{
    if( this->seriesData.size() == 0 )
    {
        return;
    }

    auto curveIterator = findClosestCurve( pos );
    if(curveIterator == seriesData.end())
    {
        return;
    }
    auto seli = curveIterator;

    seli->setOffset( 0.0 );
    seli->setScale( 1.0 );
    this->replot();
}

void PlotZoomerAdv::widgetKeyPressEvent( QKeyEvent *ke )
{
#if PLOT_ZOOMER_ADV_DBG
    qDebug() << "KEY" << ke->key() << Qt::KeyString(ke->key()) << ke->modifiers();
#endif /*PLOT_ZOOMER_ADV_DBG*/

    if( (ke->key() == Qt::Key_Plus || ke->key() == Qt::Key_Minus) &&
            ( (int)ke->modifiers() == Qt::ShiftModifier ||
              (int)ke->modifiers() == Qt::KeypadModifier ) )
    {
        ke->setModifiers( (Qt::KeyboardModifiers)((int)ke->modifiers() & ~( Qt::KeypadModifier | Qt::ShiftModifier )) );
    }

    switch(ke->key())
    {
    case Qt::Key_Plus:
    {
        if(!(ke->modifiers() & Qt::ControlModifier))
        {
            break;
        }

        auto zri = this->zoomRectIndex();
        QStack<QRectF> st;
        st += QVector<QRectF>{ this->zoomStack().begin(), this->zoomStack().begin() + zri + 1 };
        auto zr = st[zri];
        double k = 0.5;
        st.push(QRectF(zr.left() + zr.width() * k * 0.5, zr.top() + zr.height() * k * 0.5, zr.width() * k, zr.height() * k));
        this->setZoomStack(st);

        break;
    }
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
    {
        if(!(ke->modifiers() & Qt::ControlModifier))
        {
            break;
        }

        bool isH, isUp;
        switch(ke->key())
        {
        case Qt::Key_Up:
            isH = 1; isUp = 1; break;
        case Qt::Key_Down:
            isH = 1; isUp = 0; break;
        case Qt::Key_Right:
            isH = 0; isUp = 1; break;
        case Qt::Key_Left:
            isH = 0; isUp = 0; break;
        }

        bool isAlt = ke->modifiers() & Qt::AltModifier;
        auto zb = this->zoomBase();
        auto zri = this->zoomRectIndex();
        QStack<QRectF> st;
        st += QVector<QRectF>{ this->zoomStack().begin(), this->zoomStack().begin() + zri + 1 };
        auto zr = st[zri];
        QPointF foc;
        double w = zr.width();
        double h = zr.height();
//            st.push(zb);
        double step;
        if(isH)
        {
            step = h * (isAlt? 0.01 : 0.1) * (isUp? 1.0 : -1.0);
            if(isUp && (zr.bottom() + step) > zb.bottom())
            {
                step = zb.bottom() - zr.bottom();
            }
            else if(!isUp && (zr.top() + step) < zb.top())
            {
                step = zb.top() - zr.top();
            }
            zr = QRectF(zr.left(), zr.top() + step, zr.width(), zr.height());
        }
        else
        {
            step = w * (isAlt? 0.01 : 0.1) * (isUp? 1.0 : -1.0);
            if(isUp && (zr.right() + step) > zb.right())
            {
                step = zb.right() - zr.right();
            }
            else if(!isUp && (zr.left() + step) < zb.left())
            {
                step = zb.left() - zr.left();
            }
            zr = QRectF(zr.left() + step, zr.top(), zr.width(), zr.height());
        }
        foc = { zr.left() + zr.width() * 0.5, zr.top() + zr.height() * 0.5 };
        for(unsigned int i = 1; i < zri; ++i)
        {
            auto r = st[i];
            st[i] = QRectF(foc.x() - r.width() * 0.5, foc.y() - r.height() * 0.5, r.width(), r.height()) & zb;
        }
        st.back() = zr;
//            st.push(zr);
        this->setZoomStack(st, zri);
        break;
    }
    default:
        break;
    }

    QwtPlotZoomer::widgetKeyPressEvent( ke );
}

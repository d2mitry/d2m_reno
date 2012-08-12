
#include "plot.h"

#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qevent.h>

class CurveData: public QwtArraySeriesData<QPointF>
{
public:
    CurveData()
    {
    }

    virtual QRectF boundingRect() const
    {
        if ( d_boundingRect.width() < 0.0 )
            d_boundingRect = qwtBoundingRect( *this );

        return d_boundingRect;
    }

    inline void append( const QPointF &point )
    {
        d_samples += point;
    }

    void clear()
    {
        d_samples.clear();
        d_samples.squeeze();
        d_boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    }

    void clearStaleVal(double limit)
    {
        const QVector<QPointF> values = d_samples;
        d_samples.clear();

        int index;
        for ( index = values.size() - 1; index >= 0; index-- )
        {
            if ( values[index].x() < limit )
                break;
        }

        if ( index > 0 )
            d_samples += (values[index++]);

        while (index < values.size() - 1)
            d_samples += (values[index++]);

        d_boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    }

    /*QByteArray printData()
    {
        const QVector<QPointF> values = d_samples;
        int index;
        for ( index = values.size() - 1; index >= 0; index-- )
        {
            QByteArray data[index] = values[index];
            //textEdit->insertPlainText(QString(values[index]));
        }
    }*/
};

Plot::Plot(QWidget *parent):
    QwtPlot(parent),
    d_paintedPoints0(0),
    d_paintedPoints1(0),
    d_interval(0.0, 25.0),
    d_timerId(-1)
{
    d_directPainter0 = new QwtPlotDirectPainter();

    setAutoReplot(false);

    canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, false);


#if defined(Q_WS_X11)
    
    canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);

    if ( canvas()->testPaintAttribute( QwtPlotCanvas::BackingStore ) )
    {
        canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
        canvas()->setAttribute(Qt::WA_NoSystemBackground, true);
    }

#endif

    initGradient();

    plotLayout()->setAlignCanvasToScales(true);

    setAxisTitle(QwtPlot::xBottom, "Time [s]");
    setAxisScale(QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue()); 
    setAxisScale(QwtPlot::yLeft, 0, 10.0);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(QPen(Qt::green, 0.0, Qt::DotLine));
    grid->enableX(true);
    grid->enableXMin(true);
    grid->enableY(true);
    grid->enableYMin(false);
    grid->attach(this);

    d_origin = new QwtPlotMarker();
    d_origin->setLineStyle(QwtPlotMarker::Cross);
    d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);
    d_origin->setLinePen(QPen(Qt::red, 0.0, Qt::DashLine));
    d_origin->attach(this);

    d_curve0 = new QwtPlotCurve();
    d_curve0->setStyle(QwtPlotCurve::Lines);
    d_curve0->setPen(QPen(Qt::green,3));
#if 1
    d_curve0->setRenderHint(QwtPlotItem::RenderAntialiased, true);
#endif
#if 1
    d_curve0->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
#endif
    d_curve0->setData(new CurveData());
    d_curve0->attach(this);

    d_curve1 = new QwtPlotCurve();
    d_curve1->setStyle(QwtPlotCurve::Lines);
    d_curve1->setPen(QPen(Qt::red,3));
#if 1
    d_curve1->setRenderHint(QwtPlotItem::RenderAntialiased, true);
#endif
#if 1
    d_curve1->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
#endif
    d_curve1->setData(new CurveData());
    d_curve1->attach(this);
}

Plot::~Plot()
{
    delete d_directPainter0;
}

void Plot::SetTmpfile(const QString &f)
{
   tmpfile = f;
   file.setFileName(tmpfile);
}
QString Plot::GetTmpfile()
{
   return tmpfile;
}

void Plot::initGradient()
{
    QPalette pal = canvas()->palette();

#if QT_VERSION >= 0x040400
    QLinearGradient gradient( 0.0, 0.0, 1.0, 0.0 );
    gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
    gradient.setColorAt(0.0, QColor( 0, 49, 110 ) );
    gradient.setColorAt(1.0, QColor( 0, 87, 174 ) );

    pal.setBrush(QPalette::Window, QBrush(gradient));
#else
    pal.setBrush(QPalette::Window, QBrush( color ));
#endif

    canvas()->setPalette(pal);
}

void Plot::start()
{
    d_clock.start();
    d_timerId = startTimer(10);
}

void Plot::replot()
{
    CurveData *data0 = static_cast<CurveData *>( d_curve0->data());
    CurveData *data1 = static_cast<CurveData *>( d_curve1->data());
    QwtPlot::replot();
    d_paintedPoints0 = data0->size();
    d_paintedPoints1 = data1->size();
}

void Plot::setIntervalLength(double interval)
{
    if ( interval > 0.0 && interval != d_interval.width() )
    {
        d_interval.setMaxValue(d_interval.minValue() + interval);
        setAxisScale(QwtPlot::xBottom, 
            d_interval.minValue(), d_interval.maxValue());

        replot();
    }
}

void Plot::updateCurve()
{
    CurveData *data0 = (CurveData *)d_curve0->data();
    CurveData *data1 = (CurveData *)d_curve1->data();
    const int numPoints0 = data0->size();
    const int numPoints1 = data1->size();

    if ((numPoints0 > d_paintedPoints0)||(numPoints1 > d_paintedPoints1))
    {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        if ( doClip )
        {
            /*
                Depending on the platform setting a clip might be an important
                performance issue. F.e. for Qt Embedded this reduces the
                part of the backing store that has to be copied out - maybe
                to an unaccelerated frame buffer device.
            */

            const QwtScaleMap xMap0 = canvasMap( d_curve0->xAxis() );
            const QwtScaleMap yMap0 = canvasMap( d_curve0->yAxis() );

            const QwtScaleMap xMap1 = canvasMap( d_curve1->xAxis() );
            const QwtScaleMap yMap1 = canvasMap( d_curve1->yAxis() );

            QRectF br0 = qwtBoundingRect( *data0,
                d_paintedPoints0 - 1, numPoints0 - 1 );

            QRectF br1 = qwtBoundingRect( *data1,
                d_paintedPoints1 - 1, numPoints1 - 1 );

            const QRect clipRect0 = QwtScaleMap::transform( xMap0, yMap0, br0 ).toRect();
            const QRect clipRect1 = QwtScaleMap::transform( xMap1, yMap1, br1 ).toRect();
            const QRect clipRect = clipRect0.united(clipRect1);
            d_directPainter0->setClipRegion(clipRect);
            //d_directPainter1->setClipRegion(clipRect1);
        }

        d_directPainter0->drawSeries(d_curve0,
            d_paintedPoints0 - 1, numPoints0 - 1);
        d_paintedPoints0 = numPoints0;

        d_directPainter0->drawSeries(d_curve1,
            d_paintedPoints1 - 1, numPoints1 - 1);
        d_paintedPoints1 = numPoints1;
    }
}

void Plot::RecToFile0(double elapsed,double u)
{

    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << elapsed << "\t" << (u*5/1024) << "\t";
        file.close();
    }
    else
    {
        qWarning("Can not open file %s");
    }

}

void Plot::RecToFile1(double elapsed,double u)
{

    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << elapsed << "\t" << (u*5/1024) << "\n";
        file.close();
    }
    else
    {
        qWarning("Can not open file %s");
    }

}

void Plot::appendPoint0(double u)
{
    double elapsed = d_clock.elapsed() / 1000.0;
    qDebug() << "d_clock0: " << elapsed;
    qDebug() << "udouble0: " << u;
    QPointF point(elapsed,(u*5/1024));
    CurveData *data = static_cast<CurveData *>( d_curve0->data() );
    data->append(point);
    RecToFile0(elapsed,u);

    const int numPoints = data->size();
    if ( numPoints > d_paintedPoints0 )
    {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        if ( doClip )
        {
            /*
                Depending on the platform setting a clip might be an important
                performance issue. F.e. for Qt Embedded this reduces the
                part of the backing store that has to be copied out - maybe
                to an unaccelerated frame buffer device.
            */

            const QwtScaleMap xMap = canvasMap( d_curve0->xAxis() );
            const QwtScaleMap yMap = canvasMap( d_curve0->yAxis() );

            QRectF br = qwtBoundingRect( *data,
                d_paintedPoints0 - 1, numPoints - 1 );

            const QRect clipRect = QwtScaleMap::transform( xMap, yMap, br ).toRect();
            d_directPainter0->setClipRegion( clipRect );
        }

        d_directPainter0->drawSeries(d_curve0,
            d_paintedPoints0 - 1, numPoints - 1);
        d_paintedPoints0 = numPoints;
    }
}

void Plot::appendPoint1(double u)
{
    double elapsed = d_clock.elapsed() / 1000.0;
    qDebug() << "d_clock1: " << elapsed;
    qDebug() << "udouble1: " << u;
    QPointF point(elapsed,(u*5/1024));
    CurveData *data = static_cast<CurveData *>( d_curve1->data() );
    data->append(point);
    RecToFile1(elapsed,u);

    const int numPoints = data->size();
    if ( numPoints > d_paintedPoints1 )
    {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        if ( doClip )
        {
            /*
                Depending on the platform setting a clip might be an important
                performance issue. F.e. for Qt Embedded this reduces the
                part of the backing store that has to be copied out - maybe
                to an unaccelerated frame buffer device.
            */

            const QwtScaleMap xMap = canvasMap( d_curve1->xAxis() );
            const QwtScaleMap yMap = canvasMap( d_curve1->yAxis() );

            QRectF br = qwtBoundingRect( *data,
                d_paintedPoints1 - 1, numPoints - 1 );

            const QRect clipRect = QwtScaleMap::transform( xMap, yMap, br ).toRect();
            d_directPainter0->setClipRegion( clipRect );
        }

        d_directPainter0->drawSeries(d_curve1,
            d_paintedPoints1 - 1, numPoints - 1);
        d_paintedPoints1 = numPoints;
    }
}


void Plot::incrementInterval()
{
    d_interval = QwtInterval(d_interval.maxValue(),
        d_interval.maxValue() + d_interval.width());

    CurveData *data0 = static_cast<CurveData *>( d_curve0->data() );
    CurveData *data1 = static_cast<CurveData *>( d_curve1->data() );
    data0->clearStaleVal(d_interval.minValue());
    data1->clearStaleVal(d_interval.minValue());

    QwtScaleDiv scaleDiv = *axisScaleDiv(QwtPlot::xBottom);
    scaleDiv.setInterval(d_interval);

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        QList<double> ticks = scaleDiv.ticks(i);
        for ( int j = 0; j < ticks.size(); j++ )
            ticks[j] += d_interval.width();
        scaleDiv.setTicks(i, ticks);
    }
    setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);

    d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);

    d_paintedPoints0 = 0;
    d_paintedPoints1 = 0;
    replot();
}

void Plot::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == d_timerId )
    {
        updateCurve();

        const double elapsed = d_clock.elapsed() / 1000.0;
        if ( elapsed > d_interval.maxValue() )
            incrementInterval();

        return;
    }

    QwtPlot::timerEvent(event);
}

void Plot::resizeEvent(QResizeEvent *event)
{
    d_directPainter0->reset();
    QwtPlot::resizeEvent(event);
}

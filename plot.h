#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>
#include <QFile>


class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot(QWidget * = NULL);
    virtual ~Plot();

    void start();
    virtual void replot();
    virtual void timerEvent(QTimerEvent *);


    QString GetTmpfile();

public Q_SLOTS:
    void SetTmpfile(const QString &f);
    void setIntervalLength(double);
    void appendPoint0(double u);
    void appendPoint1(double u);

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    void initGradient();
    void updateCurve();

    void RecToFile0(double elapsed,double u);
    void RecToFile1(double elapsed,double u);

    void incrementInterval();

    QwtPlotMarker *d_origin;
    QwtPlotCurve *d_curve0;
    QwtPlotCurve *d_curve1;
    int d_paintedPoints0;
    int d_paintedPoints1;

    QwtPlotDirectPainter *d_directPainter0;
    //QwtPlotDirectPainter *d_directPainter1;

    QwtInterval d_interval;
    int d_timerId;

    QwtSystemClock d_clock;

    QString tmpfile;
    QFile file;

};

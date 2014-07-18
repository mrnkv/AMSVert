#ifndef RESULTDRAWWIDGET_H
#define RESULTDRAWWIDGET_H

#include <QWidget>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>



class ResultDrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ResultDrawWidget(QWidget *parent = 0);
    void setData(QVector<QPointF> data);
signals:

public slots:

protected:
private:
    QwtPlot *mastPlot;
    QwtPlotCurve *mastCurve;
    QwtPlotGrid *grid;
};

#endif // RESULTDRAWWIDGET_H

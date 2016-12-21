#ifndef RESULTDRAWWIDGET_H
#define RESULTDRAWWIDGET_H

#include <QWidget>
#include <QVector>

#include "amsmodel.h"

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_symbol.h>



class ResultDrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ResultDrawWidget(QWidget *parent = 0);
    void setData(QVector<QPointF> data, AMSModel::AMS_type type);
signals:

public slots:

protected:

private:
    QwtPlot *mastPlot;
    QwtPlotCurve *mastCurve;
    QwtPlotCurve *limitCurveRight, *limitCurveLeft;
    QwtPlotGrid *grid;
    QwtSymbol *symbol;
    QVector<QwtPlotMarker *> *markers;
};

#endif // RESULTDRAWWIDGET_H

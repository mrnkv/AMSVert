#include <QPainter>
#include <QTransform>
#include <QDebug>
#include <QtAlgorithms>
#include <QVBoxLayout>
#include "resultdrawwidget.h"

ResultDrawWidget::ResultDrawWidget(QWidget *parent) :
    QWidget(parent)
{
    mastPlot = new QwtPlot(this);
    mastPlot->setTitle("Mast dislocations");
    QPen blue_pen = QPen(Qt::blue, 3);
    QPen green_pen = QPen(Qt::green, 1);

    mastCurve = new QwtPlotCurve();
    mastCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    mastCurve->setPen(blue_pen);
    mastCurve->attach(mastPlot);

    limitCurveLeft = new QwtPlotCurve();
    limitCurveLeft->setPen(green_pen);
    limitCurveLeft->attach(mastPlot);
    limitCurveRight = new QwtPlotCurve();
    limitCurveRight->setPen(green_pen);
    limitCurveRight->attach(mastPlot);


    grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Qt::gray));
    grid->attach(mastPlot);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mastPlot);
    this->setLayout(layout);
}

void
ResultDrawWidget::setData(QVector<QPointF> data, AMSModel::AMS_type type){
    /*
    for(auto i = data.begin(); i != data.end(); i++){
        qDebug() << "Hi" << *i;
    }
    */
    float scale = (type == AMSModel::MAST ? 1000 : 1500);
    float height = data[data.size() - 1].ry();
    float width = height/scale;
    double xl[2] = {0, -width};
    double xr[2] = {0, width};
    double y[2] = {0, height};
    limitCurveLeft->setSamples(&xl[0], &y[0], 2);
    limitCurveRight->setSamples(&xr[0], &y[0], 2);

    mastCurve->setSamples(data);
    mastPlot->replot();
}


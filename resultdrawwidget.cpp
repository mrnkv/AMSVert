#include <QPainter>
#include <QTransform>
#include <QDebug>
#include <QtAlgorithms>
#include <QVBoxLayout>

#include <qwt/qwt_symbol.h>

#include "resultdrawwidget.h"


ResultDrawWidget::ResultDrawWidget(QWidget *parent) :
    QWidget(parent)

{
    markers = new QVector<QwtPlotMarker*>();
    mastPlot = new QwtPlot(this);
    mastPlot->setTitle("Mast dislocations");
    QPen blue_pen = QPen(Qt::blue, 3);
    QPen green_pen = QPen(Qt::green, 1);

    mastCurve = new QwtPlotCurve();
    mastCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    mastCurve->setPen(blue_pen);

    symbol = new QwtSymbol(QwtSymbol::Rect);
    symbol->setSize(QSize(5,5));

    mastCurve->setSymbol(symbol);
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
    for(auto m = markers->begin(); m != markers->end(); ++m){
        delete(*m);
    }

    markers->clear();

    for(auto p : data){
        QwtPlotMarker *m = new QwtPlotMarker();
        m->attach(this->mastPlot);
        m->setValue(p.rx() + 30, p.ry());
        m->setLabel(QString("%1").arg(p.rx()));
        markers->push_back(m);
    }

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


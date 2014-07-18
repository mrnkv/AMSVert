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
    QPen pen = QPen(Qt::blue, 3);

    mastCurve = new QwtPlotCurve();
    mastCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    mastCurve->setPen(pen);
    mastCurve->attach(mastPlot);

    grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Qt::gray));
    grid->attach(mastPlot);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mastPlot);
    this->setLayout(layout);
}

void
ResultDrawWidget::setData(QVector<QPointF> data){
    for(auto i = data.begin(); i != data.end(); i++){
        qDebug() << "Hi" << *i;
    }
    mastCurve->setSamples(data);
    mastPlot->replot();
}


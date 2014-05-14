#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDomDocument>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTextStream>

#include "amsmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void exitProg();
    void newFile();
    void createFromSDR();
    void setupView();
private:
    Ui::MainWindow *ui;
    AMSModel *modelX, *modelY;
    AMSModel::AMS_type amsType;
    bool readDataFromXML(QDomDocument);
    QString fileName;
    QGraphicsScene *sceneX, *sceneY, *sceneZ;
    QVector<QGraphicsRectItem*> dotsX, dotsY, dotsZ;
    QVector<QGraphicsLineItem*> linesX, linesY, linesZ;
    QVector<MesPoint> readPoints(QTextStream&);
    void pointsToModel(QVector<MesPoint>, AMSModel*);

};

#endif // MAINWINDOW_H

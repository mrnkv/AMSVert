#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newamsdialog.h"
#include <QTableWidget>
#include <QDebug>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

const QString XML_VERSION("1.0");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileName(),
    dotsX(0),
    dotsY(0),
    dotsZ(0)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Ось X");
    ui->tabWidget->setTabText(1, "Ось Y");
    ui->tabWidget->setTabText(2, "Диаграммы");
    ui->tabWidget->setVisible(false);
    //setup views
    sceneX = new QGraphicsScene(this);
    sceneY = new QGraphicsScene(this);
    sceneZ = new QGraphicsScene(this);
    ui->graphicsViewX->setScene(sceneX);
    ui->graphicsViewY->setScene(sceneY);
    ui->graphicsViewZ->setScene(sceneZ);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::openFile(){
    qDebug() << "Opening and reading file...";
    QDomDocument doc;
    fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "",
                                                    tr("Файлы XML (*.xml)"));
    if(fileName != ""){
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly)){
            QString *error = new QString();
            int strNum;
            if(doc.setContent(&file, true, error, &strNum)){
                if(readDataFromXML(doc)){
                    ui->tabWidget->setVisible(true);
                    ui->levels_X->setModel(modelX);
                    ui->levels_Y->setModel(modelY);
                }
            }
            else{
                qDebug() << *error << strNum;
            }
            file.close();
        }
    }
}

bool
MainWindow::readDataFromXML(QDomDocument doc){
    QDomElement elem = doc.documentElement();
    if(elem.tagName() != "ams"){
        qDebug() << tr("Этот файл не содержит результатов измерений АМС");
        return false;
    }
    if(elem.attribute("version") != "1.0"){
        qDebug() << tr("Несовместимая версия файла результатов измерений");
        return false;
    }
    qDebug() << elem.attribute("type");
    AMSModel::AMS_type type;
    if (elem.attribute("type") == "MAST"){
        type = AMSModel::MAST;
    }else if(elem.attribute("type") == "TOWER"){
        type = AMSModel::TOWER;
    }else{
        qDebug() << tr("Неверный тип АМС");
        return false;
    }
    this->modelX = new AMSModel(this);
    this->modelY = new AMSModel(this);
    modelX->setType(type);
    modelY->setType(type);
    size_t count = elem.childNodes().count();
    for(size_t i = 0; i < count; i++){
        QDomNode node = elem.childNodes().at(i);
        if(node.toElement().tagName() == "axis"){
            if(node.toElement().attribute("axisname") == "X"){
                float dist = QString(node.toElement().attribute("distance")).toFloat();
                modelX->setDistance(dist);
                size_t c = node.childNodes().count();
                for(size_t j = 0; j < c; j++){
                    QDomElement ln = node.childNodes().at(j).toElement();
                    Level l(QString(ln.attribute("height")).toFloat(),
                            QString(ln.attribute("leftKL")).toFloat(),
                            QString(ln.attribute("leftKR")).toFloat(),
                            QString(ln.attribute("rightKL")).toFloat(),
                            QString(ln.attribute("rightKR")).toFloat());
                    modelX->addLevel(l);
                }
            }
            if(node.toElement().attribute("axisname") == "Y"){
                float dist = QString(node.toElement().attribute("distance")).toFloat();
                modelY->setDistance(dist);
                size_t c = node.childNodes().count();
                for(size_t j = 0; j < c; j++){
                    QDomElement ln = node.childNodes().at(j).toElement();
                    Level l(QString(ln.attribute("height")).toFloat(),
                            QString(ln.attribute("leftKL")).toFloat(),
                            QString(ln.attribute("leftKR")).toFloat(),
                            QString(ln.attribute("rightKL")).toFloat(),
                            QString(ln.attribute("rightKR")).toFloat());
                    modelY->addLevel(l);
                }
            }
        }
    }
    return true;
}

bool
MainWindow::saveFile(){
    qDebug() << "Saving file" ;
    if(fileName.isEmpty())
        return saveFileAs();

    QDomDocument doc("AMS");
    QDomElement root = doc.createElement("ams");
    root.setAttribute("version", XML_VERSION);
    if(modelX->getType() == AMSModel::TOWER){
        root.setAttribute("type", "TOWER");
    }else if(modelX->getType() == AMSModel::MAST){
        root.setAttribute("type", "MAST");
    }
    QDomElement axisX = doc.createElement("axis");
    axisX.setAttribute("axisname", "X");
    axisX.setAttribute("distance", modelX->getDistance());
    QVector<Level> vl = modelX->getLevels();
    size_t count = vl.size();
    for(size_t i = 0; i < count; i++){
        QDomElement l = doc.createElement("level");
        l.setAttribute("height", vl[i].levelHeight);
        l.setAttribute("leftKL", vl[i].angleLeftKL);
        l.setAttribute("leftKR", vl[i].angleLeftKR);
        l.setAttribute("rightKL", vl[i].angleRightKL);
        l.setAttribute("rightKR", vl[i].angleRightKR);
        axisX.appendChild(l);
    }
    QDomElement axisY = doc.createElement("axis");
    axisY.setAttribute("axisname", "Y");
    axisY.setAttribute("distance", modelY->getDistance());
    vl = modelY->getLevels();
    count = vl.size();
    for(size_t i = 0; i < count; i++){
        QDomElement l = doc.createElement("level");
        l.setAttribute("height", vl[i].levelHeight);
        l.setAttribute("leftKL", vl[i].angleLeftKL);
        l.setAttribute("leftKR", vl[i].angleLeftKR);
        l.setAttribute("rightKL", vl[i].angleRightKL);
        l.setAttribute("rightKR", vl[i].angleRightKR);
        axisY.appendChild(l);
    }
    root.appendChild(axisX);
    root.appendChild(axisY);
    doc.appendChild(root);
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    out << doc.toString();
    file.close();
    return true;
}

bool
MainWindow::saveFileAs(){
    qDebug() << "Saving file as ...";
    fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл как..."), "",
                                                    tr("Файлы XML (*.xml)"));
    if(fileName.isEmpty())
        return false;
    if (! (fileName.endsWith(".xml", Qt::CaseInsensitive)))
        fileName += ".xml";
    return saveFile();
}

void
MainWindow::exitProg(){
    qDebug() <<"exiting ...";
}

void
MainWindow::newFile(){
    qDebug() << "Creating new file ...";
    NewAMSDialog *dialog = new NewAMSDialog(this);
    int result = dialog->exec();
    if (result == QDialog::Accepted){
        qDebug() << dialog->getType() << "\t" << dialog->getLevelsX() << "levels";
        this->modelX = new AMSModel(dialog->getType(),
                                    dialog->getLevelsX(),
                                    dialog->getDistanceX());
        this->modelY = new AMSModel(dialog->getType(),
                                    dialog->getLevelsY(),
                                    dialog->getDistanceY());
        ui->tabWidget->setVisible(true);
        ui->levels_X->setModel(modelX);
        ui->levels_Y->setModel(modelY);
        setupView();
    }
}

void
MainWindow::setupView(){
    dotsX.resize(modelX->getLevels().size());
    dotsY.resize(modelY->getLevels().size());
    //dotsZ.resize(mi)
    QBrush redBrush(Qt::red);
    QBrush blueBrush(Qt::blue);
    QPen blackpen(Qt::black);
    blackpen.setWidth(3);
    int x = 0;
    int y = 0;

    for(int i = 0; i < dotsX.size(); i++){
        dotsX[i] = sceneX->addRect(x, y, 10, 10, blackpen, redBrush);
        y += 100;
    }
    x = 0;
    y = 0;
    for(int i = 0; i < dotsY.size(); i++){
        dotsY[i] = sceneY->addRect(x, y, 10, 10, blackpen, blueBrush);
        y += 100;
    }
/*
    ellipse = scene->addEllipse(0, 0, 100, 100, blackpen, redBrush);
    rectangle = scene->addRect(-25, 25, 50, 50, blackpen, blueBrush);
    rectangle->setFlag(QGraphicsItem::ItemIsMovable);
    */

}

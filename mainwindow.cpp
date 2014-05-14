#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newamsdialog.h"
#include "amsfromfiles.h"
#include <QTableWidget>
#include <QDebug>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QFileDialog>
#include <QStringList>
#include <iostream>
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
    ui->graphicsViewX->scale(1, -1);
    ui->graphicsViewY->scale(1, -1);
    //models
    this->modelX = new AMSModel(this);
    this->modelY = new AMSModel(this);
    connect(this->modelX, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(setupView()));
    connect(this->modelY, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(setupView()));
    //setupView();
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
                    setupView();
                }
            }
            else{
                qDebug() << *error << strNum;
            }
            file.close();
        }
    }
}

QVector<MesPoint>
MainWindow::readPoints(QTextStream &stream){
    QVector<MesPoint> points;
    QString str;
    while (!stream.atEnd()){
        str = stream.readLine();
        QStringList l = str.split(" ", QString::SkipEmptyParts);
        if(l.size() == 5){
            bool num = false;
            bool vangle = false;
            bool hangle = false;
            l.at(1).toInt(&num);
            l.at(2).toFloat(&vangle);
            l.at(3).toFloat(&hangle);
            if(num && vangle && hangle){
                MesPoint point;
                point.tc = l.at(0);
                point.num = l.at(1).toInt();
                point.va = l.at(2).toFloat();
                point.ha = l.at(3).toFloat();
                if(point.va > 180)
                    point.nva = 360 - point.va;
                else
                    point.nva = point.va;
                if(point.ha > 180)
                    point.nha = point.ha - 180;
                else
                    point.nha = point.ha;
                points.push_back(point);
            }
        }
    }
    if(stream.status() != QTextStream::Ok){
        qDebug() << "File reading error...";
    }
    return points;
}

void
MainWindow::pointsToModel(QVector<MesPoint> points, AMSModel* model){
    if(points.size() % 4 != 0){
        qDebug () << "Ошибка! Число точек должно быть кратно 4";
        return;
    }
    //отсортируем по высоте
    qSort(points.begin(), points.end(), [](MesPoint &a, MesPoint &b){return a.nva > b.nva;});
    for(auto i = points.begin(); i != points.end(); ++i){
        std::cout.setf(std::ios::fixed, std::ios::floatfield);
        std::cout.precision(4);
        std::cout << (*i).ha <<"\t" << (*i).va <<"\t" << (*i).nva << std::endl;
    }
    QVector<MesPoint> level;
    //теперь по 4 точки в уровне отсортируем по гор углу
    // и забьем в модель.
    auto i = points.begin();
    while(i != points.end()){
        level.push_back(*i); i++;
        level.push_back(*i); i++;
        level.push_back(*i); i++;
        level.push_back(*i); i++;
        qSort(level.begin(), level.end(), [](MesPoint &a, MesPoint &b){return a.nha < b.nha;});
        float lKL, lKR, rKL, rKR;
        if(level[0].tc.contains("F1")){
            lKL = level[0].ha; lKR = level[1].ha;
        }else{
            lKL = level[1].ha; lKR = level[0].ha;
        }
        if(level[2].tc.contains("F1")){
            rKL = level[2].ha; rKR = level[3].ha;
        }else{
            rKL = level[3].ha; rKR = level[2].ha;
        }
        Level l(0, lKL, lKR, rKL, rKR);
        model->addLevel(l);
        level.clear();
    }
    return;
}


void
MainWindow::createFromSDR(){
    /*
     * SDR файл содержит строки с данными измеренных углов
     * 09F1     228     91.14222        201.47056   Y
     * 1 поле -- круг теодолита
     * 2 поле -- номер точки
     * 3 поле -- вертикальный угол на засечку
     * 4 -- горизонтальный угол
     * 5 -- какая-то буква
     * кроме того в начале файла идет еще какая-то служебная информация
     */
    qDebug() << "Creating from SDR file...";
    AmsFromFiles *dialog = new AmsFromFiles(this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    QString fileX = dialog->getFileX();
    QString fileY = dialog->getFileY();
    float distX = dialog->getDistX();
    float distY = dialog->getDistY();
    this->modelX->clearData();
    this->modelY->clearData();

    qDebug() << fileX << fileY << distX << distY;

    QFile file;
    QVector<MesPoint> points;
    QTextStream stream;
    //read X file and create modelX
    file.setFileName(fileX);
    if(!file.open(QIODevice::ReadOnly))
        return;
    stream.setDevice(&file);
    points = readPoints(stream);
    pointsToModel(points, modelX);
    file.close();
    //read Y file and create modelY
    file.setFileName(fileY);
    if(!file.open(QIODevice::ReadOnly))
        return;
    stream.setDevice(&file);
    points = readPoints(stream);
    pointsToModel(points, modelY);
    file.close();
    modelX->setDistance(distX);
    modelY->setDistance(distY);
    modelX->setType(dialog->getType());
    modelY->setType(dialog->getType());
    ui->tabWidget->setVisible(true);
    ui->levels_X->setModel(modelX);
    ui->levels_Y->setModel(modelY);


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
    this->modelX->clearData();
    this->modelY->clearData();
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

    }
}

void
MainWindow::setupView(){
    dotsX.clear();
    dotsY.clear();
    dotsZ.clear();
    sceneX->clear();
    sceneY->clear();
    int blockWidth = 10;
    int scaleX = 2;
    int scaleY = 15;
    dotsX.resize(modelX->getLevels().size());
    dotsY.resize(modelY->getLevels().size());
    //dotsZ.resize(mi)
    QBrush redBrush(Qt::red);
    QBrush blueBrush(Qt::blue);
    QPen blackpen(Qt::black);
    blackpen.setWidth(3);
    //drow bound lines
    QModelIndex hxi = modelX->index(dotsX.size()-1, 0);
    float hx = modelX->data(hxi, Qt::DisplayRole).toFloat();
    sceneX->addLine(0, 0, hx*scaleX, hx*scaleY);
    sceneX->addLine(0, 0, -hx*scaleX, hx*scaleY);
    QModelIndex hyi = modelY->index(dotsY.size()-1, 0);
    float hy = modelY->data(hyi, Qt::DisplayRole).toFloat();
    sceneY->addLine(0, 0, hy*scaleX, hy*scaleY);
    sceneY->addLine(0, 0, -hy*scaleX, hy*scaleY);

    //drow blocks and lines
    float x = 0;
    float y = 0;
    for(int i = 0; i < dotsX.size(); i++){
        QModelIndex xi = this->modelX->index(i, 5);
        QModelIndex hi = this->modelY->index(i, 0);
        float disl = modelX->data(xi, Qt::DisplayRole).toFloat();
        float h = modelX->data(hi, Qt::DisplayRole).toFloat();
        float xcoord = disl*scaleX;
        float ycoord = h*scaleY;
        sceneX->addLine(x, y, xcoord, ycoord, QPen(Qt::red, 3));
        dotsX[i] = sceneX->addRect(xcoord-blockWidth/2.0, ycoord-blockWidth/2.0 , blockWidth, blockWidth, blackpen, redBrush);
        x = xcoord; y = ycoord;
    }
    x = 0; y = 0;
    for(int i = 0; i < dotsY.size(); i++){
        QModelIndex yi = this->modelY->index(i, 5);
        QModelIndex hi = this->modelY->index(i, 0);
        float disl = modelY->data(yi, Qt::DisplayRole).toFloat();
        float h = modelY->data(hi, Qt::DisplayRole).toFloat();
        float xcoord = disl*scaleX;
        float ycoord = h*scaleY;
        sceneY->addLine(x, y, xcoord, ycoord, QPen(Qt::blue, 3));
        dotsY[i] = sceneY->addRect(xcoord-blockWidth/2.0, ycoord-blockWidth/2.0 , blockWidth, blockWidth, blackpen, blueBrush);
        x = xcoord; y = ycoord;
    }
/*
    ellipse = scene->addEllipse(0, 0, 100, 100, blackpen, redBrush);
    rectangle = scene->addRect(-25, 25, 50, 50, blackpen, blueBrush);
    rectangle->setFlag(QGraphicsItem::ItemIsMovable);
    */

}

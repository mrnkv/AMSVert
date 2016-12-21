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
    fileName()
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Ось X");
    ui->tabWidget->setTabText(1, "Ось Y");
    ui->tabWidget->setTabText(2, "Диаграммы");
    ui->tabWidget->setVisible(false);
    this->modelX = new AMSModel(this);
    this->modelY = new AMSModel(this);
    connect(this->modelX, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(drawResults()));
    connect(this->modelY, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(drawResults()));
}

void
MainWindow::drawResults(){
    //Нарисовать на графике X
    int numDots = modelX->getLevels().size();
    dotsX.clear();
    for(int i = 0; i < numDots; i++){
        QModelIndex xi = this->modelX->index(i, 5);
        QModelIndex hi = this->modelX->index(i, 0);
        float disl = modelX->data(xi, Qt::DisplayRole).toFloat();
        float h = modelX->data(hi, Qt::DisplayRole).toFloat();
        float xcoord = disl;
        float ycoord = h*1000;//переход от метров к милииметрам
        dotsX.push_back(QPointF(xcoord, ycoord));
    }
    numDots = modelY->getLevels().size();
    dotsY.clear();
    for(int i = 0; i < numDots; i++){
        QModelIndex yi = this->modelY->index(i, 5);
        QModelIndex hi = this->modelY->index(i, 0);
        float disl = modelY->data(yi, Qt::DisplayRole).toFloat();
        float h = modelY->data(hi, Qt::DisplayRole).toFloat();
        float xcoord = disl;
        float ycoord = h*1000;
        dotsY.push_back(QPointF(xcoord, ycoord));
    }
    ui->resultDrawX->setData(dotsX, modelX->getType());
    ui->resultDrawY->setData(dotsY, modelY->getType());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::openFile(){
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
                    ui->distance_X->setValue(modelX->getDistance());
                    ui->distance_Y->setValue(modelY->getDistance());
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
        if(str.left(2) == "09"){
            MesPoint point;
            point.tc = str.left(4);
            point.num = str.mid(20, 16).toInt();
            point.va = str.mid(52,16).toFloat();
            point.ha = str.mid(68,16).toFloat();
            (point.va > 180) ? (point.nva = 360 - point.va) : (point.nva = point.va);
            (point.ha > 180) ? (point.nha = point.ha - 180) : (point.nha = point.ha);
            points.push_back(point);
        }
        else{
            continue;
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
    AmsFromFiles *dialog = new AmsFromFiles(this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    QString fileX = dialog->getFileX();
    QString fileY = dialog->getFileY();
    float distX = dialog->getDistX();
    float distY = dialog->getDistY();
    this->modelX->clearData();
    this->modelY->clearData();

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
    ui->distance_X->setValue(modelX->getDistance());
    ui->distance_Y->setValue(modelY->getDistance());
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
            QString axisName = node.toElement().attribute("axisname");
            float dist = QString(node.toElement().attribute("distance")).toFloat();
            if(axisName == "X"){
                modelX->setDistance(dist);
            }
            else if(axisName == "Y"){
                modelY->setDistance(dist);
            }
            size_t c = node.childNodes().count();
            for(size_t j = 0; j < c; j++){
                QDomElement ln = node.childNodes().at(j).toElement();
                Level l(QString(ln.attribute("height")).toFloat(),
                        QString(ln.attribute("leftKL")).toFloat(),
                        QString(ln.attribute("leftKR")).toFloat(),
                        QString(ln.attribute("rightKL")).toFloat(),
                        QString(ln.attribute("rightKR")).toFloat());
                if(axisName == "X"){
                    modelX->addLevel(l);
                }
                else if(axisName == "Y"){
                    modelY->addLevel(l);
                }
            }
        }
    }
    ui->distance_X->setValue(modelX->getDistance());
    ui->distance_Y->setValue(modelY->getDistance());
    emit modelX->dataChanged(QModelIndex(), QModelIndex());
    emit modelY->dataChanged(QModelIndex(), QModelIndex());
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
        ui->distance_X->setValue(modelX->getDistance());
        ui->distance_Y->setValue(modelY->getDistance());

    }
}

void
MainWindow::setupView(){
}

void MainWindow::on_distance_X_valueChanged(double arg1)
{
    modelX->setDistance(arg1);
    emit modelX->dataChanged(QModelIndex(), QModelIndex());
}

void MainWindow::on_distance_Y_valueChanged(double arg1)
{
    modelY->setDistance(arg1);
    emit modelY->dataChanged(QModelIndex(), QModelIndex());
}

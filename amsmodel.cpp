#include <cmath>
#include <QDebug>
#include "amsmodel.h"

Level::Level(float h, float a1, float a2, float a3, float a4)
{
    levelHeight = h;
    angleLeftKL = a1;
    angleLeftKR = a2;
    angleRightKL = a3;
    angleRightKR = a4;
    dislocation = 0;

}

Level::Level()
{
}

AMSModel::AMSModel(QObject *parent) :
    QAbstractTableModel(parent),
    levels(0)
{

}
AMSModel::AMSModel(AMSModel::AMS_type type,
         size_t lvls,
         float dist,
         QObject *parent):
    QAbstractTableModel(parent),
    levels(lvls)
{
    this->type = type;
    this->dist = dist;
    /*
    Level l1 = Level(0, 41.5225, 221.5244, 43.53, 223.5292);
    levels[0] = l1;
    l1 = Level(7.2, 41.5497, 221.5517, 43.5536, 223.5556);
    levels[1] = l1;
    */
}

int
AMSModel::rowCount(const QModelIndex &/*parent*/)const{
    return levels.size();
}

int
AMSModel::columnCount(const QModelIndex &/*parent*/)const{
    /*
     * высота
     * левый пояс левый круг
     * левый пояс правый круг
     * правый пояс левый круг
     * правыйпояс правый круг
     * смещение
     */
    return 6;
}

QVariant
AMSModel::data(const QModelIndex &index, int role) const{
    int row = index.row();
    int col = index.column();
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole){
        switch(col){
        case 0:
            return levels[row].levelHeight;
        case 1:
            return levels[row].angleLeftKL;
        case 2:
            return levels[row].angleLeftKR;
        case 3:
            return levels[row].angleRightKL;
        case 4:
            return levels[row].angleRightKR;
        case 5:
            return calcDislocation(row, levels, dist);
        }
    }
    return QVariant();
}

Qt::ItemFlags
AMSModel::flags(const QModelIndex & /*index*/) const{
    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool
AMSModel::setData(const QModelIndex & index, const QVariant & value, int role){
    int row = index.row();
    int col = index.column();
    if (role == Qt::EditRole)
    {
        switch(col){
        case 0:
            levels[row].levelHeight = value.toFloat();
            break;
        case 1:
            levels[row].angleLeftKL = value.toFloat();
            break;
        case 2:
            levels[row].angleLeftKR = value.toFloat();;
            break;
        case 3:
            levels[row].angleRightKL = value.toFloat();;
            break;
        case 4:
            levels[row].angleRightKR = value.toFloat();;
        }
    }
    return true;
}

QVariant
AMSModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString(tr("Высота"));
            case 1:
                return QString(tr("Левый(KL)"));
            case 2:
                return QString(tr("Левый(KR)"));
            case 3:
                return QString(tr("Правый(KL)"));
            case 4:
                return QString(tr("Правый(KR)"));
            case 5:
                return QString(tr("Смещение (мм)"));
            }
        }
    }
    return QVariant();
}

void
AMSModel::setType(AMSModel::AMS_type type){
    this->type = type;
}

void
AMSModel::setDistance(float dist){
    this->dist = dist;
}

void
AMSModel::addLevel(Level l){
    this->levels.push_back(l);
}
AMSModel::AMS_type
AMSModel::getType(){
    return type;
}

float
AMSModel::getDistance(){
    return dist;
}

QVector<Level>
AMSModel::getLevels(){
    return levels;
}
void
AMSModel::ifDataChanged(){
}

float
calcDislocation(int level, QVector<Level> &levels, float dist){
        qDebug() << "============================================";
        qDebug() << level;
        float leftKL, leftKR, rightKL, rightKR;
        if (levels[level].angleLeftKL > 1800.0)
            leftKL = levels[level].angleLeftKL - 180.0;
        else
            leftKL = levels[level].angleLeftKL;
        if (levels[level].angleLeftKR > 180)
            leftKR = levels[level].angleLeftKR - 180;
        else
            leftKR = levels[level].angleLeftKR;
        if (levels[level].angleRightKL> 180)
            rightKL = levels[level].angleRightKL - 180;
        else
            rightKL = levels[level].angleRightKL;
        if (levels[level].angleRightKR> 180)
            rightKR = levels[level].angleRightKR - 180;
        else
            rightKR = levels[level].angleRightKR;

        levels[level].averLeft = (leftKL + leftKR)/2.0;
        qDebug() << "averLeft = " << levels[level].averLeft;
        levels[level].averRight = (rightKL + rightKR)/2.0;
        qDebug() << "averRight = " << levels[level].averRight;
        levels[level].averAngle = (levels[level].averLeft + levels[level].averRight)/2.0;
        qDebug() << "averAngle = " <<levels[level].averAngle;
        float distance = levels[level].averAngle - levels[0].averAngle;
        qDebug() << "levels[0].averAngle = " << levels[0].averAngle;
        qDebug() << "distance = " << distance;
        float t = tanf(distance * M_PI/180.0);
        qDebug() << "tan = " << t;
        qDebug() << "dist = " << dist;
        float dislocation = t*dist*1000;
        qDebug() << "dislovation = " << dislocation;
        levels[level].dislocation = dislocation;
        return dislocation;
}

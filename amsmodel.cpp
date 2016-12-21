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
    QAbstractItemModel(parent),
    levels(0)
{
}

AMSModel::AMSModel(AMSModel::AMS_type type,
         size_t lvls,
         float dist,
         QObject *parent):
    QAbstractItemModel(parent),
    levels()
{
    this->type = type;
    this->dist = dist;
    for(size_t i = 0; i < lvls; ++i){
        Level l(0,0,0,0,0);
        levels.push_back(l);
    }
}

int
AMSModel::rowCount(const QModelIndex &/*parent*/)const{
    return levels.size();
}

QModelIndex
AMSModel::parent(const QModelIndex& /*child*/) const{
    return QModelIndex();
}

int
AMSModel::columnCount(const QModelIndex& /*parent*/)const{
    /*
     * высота
     * левый пояс левый круг
     * левый пояс правый круг
     * правый пояс левый круг
     * правыйпояс правый круг
     * смещение
     * превышение
     */
    return 7;
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
            return QString::number(levels[row].angleLeftKL, 'f', 4);
        case 2:
            return QString::number(levels[row].angleLeftKR, 'f', 4);
        case 3:
            return QString::number(levels[row].angleRightKL, 'f', 4);
        case 4:
            return QString::number(levels[row].angleRightKR, 'f', 4);
        case 5:
            return QString::number(calcDislocation(row, levels, dist), 'f', 0);
        case 6:
            float m = (this->type == AMSModel::MAST ? 1500 : 1000);
            float dop = 1000*levels[row].levelHeight/m;
            float dis = calcDislocation(row, levels, dist);
            if(dop > abs(dis))
                return QString("Норма");
            else
                return QString::number(abs(dis) - dop);
        }
    }
    return QVariant();
}

Qt::ItemFlags
AMSModel::flags(const QModelIndex& index) const{
    if(!index.isValid()){
        return 0;
    }
    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QModelIndex
AMSModel::index(int row, int column, const QModelIndex& parent) const {
    if(!hasIndex(row, column, parent))
        return QModelIndex();
    return createIndex(row, column, nullptr);
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
        emit this->dataChanged(index,index);
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
            case 6:
                return QString(tr("Превышение"));
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
AMSModel::clearData(){
    levels.clear();
}

float
calcDislocation(int level, QVector<Level> &levels, float dist){
        float leftKL, leftKR, rightKL, rightKR;
        if (levels[level].angleLeftKL > 180.0)
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
        levels[level].averRight = (rightKL + rightKR)/2.0;
        levels[level].averAngle = (levels[level].averLeft + levels[level].averRight)/2.0;
        float distance = levels[level].averAngle - levels[0].averAngle;
        float t = tanf(distance * M_PI/180.0);
        float dislocation = t*dist*1000;
        levels[level].dislocation = dislocation;
        return dislocation;
}

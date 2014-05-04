#include "amsmodel.h"

Level::Level(float h, float a1, float a2, float a3, float a4)
{
    levelHigh = h;
    angleLeftKL = a1;
    angleLeftKR = a2;
    angleRightKL = a3;
    angleRightKR = a4;

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
     */
    return 5;
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
            return levels[row].levelHigh;
        case 1:
            return levels[row].angleLeftKL;
        case 2:
            return levels[row].angleLeftKR;
        case 3:
            return levels[row].angleRightKL;
        case 4:
            return levels[row].angleRightKR;
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
            levels[row].levelHigh = value.toFloat();
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

#ifndef AMSMODEL_H
#define AMSMODEL_H

#include <QAbstractTableModel>
#include <QtXml/QDomDocument>

/*
 *АМС модель - представляет модель данных для описания АМС
 * используется простая структура
 * каждая строка таблицы содержит структуру типа Level
 */
struct Level{
    Level();
    Level(float, float, float, float, float);
    float levelHeight;
    float angleLeftKL;
    int angleLeftKLm[3];
    float angleLeftKR;
    int angleLeftKRm[3];
    float angleRightKL;
    int angleRightKLm[3];
    float angleRightKR;
    int angleRightKRm[3];
    float averLeft;
    int averLeftm[3];
    float averRight;
    int averRightm[3];
    float averAngle;
    int averAnglem[3];
    float dislocation;
};

class AMSModel : public QAbstractTableModel

{
    Q_OBJECT
public:
    explicit AMSModel(QObject *parent = 0);
    enum AMS_type { MAST, TOWER};
    AMSModel(AMSModel::AMS_type ams_type,
        size_t levels,
        float dist,
        QObject *parent = 0);
    int rowCount(const QModelIndex &parent)const;
    int columnCount(const QModelIndex &parent)const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & /*index*/) const;
    void setType(AMSModel::AMS_type);
    void setDistance(float);
    void addLevel(Level);
    AMSModel::AMS_type getType();
    float getDistance();
    QVector<Level> getLevels();
signals:
public slots:
    void ifDataChanged();
private:
    AMSModel::AMS_type type;
    float dist;
    mutable QVector<Level> levels;
};

float calcDislocation(int level, QVector<Level> &, float dist);

#endif // AMSMODEL_H

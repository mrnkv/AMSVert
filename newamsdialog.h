#ifndef NEWAMSDIALOG_H
#define NEWAMSDIALOG_H

#include <QDialog>
#include <QString>
#include "amsmodel.h"

namespace Ui {
class NewAMSDialog;
}

class NewAMSDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewAMSDialog(QWidget *parent = 0);
    ~NewAMSDialog();
    int getLevelsX();
    int getLevelsY();
    float getDistanceX();
    float getDistanceY();
    AMSModel::AMS_type getType();

private:
    Ui::NewAMSDialog *ui;
};

#endif // NEWAMSDIALOG_H

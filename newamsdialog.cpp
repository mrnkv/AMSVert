#include "newamsdialog.h"
#include "ui_newamsdialog.h"

NewAMSDialog::NewAMSDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAMSDialog)
{
    ui->setupUi(this);
    ui->type->addItem("Мачта");
    ui->type->addItem("Башня");
}

AMSModel::AMS_type
NewAMSDialog::getType(){
    if (ui->type->currentText() == "Мачта")
        return AMSModel::MAST;
    return AMSModel::TOWER;
}

int
NewAMSDialog::getLevelsX(){
    return ui->levels_X->value();
}
int

NewAMSDialog::getLevelsY(){
    return ui->levels_Y->value();
}

float
NewAMSDialog::getDistanceX(){
    return ui->distance_X->value();
}

float
NewAMSDialog::getDistanceY(){
    return ui->distance_Y->value();
}


NewAMSDialog::~NewAMSDialog()
{
    delete ui;
}

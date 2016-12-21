#include "amsfromfiles.h"
#include "ui_amsfromfiles.h"

#include <QFileDialog>

AmsFromFiles::AmsFromFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AmsFromFiles)
{
    ui->setupUi(this);
    ui->amsType->addItem(tr("Mast"));
    ui->amsType->addItem(tr("Tower"));

}

AmsFromFiles::~AmsFromFiles()
{
    delete ui;
}

float
AmsFromFiles::getDistX(){
    return ui->distX->value();
}

float
AmsFromFiles::getDistY(){
    return ui->distY->value();
}


QString
AmsFromFiles::getFileX(){
    return fileXname;
}

QString
AmsFromFiles::getFileY(){
    return fileYname;
}

void
AmsFromFiles::setFileX(){

    fileXname = QFileDialog::getOpenFileName(this, tr("Select measurements file"), "",
                                                    tr("Files SDR (*.SDR)"));
    ui->fileX->setText(fileXname);
}

void
AmsFromFiles::selFileY(){
    fileYname = QFileDialog::getOpenFileName(this, tr("Select measurements file"), "",
                                                    tr("Files SDR (*.SDR)"));
    ui->fileY->setText(fileYname);
}

AMSModel::AMS_type
AmsFromFiles::getType(){
    if (ui->amsType->currentText() == tr("Mast"))
        return AMSModel::MAST;
    return AMSModel::TOWER;
}

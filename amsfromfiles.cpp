#include "amsfromfiles.h"
#include "ui_amsfromfiles.h"

#include <QFileDialog>

AmsFromFiles::AmsFromFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AmsFromFiles)
{
    ui->setupUi(this);
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
AmsFromFiles::selectXFile(){

    fileXname = QFileDialog::getOpenFileName(this, tr("Выбрать файл измерений"), "",
                                                    tr("Файлы SDR (*.SDR)"));
    ui->fileX->setText(fileXname);
}

void
AmsFromFiles::selectYFile(){
    fileYname = QFileDialog::getOpenFileName(this, tr("Выбрать файл измерений"), "",
                                                    tr("Файлы SDR (*.SDR)"));
    ui->fileY->setText(fileYname);
}

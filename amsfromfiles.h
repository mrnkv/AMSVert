#ifndef AMSFROMFILES_H
#define AMSFROMFILES_H

#include <QDialog>
#include "newamsdialog.h"

namespace Ui {
class AmsFromFiles;
}

class AmsFromFiles : public QDialog
{
    Q_OBJECT

public:
    explicit AmsFromFiles(QWidget *parent = 0);
    ~AmsFromFiles();
    float getDistX();
    float getDistY();
    QString getFileX();
    QString getFileY();
    AMSModel::AMS_type getType();

public slots:
    void setFileX();
    void selFileY();
private:
    Ui::AmsFromFiles *ui;
    QString fileXname;
    QString fileYname;
    float distX;
    float distY;
 };

#endif // AMSFROMFILES_H

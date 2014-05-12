#ifndef AMSFROMFILES_H
#define AMSFROMFILES_H

#include <QDialog>

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
public slots:
    void selectXFile();
    void selectYFile();
private:
    Ui::AmsFromFiles *ui;
    QString fileXname;
    QString fileYname;
    float distX;
    float distY;
 };

#endif // AMSFROMFILES_H

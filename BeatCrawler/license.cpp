#include "license.h"
#include "ui_license.h"

License::License(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::License)
{
    ui->setupUi(this);
    setWindowTitle("Enter Serial");

}

License::~License()
{
    delete ui;
}

void License::on_pushButton_Enter_License_clicked()
{

}

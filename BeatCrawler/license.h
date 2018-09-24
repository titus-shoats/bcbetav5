#ifndef LICENSE_H
#define LICENSE_H

#include <QDialog>

namespace Ui {
class License;
}

class License : public QDialog
{
    Q_OBJECT

public:
    explicit License(QWidget *parent = nullptr);
    ~License();

private:
    Ui::License *ui;
private slots:
    void on_pushButton_Enter_License_clicked();

};

#endif // LICENSE_H

#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

//    QFile f(":qdarkstyle/style.qss");
//    if (!f.exists())
//    {
//        qDebug() << "Unable to set stylesheet, file not found\n";
//    }
//    else
//    {
//        f.open(QFile::ReadOnly | QFile::Text);
//        QTextStream ts(&f);
//        qApp->setStyleSheet(ts.readAll());
//    }
    w.show();

    return a.exec();
}

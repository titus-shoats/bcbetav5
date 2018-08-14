#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include "webview.h"
#include "worker.h"
#include <QNetworkRequest>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getRequest();


private:
    Ui::MainWindow *ui;
    WebView *view1;
    WebView *view2;
    WebView *view3;
    WebView *view4;
    WebView *webview;

    QNetworkRequest *request1;
    QNetworkRequest *request2;
    QNetworkRequest *request3;
    QNetworkRequest *request4;
    QTimer *timer;
    Worker *worker;

public slots:
    void getThread();
    void receiverLoopWebViewParams(QString searchEngineParam);
    void receiverMultiLoopWebViewParams(QString searchEngineParamMulti1,QString searchEngineParamMulti2,
                                        QString searchEngineParamMulti3,QString searchEngineParamMulti4);
};

#endif // MAINWINDOW_H

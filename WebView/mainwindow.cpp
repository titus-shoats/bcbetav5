#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),view1(new WebView()),
    view2(new WebView()),view3(new WebView()),view4(new WebView()),
    request1(new QNetworkRequest()),request2(new QNetworkRequest()),
    request3(new QNetworkRequest()), request4(new QNetworkRequest()),timer(new QTimer()),
    webview(new WebView()), worker(new Worker())


{
    ui->setupUi(this);
    QSize size = this->size();
    this->setFixedSize(size);



    worker = new Worker();
    connect(worker,SIGNAL(emitLoopWebViewParams(QString)),this,SLOT(receiverLoopWebViewParams(QString )));
    connect(worker,SIGNAL(emitMultiLoopWebViewParams(QString,QString,QString,QString)),
            this,SLOT(receiverMultiLoopWebViewParams(QString,QString ,QString,QString)));

    view1->setGeometry(0,80,600,600);
    view1->setAttribute(Qt::WA_DeleteOnClose);
    view1->settings()->setAttribute(QWebSettings::JavascriptEnabled,false);

    view2->setGeometry(0,80,600,600);
    view2->setAttribute(Qt::WA_DeleteOnClose);
    view2->settings()->setAttribute(QWebSettings::JavascriptEnabled,false);

    view3->setGeometry(0,80,600,600);
    view3->setAttribute(Qt::WA_DeleteOnClose);
    view3->settings()->setAttribute(QWebSettings::JavascriptEnabled,false);

    view4->setGeometry(0,80,600,600);
    view4->setAttribute(Qt::WA_DeleteOnClose);
    view4->settings()->setAttribute(QWebSettings::JavascriptEnabled,false);

    worker->initHarvest();

    //connect(timer,&QTimer::timeout,webview,&WebView::insertEmailsJson);
    //timer->start(5000);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete view1;
    delete view2;
    delete view3;
    delete view4;

    delete request1;
    delete request2;
    delete request3;
    delete request4;
    delete timer;
    delete webview;
    delete worker;
}


void MainWindow::receiverLoopWebViewParams(QString searchEngineParam)
{

    // const QUrl url1("https://www.google.com/search?q=whats+my+ip&oq=whats+my+ip&aqs=chrome.0.35i39j0l5.3021j0j7&sourceid=chrome&ie=UTF-8");
    const QUrl url1(searchEngineParam);
    request1->setUrl(url1);
    view1->load(*request1);
    view1->view1Page(view1->page());
   // view1->show();
   // qDebug() << searchEngineParam;
}
void MainWindow::receiverMultiLoopWebViewParams(QString searchEngineParamMulti1,QString searchEngineParamMulti2,
                                                QString searchEngineParamMulti3,QString searchEngineParamMulti4)
{

//     qDebug() << "Multi 1 " << searchEngineParamMulti1;
//     qDebug() << "Multi 2 "<<  searchEngineParamMulti2;
//     qDebug() << "Multi 3 "<<  searchEngineParamMulti3;
//     qDebug() << "Multi 4 "<<  searchEngineParamMulti4;

    const QUrl url1(searchEngineParamMulti1);
    const QUrl url2(searchEngineParamMulti2);
   // const QUrl url3(searchEngineParamMulti3);
   // const QUrl url4(searchEngineParamMulti4);

    request1->setUrl(url1);
    request2->setUrl(url2);
   // request3->setUrl(url3);
   // request4->setUrl(url4);


    view1->load(*request1);
    view2->load(*request2);
    //view3->load(*request3);
   // view4->load(*request4);


    view1->view1Page(view1->page());
    view2->view2Page(view2->page());
   // view3->view3Page(view3->page());
   // view4->view4Page(view4->page());


   // view1->show();
   // view2->show();
   // view3->show();
  // view4->show();
}

void MainWindow::getThread(){
    //QFuture<void> thread1 = QtConcurrent::run(this,&MainWindow::getTimer);


}

void MainWindow::getRequest()
{
  //  view1->show();
  //  view->show();

   // const QUrl url1("http://bing.com");

   // QByteArray userAgent;
    //QNetworkProxy proxy;
    //proxy.setType(QNetworkProxy::HttpProxy);
   // proxy.setHostName(QString("173.234.232.178"));
   // proxy.setPort(3128);
   // proxy.setUser("username");
    //proxy.setPassword("password");
   // view->setViewProxy(proxy);

    //QNetworkProxy::setApplicationProxy(proxy);

   // page()->networkAccessManager()->setProxy(proxy);
    //request->setRawHeader("User-Agent",userAgent);
   // view->setViewProxy("http://173.234.232.8",3128);

     //request->setUrl(url);
     //view->load(*request);
    // view->hide();
    //request->setRawHeader("User-Agent",userAgent);


}

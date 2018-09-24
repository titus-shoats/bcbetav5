#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
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


    worker->initHarvest();

    //connect(timer,&QTimer::timeout,webview,&WebView::insertEmailsJson);
    //timer->start(5000);


}

MainWindow::~MainWindow()
{
    delete ui;
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

  //  QFuture<void> future1;
  //  future1 = QtConcurrent::run(this,&MainWindow::runThread1,searchEngineParam);


 //  view1.view1Page("https://www.bing.com/search?q=site%3ainstagram.com+%22%40gmail.com%22+%22my+music%22&qs=n&sp=-1&pq=site%3ainstagram.com+%22%40gmail.com%22+%22my+music%22&sc=1-42&sk=&cvid=86346455A4764D3C84738BC8701E5CED&first=11&FORM=PERE");
  // qDebug() << searchEngineParam;
    view1.view1Page(searchEngineParam);
}
void MainWindow::runThread1(QString url)
{

}
void MainWindow::receiverMultiLoopWebViewParams(QString searchEngineParamMulti1,QString searchEngineParamMulti2,
                                                QString searchEngineParamMulti3,QString searchEngineParamMulti4)
{

     //qDebug() << "Multi 1 " << searchEngineParamMulti1;
//     qDebug() << "Multi 2 "<<  searchEngineParamMulti2;
//     qDebug() << "Multi 3 "<<  searchEngineParamMulti3;
//     qDebug() << "Multi 4 "<<  searchEngineParamMulti4;


    mutex.lock();
   // view1.view1Page("https://www.bing.com/search?q=site%3ainstagram.com+%22%40gmail.com%22+%22my+music%22&qs=n&sp=-1&pq=site%3ainstagram.com+%22%40gmail.com%22+%22my+music%22&sc=1-42&sk=&cvid=86346455A4764D3C84738BC8701E5CED&first=11&FORM=PERE");
    view1.view1Page(searchEngineParamMulti1);
    mutex.unlock();

    mutex.lock();
   // view2->view2Page("https://www.bing.com/search?q=site%3Ainstagram.com%20%22%40gmail.com%22%20%22beats%20for%20sale%22&qs=n&form=QBRE&sp=-1&pq=site%3Ainstagram.com%20%22%40gmail.com%22%20%22beats%20for%20sale%22&sc=1-48&sk=&cvid=BE31A20458124FEFA6B350BD5330F6AE");
    view2->view2Page(searchEngineParamMulti2);
    mutex.unlock();


    mutex.lock();
    //view3->view3Page("https://www.google.com/search?ei=6dynW9jLLuGijwT42YPYBQ&q=site%3Ainstagram.com+%22%40yahoo.com%22+%22beats+for+sale%22&oq=site%3Ainstagram.com+%22%40yahoo.com%22+%22beats+for+sale%22&gs_l=psy-ab.3...2458.5735..6310...0.0..0.151.881.0j7......0....1..gws-wiz.JxW8nRarn3o");
    view3->view3Page(searchEngineParamMulti3);
    mutex.unlock();

    mutex.lock();
    //view4->view4Page("https://www.google.com/search?ei=8tynW6CzF4OzjwScj4-AAw&q=site%3Ainstagram.com+%22%40yahoo.com%22+%22need+beats%22&oq=site%3Ainstagram.com+%22%40yahoo.com%22+%22need+beats%22&gs_l=psy-ab.3...19173.20746..21249...0.0..0.154.1198.3j7......0....1..gws-wiz.DdgurK6_RLc");
    view4->view4Page(searchEngineParamMulti4);
    mutex.unlock();


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

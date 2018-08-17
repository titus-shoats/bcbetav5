#include "webview.h"
#include <QPaintEvent>
#include <QWebFrame>

WebView::WebView(QWidget *parent)
    : QWebView(parent)
    , inLoading(false),emailListFile(new QFile()),emailList(new QStringList()),
      readEmailFile(new QFile())
{
    connect(this, SIGNAL(loadStarted()), this, SLOT(newPageLoading()));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(pageLoaded(bool)));
    page()->setPreferredContentsSize(QSize(600, 768));
    openEmailListFile(emailListFile);
    readEmailListFile(readEmailFile);
    connOpen();
    workerCounterNum =0;
    workerCounterPtr = &workerCounterNum;

    proxyServerCounterNum =0;
    proxyServerCounterPtr =&proxyServerCounterNum;

    httpStatusNum = 0;
    httpStatusPtr =&httpStatusNum;
   // connect(&this->worker,&WebView::emitWebViewLog,this,&Worker::receiverWebViewLog);
   connect(this,&WebView::emitWebViewLog,&this->worker,&Worker::receiverWebViewLog);

}

WebView::~WebView(){
   // delete request;
    emailListFile->close();
    delete emailListFile;
    delete emailList;
    delete readEmailFile;
    //connClose();
}

void WebView::openEmailListFile(QFile * emailListFile){

//    emailListFile->setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emailList.txt");
//    if(! emailListFile->open(QIODevice::WriteOnly | QIODevice::Text)){
//        qDebug() << "Error opening keyword file in worker";
//        return;
//    }
}

void WebView::readEmailListFile(QFile * readEmailFile) const
{

//    readEmailFile->setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emailList.txt");
//    if(! readEmailFile->open(QIODevice::ReadOnly | QIODevice::Text)){
//        qDebug() << "Error opening email file in webview";
//        return;
//    }
}


void WebView::paintEvent(QPaintEvent *event)
{
    if (inLoading && loadingTime.elapsed() < 750) {
        QPainter painter(this);
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        foreach (const QRect &rect, event->region().rects()) {
            painter.drawRect(rect);
        }
    } else {
        QWebView::paintEvent(event);
    }
}

void WebView::newPageLoading()
{
    inLoading = true;
    loadingTime.start();
}

// Rotates proxy with a specified interval, and rotate due to a httpStatus Code
int WebView::proxyRotater()
{

    QList <QString> *proxyServersJson;
    proxyServersJson = new QList<QString>();
    QString val;
    QFile file;
    file.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/params.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening json file in webview" << file.errorString();
    }
    val = file.readAll();
    file.close();
    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObject = document.object();
    QJsonValue proxyServers = jsonObject.value(QString("ProxyServers"));
    QJsonArray proxyServersArray = proxyServers.toArray();
    QJsonValue timerOptions = jsonObject.value(QString("TimerOptions"));
    QJsonObject timerOptionsKey = timerOptions.toObject();
    bool isProxyEmpty = false;
    int proxyRotateInterval = timerOptionsKey["ProxyRotate"].toInt();
    bool canProxyCounterIncrement = false;
    QString proxies;
    QStringList proxyServersList;
    enum defaultApplicationProxy{DEFAULT_PROXY_SETTINGS =0,PROXY_LIST=1};
    for(int i =0; i < proxyServersArray.size(); i++ )
    {

        proxyServersList.prepend(proxyServersArray[i].toString());
    }

    /*******If user insert 4 or more proxies create 4 proxy list******/
    if(proxyServersArray.size() >= 4)
    {
        /******Create 2 proxy list*****/
        for(int i=0; i < proxyServersArray.size(); i++)
        {
            if(i%2==0)
            {
                proxyList1 << proxyServersArray[i].toString();
            }else
            {
                proxyList2 << proxyServersArray[i].toString();

            }
        }

        /******Create 2 more proxy list*****/
        for(int i=0; i < proxyList1.size(); i++)
        {
            if(i%2==0)
            {
                proxyList3 << proxyList1.value(i);
            }else
            {
                proxyList4 << proxyList1.value(i);

            }
        }

        return PROXY_LIST;
    }// end of if proxyServersArray size is greater or equal to 4


   /*******If user inserts less than 4 proxies,roate, and use application wide proxy setting,****/
   if(proxyServersArray.size() < 4)
   {
       if (proxyServersList.isEmpty())
       {

           isProxyEmpty = true;
       }

       if (!proxyServersList.isEmpty())
       {
           isProxyEmpty = false;

       }

       // if workerCounter == *proxyRotateIntervalPtr, reset workerCounter; if certain number of
       // http request have been made rotate proxy
       if (*workerCounterPtr <= proxyRotateInterval)
       {
           // only rotate each proxy if proxyCounterPtr is not greater than our proxyServer qlist
           if ((*proxyServerCounterPtr) <= proxyServersList.size())
           {
               // if proxy counter is not greater than proxyServer qlist, proxyCounter can increment
               canProxyCounterIncrement = true;
           }

           // if proxy counter is equal to the size of proxyServer qlist, we cant increment
           if ((*proxyServerCounterPtr) == proxyServersList.size())
           {
               canProxyCounterIncrement = false;
               // if proxyServerCounter is equal to the size of the proxyServer qlist, reset it to 0
               *proxyServerCounterPtr = 0;
           }


           // if proxies contained in qlist empty in main thread, if so clear the proxylist in this thread also
           if (isProxyEmpty == true && proxyServersArray.size() == 0)
           {
               //qDebug() << "Proxy Empty";
               //qDebug() << *proxyServers;
               //qDebug() <<proxyServersArray.size();
               proxyServersList.clear();
               proxies = "";
           }

           // if proxies contained in qlist are not empty, and we can keep incrementing,
           // our proxies are good to use/rotate
           if (isProxyEmpty == false && canProxyCounterIncrement == true)
           {
               proxies = proxyServersList.value(*proxyServerCounterPtr);
               //qDebug() << "Counter-->" << proxies;

           }
           //qDebug() << "Counter-->" << *proxyServerCounterPtr;
           //qDebug() << "Proxies-->" << proxies;

       }


       // if workerCounter is greater than *proxyRotateIntervalPtr/ amount of http request before proxy rotates
       // or if a httpStatusError occured
       if (*workerCounterPtr >= proxyRotateInterval)
       {
           // restart workerCounter
           *workerCounterPtr = 0;
           // increment proxyServerPtr to go through each proxyServer index every interval
           (*proxyServerCounterPtr) += 1;

           QUrl proxies_(proxies);
           QNetworkProxy proxy;
           int proxyPort =proxies_.port();
           proxy.setType(QNetworkProxy::Socks5Proxy);
           proxy.setUser("username");
           proxy.setPassword("password");
           proxy.setType(QNetworkProxy::HttpProxy);
           proxy.setHostName(proxies_.host());
           proxy.setPort(static_cast<quint16>(proxyPort));
           QNetworkProxy::setApplicationProxy(proxy);
       }

       // increment workerCounter if we have not hit our http request limit to rotate each proxy
       (*workerCounterPtr) += 1;
       return DEFAULT_PROXY_SETTINGS;

   }// end of if proxyServersArray size is less than 4

}

void WebView::view1Page(QWebPage *page)
{

    qDebug() << "proxy rotater xxxx--->" << proxyRotater();

    /******
      If httpStatus Errors occured/viewPage1ProxyRotate == true
      and proxyRotater() ==1/has proxy list, use list
    *****/
    if(viewPage1ProxyRotate ==true && proxyRotater() == 1)
    {
        if(!proxyList1.isEmpty())
        {

        }
        qDebug() << "View 1 Http status ERROR ,Use proxy list ";
    }
    /******
      If httpStatus Errors occur/viewPage1ProxyRotate == true
      and proxyRotater() ==0/use default proxy settings
    *****/
    if(viewPage1ProxyRotate == true && proxyRotater() == 0)
    {
        qDebug() << "View 1 Http status ERROR ,Use default proxy settings";

    }



    /******
      If httpStatus Errors dont occur/viewPage1ProxyRotate == false
      and proxyRotater() ==0/use default proxy settings
    *****/
    if(viewPage1ProxyRotate == false && proxyRotater() == 0)
    {
        qDebug() << "View 1 Http status SUCCESSFUL ,,Use default proxy settings ";

    }

    /******
      If httpStatus Errors dont occur/viewPage1ProxyRotate == false
      and proxyRotater() ==1/use proxy list
    *****/
    if(viewPage1ProxyRotate == false && proxyRotater() == 1)
    {
        qDebug() << "View 1 Http status SUCCESSFUL  Use proxy list ";

    }


    connect(this, &QWebView::loadFinished, [this,page](bool success) {

        connect(page->networkAccessManager(),SIGNAL(finished(QNetworkReply *)),
                             this,SLOT(view1HttpResponseFinished(QNetworkReply *)));

        if (!success)
        {
            qDebug() << "Could not load page Multi Instance 1";
        }
       if(success) {



            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");
            QRegularExpressionMatchIterator i = re.globalMatch( page->mainFrame()->toPlainText());
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;
          //  connOpen();
            mydb = QSqlDatabase::addDatabase("QSQLITE");
            mydb.setDatabaseName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emailtest.db");
            if(!mydb.open())
            {
                qDebug() << "Error connecting to database";
                return;
            }else
            {
              qDebug() << "Successfully connected to database";
            }
            QSqlQuery qry;
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (!match.captured(0).isEmpty())
                    word = match.captured(0);
                if (!word.isEmpty()) {
                    words << word;
                    if (!words.value(num).isEmpty())

                    {
                        if(!qry.exec("insert or replace into crawleremails values('"+words.value(num)+"')"))
                        {
                            qDebug() << "Error inserting emails into database:: " <<
                                        qry.lastError().text();

                            return;
                        }else
                        {
                            qDebug() <<  "Successfully inserted emails-->" << words.value(num);
                        }


//                       QSqlQuery query("SELECT * FROM crawleremails where email = '"+words.value(num)+"' " );
//                       int fieldNo = query.record().indexOf("email");
//                       int count =0;
//                       while (query.next())
//                       {
//                           count++;
//                          if(words.value(num) ==  query.value(fieldNo).toString())
//                          {
//                              qDebug() << "Duplicate exist";
//                              return;
//                          }else
//                          {
//                              if(!qry.exec("insert into crawleremails values('"+words.value(num)+"')"))
//                              {
//                                  qDebug() << "Error inserting emails into database";
//                                  return;
//                              }
//                          }

//                       }


                    }
                    num++;

                } // end words !empty

            } // end while
          connClose();
        }

   });
}
void WebView::view1HttpResponseFinished(QNetworkReply * reply)
{
    int httpStatusCode;
    httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(httpStatusCode ==200)
    {
        viewPage1ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded \n"));

    }
    if(httpStatusCode> 200)
    {
        viewPage1ProxyRotate = true;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode));

    }

  //  emit emitWebViewLog(QString("HTTP Status: ") + QString(reply->error()));


//    switch (httpStatusCode)
//    {

//       case 200:
//             emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded \n"));
//            *httpStatusPtr = httpStatusCode;
//            viewPage1ProxyRotate = false;
//      break;
//      case 301:
//             emit emitWebViewLog(QString("HTTP Status: 301 Redirect rotating proxy, " +
//                                       QString(proxies.host() +" \n")));
//            *httpStatusPtr = httpStatusCode;
//             viewPage1ProxyRotate = true;

//        break;
//      case 302:
//            emit emitWebViewLog(QString("HTTP Status: 302 Redirect, rotating proxy " +
//                                       QString(proxies.host() +" \n")));
//           *httpStatusPtr = httpStatusCode;
//           viewPage1ProxyRotate = true;
//       break;
//      case 304:
//        emit emitWebViewLog(QString("HTTP Status: 304 Redirect, rotating proxy " +
//                                   QString(proxies.host() +" \n")));
//            *httpStatusPtr = httpStatusCode;
//             viewPage1ProxyRotate = true;
//       break;
//      case 400:
//        emit emitWebViewLog(QString("HTTP Status: 400 Bad Request, rotating proxy " +
//                                   QString(proxies.host() +" \n")));
//            *httpStatusPtr = httpStatusCode;
//            viewPage1ProxyRotate = true;
//       break;
//      case 403:
//        emit emitWebViewLog(QString("HTTP Status: 403 Forbidden, rotating proxy " +
//                                   QString(proxies.host() +" \n")));
//            *httpStatusPtr = httpStatusCode;
//             viewPage1ProxyRotate = true;
//       break;
//      case 404:
//        emit emitWebViewLog(QString("HTTP Status: 404 Not Found, rotating proxy " +
//                                   QString(proxies.host() +" \n")));
//            *httpStatusPtr = httpStatusCode;
//             viewPage1ProxyRotate = true;
//       break;
//      case 500:
//        emit emitWebViewLog(QString("HTTP Status: 500 Internal Server Error rotating proxy " +
//                                   QString(proxies.host() +" \n")));
//            *httpStatusPtr = httpStatusCode;
//             viewPage1ProxyRotate = true;
//       break;

//      case 502:
//        emit emitWebViewLog(QString("HTTP Status: 502 Bad Gateway, rotating proxy " +
//                                   QString(proxies.host() +" \n")));
//           *httpStatusPtr = httpStatusCode;
//             viewPage1ProxyRotate = true;
//       break;
//      case 503:
//        emit emitWebViewLog(QString("HTTP Status: 503 Service Unavailable, rotating proxy " +
//                                   QString(proxies.host() +" \n")));
//            *httpStatusPtr = httpStatusCode;
//             viewPage1ProxyRotate = true;
//       break;

//        default:
//        emit emitWebViewLog(QString("HTTP Status: Void, rotating proxy " +
//                                   QString(proxies.host() +" \n")));
//        viewPage1ProxyRotate = true;
//    }
//    switch(reply->error())
//    {
//    case QNetworkReply::NoError:
//         // No error
//         return;
//    case QNetworkReply::ContentNotFoundError:
//        failedUrl = reply->url();
//        httpStatusCode = reply->attribute(
//                QNetworkRequest::HttpStatusCodeAttribute).toInt();
//        break;
//    case QNetworkReply::ConnectionRefusedError:
//        emit emitWebViewLog(QString("The remote server refused the connection (the server is not accepting requests. \n"));
//        break;
//    case QNetworkReply::TimeoutError:
//        emit emitWebViewLog(QString("The connection to the remote server timed out \n"));
//        break;
//    case QNetworkReply::HostNotFoundError:
//        emit emitWebViewLog(QString("The host was not found \n"));
//        break;
//    case QNetworkReply::ProxyConnectionClosedError:
//        emit emitWebViewLog(QString("Proxy Connection Error \n"));
//        break;
//    case QNetworkReply::ProxyConnectionRefusedError:
//        emit emitWebViewLog(QString("Proxy Refused Error \n"));
//        break;

//    case QNetworkReply::ProxyNotFoundError:
//        emit emitWebViewLog(QString("Proxy Not Found \n"));
//        break;

//    case QNetworkReply::ProxyTimeoutError:
//        emit emitWebViewLog(QString("Proxy Timedout Error \n"));
//        break;

//    }

}


void WebView::view2Page(QWebPage *page)
{



    /******
      If httpStatus Errors occured/viewPage1ProxyRotate == true
      and proxyRotater() ==1/has proxy list, use list
    *****/
    if(viewPage2ProxyRotate ==true && proxyRotater() == 1)
    {
        if(!proxyList1.isEmpty())
        {

        }
        qDebug() << "View 2 Http status ERROR ,Use proxy list ";
    }
    /******
      If httpStatus Errors occur/viewPage1ProxyRotate == true
      and proxyRotater() ==0/use default proxy settings
    *****/
    if(viewPage2ProxyRotate == true && proxyRotater() == 0)
    {
        qDebug() << "View 2 Http status ERROR ,Use default proxy settings";

    }



    /******
      If httpStatus Errors dont occur/viewPage1ProxyRotate == false
      and proxyRotater() ==0/use default proxy settings
    *****/
    if(viewPage2ProxyRotate == false && proxyRotater() == 0)
    {
        qDebug() << "View 2 Http status SUCCESSFUL ,Use default proxy settings ";

    }

    /******
      If httpStatus Errors dont occur/viewPage1ProxyRotate == false
      and proxyRotater() ==1/use proxy list
    *****/
    if(viewPage2ProxyRotate == false && proxyRotater() == 1)
    {
        qDebug() << "View 2 Http status SUCCESSFUL ,Use proxy list ";

    }

    connect(this, &QWebView::loadFinished, [this,page](bool success) {

        connect(page->networkAccessManager(),SIGNAL(finished(QNetworkReply *)),
                this,SLOT(view2HttpResponseFinished(QNetworkReply *)));
        if (!success)
        {
            qDebug() << "Could not load page view 2";
        }
       if(success) {

            qDebug() << "Page  loaded view 2";


            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");
            QRegularExpressionMatchIterator i = re.globalMatch( page->mainFrame()->toPlainText());
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;
           // connOpen();
            mydb = QSqlDatabase::addDatabase("QSQLITE");
            mydb.setDatabaseName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emailtest.db");
            if(!mydb.open())
            {
                qDebug() << "Error connecting to database";
                return;
            }else
            {
              qDebug() << "Successfully connected to database";
            }
            QSqlQuery qry;
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (!match.captured(0).isEmpty())
                    word = match.captured(0);
                if (!word.isEmpty()) {
                    words << word;
                    if (!words.value(num).isEmpty())

                    {
                        if(!qry.exec("insert or replace into crawleremails values('"+words.value(num)+"')"))
                        {
                            qDebug() << "Error inserting emails into database:: " <<
                                        qry.lastError().text();
                            return;
                        }else
                        {
                            qDebug() <<  "Successfully inserted emails-->" << words.value(num);
                        }


//                       QSqlQuery query("SELECT * FROM crawleremails where email = '"+words.value(num)+"' " );
//                       int fieldNo = query.record().indexOf("email");
//                       int count =0;
//                       while (query.next())
//                       {
//                           count++;
//                          if(words.value(num) ==  query.value(fieldNo).toString())
//                          {
//                              qDebug() << "Duplicate exist";
//                              return;
//                          }else
//                          {
//                              if(!qry.exec("insert into crawleremails values('"+words.value(num)+"')"))
//                              {
//                                  qDebug() << "Error inserting emails into database";
//                                  return;
//                              }
//                          }

//                       }


                    }
                    num++;

                } // end words !empty

            } // end while
          connClose();
        }

   });
}
void WebView::view2HttpResponseFinished(QNetworkReply * reply)
{


    int httpStatusCode;
    httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(httpStatusCode ==200)
    {
        viewPage2ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded \n"));

    }
    if(httpStatusCode> 200)
    {
        viewPage2ProxyRotate = true;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode));

    }

    //emit emitWebViewLog(QString("HTTP Status: ") + QString(reply->error()));
}


void WebView::view3Page(QWebPage *page){



    /******
      If httpStatus Errors occured/viewPage1ProxyRotate == true
      and proxyRotater() ==1/has proxy list, use list
    *****/
    if(viewPage3ProxyRotate ==true && proxyRotater() == 1)
    {
        if(!proxyList1.isEmpty())
        {

        }
        qDebug() << "View 3 Http status ERROR ,Use proxy list ";
    }
    /******
      If httpStatus Errors occur/viewPage1ProxyRotate == true
      and proxyRotater() ==0/use default proxy settings
    *****/
    if(viewPage3ProxyRotate == true && proxyRotater() == 0)
    {
        qDebug() << "View 3 Http status ERROR ,Use default proxy settings";

    }



    /******
      If httpStatus Errors dont occur/viewPage1ProxyRotate == false
      and proxyRotater() ==0/use default proxy settings
    *****/
    if(viewPage3ProxyRotate == false && proxyRotater() == 0)
    {
        qDebug() << "View 3 Http status SUCCESSFUL ,Use default proxy settings ";

    }

    /******
      If httpStatus Errors dont occur/viewPage1ProxyRotate == false
      and proxyRotater() ==1/use proxy list
    *****/
    if(viewPage3ProxyRotate == false && proxyRotater() == 1)
    {
        qDebug() << "View 3 Http status SUCCESSFUL ,Use proxy list ";

    }

    connect(this, &QWebView::loadFinished, [this,page](bool success) {

    connect(page->networkAccessManager(),SIGNAL(finished(QNetworkReply *)),
       this,SLOT(view3HttpResponseFinished(QNetworkReply *)));



        if (!success)
        {
            qDebug() << "Could not load page view 3";
        }
       if(success) {
            qDebug() << "Page loaded view 3";

            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");
            QRegularExpressionMatchIterator i = re.globalMatch( page->mainFrame()->toPlainText());
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;
           // connOpen();
            mydb = QSqlDatabase::addDatabase("QSQLITE");
            mydb.setDatabaseName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emailtest.db");
            if(!mydb.open())
            {
                qDebug() << "Error connecting to database";
                return;
            }else
            {
              qDebug() << "Successfully connected to database";
            }
            QSqlQuery qry;
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (!match.captured(0).isEmpty())
                    word = match.captured(0);
                if (!word.isEmpty()) {
                    words << word;
                    if (!words.value(num).isEmpty())

                    {
                        if(!qry.exec("insert or replace into crawleremails values('"+words.value(num)+"')"))
                        {
                            qDebug() << "Error inserting emails into database:: " <<
                                        qry.lastError().text();
                            return;
                        }else
                        {
                            qDebug() <<  "Successfully inserted emails-->" << words.value(num);
                        }


//                       QSqlQuery query("SELECT * FROM crawleremails where email = '"+words.value(num)+"' " );
//                       int fieldNo = query.record().indexOf("email");
//                       int count =0;
//                       while (query.next())
//                       {
//                           count++;
//                          if(words.value(num) ==  query.value(fieldNo).toString())
//                          {
//                              qDebug() << "Duplicate exist";
//                              return;
//                          }else
//                          {
//                              if(!qry.exec("insert into crawleremails values('"+words.value(num)+"')"))
//                              {
//                                  qDebug() << "Error inserting emails into database";
//                                  return;
//                              }
//                          }

//                       }


                    }
                    num++;

                } // end words !empty

            } // end while
          connClose();
        }

   });
}
void WebView::view3HttpResponseFinished(QNetworkReply * reply)
{


    int httpStatusCode;
    httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(httpStatusCode ==200)
    {
        viewPage3ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded \n"));

    }
    if(httpStatusCode> 200)
    {
        viewPage3ProxyRotate = true;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode));

    }

  //  emit emitWebViewLog(QString("HTTP Status: ") + QString(reply->error()));

}

void WebView::view4Page(QWebPage *page){


    /******
      If httpStatus Errors occured/viewPage1ProxyRotate == true
      and proxyRotater() ==1/has proxy list, use list
    *****/
    if(viewPage4ProxyRotate ==true && proxyRotater() == 1)
    {
        if(!proxyList1.isEmpty())
        {

        }
        qDebug() << "View 4 Http status ERROR ,Use proxy list ";
    }
    /******
      If httpStatus Errors occur/viewPage1ProxyRotate == true
      and proxyRotater() ==0/use default proxy settings
    *****/
    if(viewPage4ProxyRotate == true && proxyRotater() == 0)
    {
        qDebug() << "View 4 Http status ERROR ,Use default proxy settings";

    }



    /******
      If httpStatus Errors dont occur/viewPage1ProxyRotate == false
      and proxyRotater() ==0/use default proxy settings
    *****/
    if(viewPage4ProxyRotate == false && proxyRotater() == 0)
    {
        qDebug() << "View 4 Http status SUCCESSFUL ,Use default";

    }

    /******
      If httpStatus Errors dont occur/viewPage1ProxyRotate == false
      and proxyRotater() ==1/use proxy list
    *****/
    if(viewPage4ProxyRotate == false && proxyRotater() == 1)
    {
        qDebug() << "View 4 Http status SUCCESSFUL ,Use proxy list ";

    }


    connect(this, &QWebView::loadFinished, [this,page](bool success) {

        connect(page->networkAccessManager(),SIGNAL(finished(QNetworkReply *)),
                this,SLOT(view4HttpResponseFinished(QNetworkReply *)));

        if (!success)
        {
            qDebug() << "Could not load page view 4";
        }
       if(success) {
            qDebug() << "Page loaded view 4";

            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");
            QRegularExpressionMatchIterator i = re.globalMatch( page->mainFrame()->toPlainText());
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;
            //connOpen();
            mydb = QSqlDatabase::addDatabase("QSQLITE");
            mydb.setDatabaseName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emailtest.db");
            if(!mydb.open())
            {
                qDebug() << "Error connecting to database";
                return;
            }else
            {
              qDebug() << "Successfully connected to database";
            }
            QSqlQuery qry;

            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (!match.captured(0).isEmpty())
                    word = match.captured(0);
                if (!word.isEmpty()) {
                    words << word;
                    if (!words.value(num).isEmpty())

                    {
                        if(!qry.exec("insert or replace into crawleremails values('"+words.value(num)+"')"))
                        {
                            qDebug() << "Error inserting emails into database:: " <<
                                        qry.lastError().text();
                            return;
                        }else
                        {
                            qDebug() <<  "Successfully inserted emails-->" << words.value(num);
                        }


//                       QSqlQuery query("SELECT * FROM crawleremails where email = '"+words.value(num)+"' " );
//                       int fieldNo = query.record().indexOf("email");
//                       int count =0;
//                       while (query.next())
//                       {
//                           count++;
//                          if(words.value(num) ==  query.value(fieldNo).toString())
//                          {
//                              qDebug() << "Duplicate exist";
//                              return;
//                          }else
//                          {
//                              if(!qry.exec("insert into crawleremails values('"+words.value(num)+"')"))
//                              {
//                                  qDebug() << "Error inserting emails into database";
//                                  return;
//                              }
//                          }

//                       }


                    }
                    num++;

                } // end words !empty

            } // end while
          connClose();
        }

   });
}
void WebView::view4HttpResponseFinished(QNetworkReply * reply)
{
    int httpStatusCode;
    httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(httpStatusCode ==200)
    {
        viewPage4ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded \n"));

    }
    if(httpStatusCode> 200)
    {
        viewPage4ProxyRotate = true;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode));

    }

   // emit emitWebViewLog(QString("HTTP Status: ") + QString(reply->error()));
}


void WebView::pageLoaded(bool successLoaded)
{
    inLoading = false;
    update();

}

//void WebView::setViewProxy(QString proxyHost,quint16 proxyPort)
void WebView::setViewProxy(QNetworkProxy proxy)
{
    // QNetworkProxy proxy;
     //proxy.setType(QNetworkProxy::Socks5Proxy);
     // proxy.setUser("username");
     // proxy.setPassword("password");
     //proxy.setType(QNetworkProxy::HttpProxy);
    // proxy.setHostName(proxyHost);
     //proxy.setPort(proxyPort);
     //QNetworkProxy::setApplicationProxy(proxy);
     page()->networkAccessManager()->setProxy(proxy);
}

void WebView::setParams()
{
  // request->setUrl(url);
  // request->setRawHeader("User-Agent",userAgent);
    for(;;){
        QEventLoop loop;
        QTimer::singleShot(5000,&loop,SLOT(quit()));
        loop.exec();
        emit emitSenderTimer();

    }
}

void WebView::insertEmailsJson()
{
   // qDebug() <<*emailList;

       QTextStream readEmailStream(readEmailFile);
         for(int  i =0; i < emailList->size(); i++)
            {
             //qDebug() <<emailList->value(i);
               if(!readEmailStream.readLine().isEmpty())
                {
                   if(readEmailStream.readLine() != emailList->value(i))
                   {
                      // qDebug() <<emailList->value(i);
                      // readEmailStream << emailList->value(i);
                   }else{
                      // qDebug() << emailList->value(i);
                   }
                }


            }


}

void WebView::connOpen()
{
    mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emailtest.db");
    if(!mydb.open())
    {
        qDebug() << "Error connecting to database";
        return;
    }else
    {
      qDebug() << "Successfully connected to database";
    }
}

void WebView::connClose()
{
    mydb.close();
    mydb.removeDatabase(QSqlDatabase::defaultConnection);
}

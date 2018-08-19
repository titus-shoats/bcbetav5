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

    proxies = new QString();

    httpStatusNum = 0;
    httpStatusPtr =&httpStatusNum;
   // connect(&this->worker,&WebView::emitWebViewLog,this,&Worker::receiverWebViewLog);
   connect(this,&WebView::emitWebViewLog,&this->worker,&Worker::receiverWebViewLog);

   initProxyListSettings();
   replyUrl = new QString();

}

WebView::~WebView(){
    delete replyUrl;
    emailListFile->close();
    delete emailListFile;
    delete emailList;
    delete readEmailFile;
    delete proxies;
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
void WebView::initProxyListSettings()
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
    proxyServersArray = proxyServers.toArray();
    QJsonValue timerOptions = jsonObject.value(QString("TimerOptions"));
    QJsonObject timerOptionsKey = timerOptions.toObject();
    QJsonValue otherOptions = jsonObject.value(QString("OtherOptions"));
    QJsonObject otherOptionsKey = otherOptions.toObject();
    QString multiUrlOption = otherOptionsKey["MULTI_URL_OPTION"].toString();

    proxyRotateInterval = timerOptionsKey["ProxyRotate"].toInt();
    QStringList tempList1;
    QStringList tempList2;  
    isProxyEmpty = false;
    canProxyCounterIncrement = false;
    isMultiURLSelected = false;

    if(multiUrlOption == "1_URL_SELECTED")
    {
        isMultiURLSelected = false;
    }
    if(multiUrlOption == "MULTI_URL_SELECTED")
    {
        isMultiURLSelected = true;
    }


    if(!proxyServersArray.isEmpty())
    {
        /*******
         If proxy servers are greater or equal to 4, and multiurl is selected
         insert proxies into 4 list
         *****/
        if(proxyServersArray.size() >= 4  && isMultiURLSelected == true)
        {
            /******Create 2 proxy list*****/
              for(int i=0; i < proxyServersArray.size(); i++)
              {
                  if(i%2==0)
                  {
                      tempList1<< proxyServersArray[i].toString();
                  }else
                  {
                      tempList2 << proxyServersArray[i].toString();
                  }
              }

              /******Create 2 more proxy list*****/
              for(int i=0; i < tempList1.size(); i++)
              {
                  if(i%2==0)

                  {
                      proxyList3 << tempList1.value(i);
                  }else
                  {
                      proxyList4 << tempList1.value(i);
                  }
              }

              /******Create 2 more unique proxy list by overwriting proxylist 1 and 2*****/
              for(int i=0; i < tempList2.size(); i++)
              {
                  if(i%2==0)

                  {
                      proxyList1 << tempList2.value(i);
                  }else
                  {
                      proxyList2 << tempList2.value(i);
                  }
              }
        }// end if proxy array size is greater than 4


        /*******
         If proxy servers are less than 4, and multiurl is not selected
         If proxy servers are greater or equal to 4, and multiurl is not selected
         insert proxies into 1 list
         *****/
        if((proxyServersArray.size() < 4 && isMultiURLSelected == false) ||
         (proxyServersArray.size()  >=4 && isMultiURLSelected == false))
        {
            for(int i=0; i < proxyServersArray.size(); i++)
            {
                proxyList1 <<  proxyServersArray[i].toString();
            }
        }// end if proxy array size is less than 4


    }// end if proxyServersArray is not empty



}


// Rotates proxy with a specified interval, and rotate due to a httpStatus Code
void WebView::proxyListRotater(QStringList proxyServersList,QWebPage *page)
{



    if(!proxyServersArray.isEmpty())
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
                    *proxies = "";
                }

                // if proxies contained in qlist are not empty, and we can keep incrementing,
                // our proxies are good to use/rotate
                if (isProxyEmpty == false && canProxyCounterIncrement == true)
                {
                    if(!proxyServersList.value(*proxyServerCounterPtr).isEmpty())
                    {
                        *proxies = proxyServersList.value(*proxyServerCounterPtr);

                    }

                }

            }

            // if workerCounter is greater than *proxyRotateIntervalPtr/ amount of http request before proxy rotates
            // or if a httpStatusError occured
            if (*workerCounterPtr >= proxyRotateInterval)
            {
                // restart workerCounter
                *workerCounterPtr = 0;
                // increment proxyServerPtr to go through each proxyServer index every interval
                (*proxyServerCounterPtr) += 1;
            }
            (*workerCounterPtr) += 1;

            if(!proxyServersList.isEmpty())
            {
                QNetworkProxy networkProxy;
               // networkProxy.setType(QNetworkProxy::Socks5Proxy);
                networkProxy.setUser("username");
                networkProxy.setPassword("password");
                networkProxy.setType(QNetworkProxy::HttpProxy);
                QUrl proxy("http://" + *proxies);
                networkProxy.setHostName(proxy.host());
                networkProxy.setPort(static_cast<quint16>(proxy.port()));
                page->networkAccessManager()->setProxy(networkProxy);
                emit emitWebViewLog(QString("Using Proxy: ") + QString(proxy.host()));
                qDebug() << "List --> " << proxyServersList;

                qDebug() << "Rotating Proxy--> " << *proxies;

               if(!proxy.host().isEmpty())
               {
                   qDebug() << "Proxy host --> "<<proxy.host();
               }
               if(proxy.port() != -1)
               {
                   qDebug() << "Proxy port --> "<<proxy.port();
               }
            }

    }// end of if proxy is empty

}

void WebView::view1Page(QWebPage *page)
{

        if(!proxyList1.isEmpty())
        {
             proxyListRotater(proxyList1,page);
        }

    connect(this, &QWebView::loadFinished, [this,page](bool success) {

        connect(page->networkAccessManager(),SIGNAL(finished(QNetworkReply * )),
                             this,SLOT(view1HttpResponseFinished(QNetworkReply *)));

        if (!success)
        {
           // qDebug() << "Could not load page Multi Instance 1";
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
    QUrl url(reply->url());
    httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    emit emitWebViewLog(QString("HTTP Status: ") + QString(reply->errorString())  +" "+ QString(url.host()+url.path()+url.query()));

    if(httpStatusCode <200)
    {
        viewPage1ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode)  +" "+ QString(url.host()+url.path()+url.query()));
    }

    if(httpStatusCode ==200)
    {
        viewPage1ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(url.host()+url.path()+url.query()));
    }
    if(httpStatusCode> 200)
    {

        viewPage1ProxyRotate = true;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode)  +" "+ QString(url.host()+url.path()+url.query()));
    }
    switch(reply->error())
    {
    case QNetworkReply::NoError:
         // No error
         emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(url.host()+url.path()+url.query()));
         return;
    case QNetworkReply::ContentNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: Content Not Found"));
        break;
    case QNetworkReply::ConnectionRefusedError:
        emit emitWebViewLog(QString("HTTP Status: The remote server refused the connection (the server is not accepting requests")
                             +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::TimeoutError:
        emit emitWebViewLog(QString("HTTP Status: The connection to the remote server timed out")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::HostNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: The host was not found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyConnectionClosedError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Connection Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyConnectionRefusedError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Refused Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Not Found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyTimeoutError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Timedout Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    default:
        emit emitWebViewLog(QString("HTTP Status: Default Error: ") + QString(reply->errorString())  +" "+ QString(url.host()+url.path()+url.query()));
    }




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
    if(!proxyList2.isEmpty())
    {
         proxyListRotater(proxyList2,page);
    }


    connect(this, &QWebView::loadFinished, [this,page](bool success) {

        connect(page->networkAccessManager(),SIGNAL(finished(QNetworkReply *)),
                this,SLOT(view2HttpResponseFinished(QNetworkReply *)));
        if (!success)
        {
            //qDebug() << "Could not load page view 2";
        }
       if(success) {

           // qDebug() << "Page  loaded view 2";


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
    QUrl url(reply->url());
    httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    emit emitWebViewLog(QString("HTTP Status: ") + QString(reply->errorString())  +" "+ QString(url.host()+url.path()+url.query()));

    if(httpStatusCode <200)
    {
        viewPage2ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode)  +" "+ QString(url.host()+url.path()+url.query()));
    }

    if(httpStatusCode ==200)
    {
        viewPage2ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(url.host()+url.path()+url.query()));
    }
    if(httpStatusCode> 200)
    {
        viewPage2ProxyRotate = true;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode)  +" "+ QString(url.host()+url.path()+url.query()));
    }
    switch(reply->error())
    {
    case QNetworkReply::NoError:
         // No error
         emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(url.host()+url.path()+url.query()));
         return;
    case QNetworkReply::ContentNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: Content Not Found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ConnectionRefusedError:
        emit emitWebViewLog(QString("HTTP Status: The remote server refused the connection (the server is not accepting requests")
                             +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::TimeoutError:
        emit emitWebViewLog(QString("HTTP Status: The connection to the remote server timed out")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::HostNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: The host was not found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyConnectionClosedError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Connection Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyConnectionRefusedError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Refused Error")   +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Not Found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyTimeoutError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Timedout Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    default:
        emit emitWebViewLog(QString("HTTP Status: Default Error: ") + QString(reply->errorString())  +" "+ QString(url.host()+url.path()+url.query()));
    }

}


void WebView::view3Page(QWebPage *page){
    if(!proxyList3.isEmpty())
    {
         proxyListRotater(proxyList3,page);
    }

    connect(this, &QWebView::loadFinished, [this,page](bool success) {

    connect(page->networkAccessManager(),SIGNAL(finished(QNetworkReply *)),
       this,SLOT(view3HttpResponseFinished(QNetworkReply *)));



        if (!success)
        {
           // qDebug() << "Could not load page view 3";
        }
       if(success) {
          //  qDebug() << "Page loaded view 3";

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
    QUrl url(reply->url());
    httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    emit emitWebViewLog(QString("HTTP Status: ") + QString(reply->errorString())  +" "+ QString(url.host()+url.path()+url.query()));

    if(httpStatusCode <200)
    {
        viewPage3ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode)  +" "+ QString(url.host()+url.path()+url.query()));
    }

    if(httpStatusCode ==200)
    {
        viewPage3ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(url.host()+url.path()+url.query()));
    }
    if(httpStatusCode> 200)
    {
        viewPage3ProxyRotate = true;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode)  +" "+ QString(url.host()+url.path()+url.query()));
    }
    switch(reply->error())
    {
    case QNetworkReply::NoError:
         // No error
         emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(url.host()+url.path()+url.query()));
         return;
    case QNetworkReply::ContentNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: Content Not Found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ConnectionRefusedError:
        emit emitWebViewLog(QString("HTTP Status: The remote server refused the connection (the server is not accepting requests")
                             +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::TimeoutError:
        emit emitWebViewLog(QString("HTTP Status: The connection to the remote server timed out")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::HostNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: The host was not found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyConnectionClosedError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Connection Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyConnectionRefusedError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Refused Error") +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Not Found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyTimeoutError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Timedout Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    default:
        emit emitWebViewLog(QString("HTTP Status: Default Error: ") + QString(reply->errorString())  +" "+ QString(url.host()+url.path()+url.query()));
    }

}

void WebView::view4Page(QWebPage *page){


    if(!proxyList4.isEmpty())
    {
         proxyListRotater(proxyList4,page);
    }
    connect(this, &QWebView::loadFinished, [this,page](bool success) {

        connect(page->networkAccessManager(),SIGNAL(finished(QNetworkReply *)),
                this,SLOT(view4HttpResponseFinished(QNetworkReply *)));

        if (!success)
        {
          //  qDebug() << "Could not load page view 4";
        }
       if(success) {
         //   qDebug() << "Page loaded view 4";

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
    QUrl url(reply->url());
    emit emitWebViewLog(QString("HTTP Status: ") + QString(reply->errorString())
                        +" "+ QString(url.host()+url.path()+url.query()));

    if(httpStatusCode <200)
    {

        viewPage4ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode)
                            +" "+ QString(url.host()+url.path()+url.query()));
    }

    if(httpStatusCode ==200)
    {
        viewPage4ProxyRotate = false;
        emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(url.host()+url.path()+url.query()));
    }
    if(httpStatusCode> 200)
    {
        viewPage4ProxyRotate = true;
        emit emitWebViewLog(QString("HTTP Status: ") + QString::number(httpStatusCode)  +" "+ QString(url.host()+url.path()+url.query()));
    }
    switch(reply->error())
    {
    case QNetworkReply::NoError:
         // No error
         emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(url.host()+url.path()+url.query()));
         return;
    case QNetworkReply::ContentNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: Content Not Found"));
        break;
    case QNetworkReply::ConnectionRefusedError:
        emit emitWebViewLog(QString("HTTP Status: The remote server refused the connection (the server is not accepting requests")
                             +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::TimeoutError:
        emit emitWebViewLog(QString("HTTP Status: The connection to the remote server timed out")
                             +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::HostNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: The host was not found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyConnectionClosedError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Connection Error"));
        break;
    case QNetworkReply::ProxyConnectionRefusedError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Refused Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyNotFoundError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Not Found")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    case QNetworkReply::ProxyTimeoutError:
        emit emitWebViewLog(QString("HTTP Status: Proxy Timedout Error")  +" "+ QString(url.host()+url.path()+url.query()));
        break;
    default:
        emit emitWebViewLog(QString("HTTP Status: Default Error: ") + QString(reply->errorString())  +" "+ QString(url.host()+url.path()+url.query()));
    }

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
  //  mydb.removeDatabase(QSqlDatabase::database().connectionName());
   // mydb.close();
}

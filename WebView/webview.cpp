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

}

WebView::~WebView(){
   // delete request;
    emailListFile->close();
    delete emailListFile;
    delete emailList;
    delete readEmailFile;
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

void WebView::view1Page(QWebPage *page)
{

    connect(this, &QWebView::loadFinished, [this,page](bool success) {
        if (!success)
        {
            qDebug() << "Could not load page view 1";
        }
       if(success) {
            qDebug() << "Page  loaded view 1";

            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");
            QRegularExpressionMatchIterator i = re.globalMatch( page->mainFrame()->toPlainText());
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;
            connOpen();
            QSqlQuery qry;
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (!match.captured(0).isEmpty())
                    word = match.captured(0);
                if (!word.isEmpty()) {
                    words << word;
                    if (!words.value(num).isEmpty())

                    {
                        if(!qry.exec("insert into crawleremails values('"+words.value(num)+"')"))
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

void WebView::view2Page(QWebPage *page)
{

    connect(this, &QWebView::loadFinished, [this,page](bool success) {
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
            connOpen();
            QSqlQuery qry;
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (!match.captured(0).isEmpty())
                    word = match.captured(0);
                if (!word.isEmpty()) {
                    words << word;
                    if (!words.value(num).isEmpty())

                    {
                        if(!qry.exec("insert into crawleremails values('"+words.value(num)+"')"))
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

void WebView::view3Page(QWebPage *page){


    connect(this, &QWebView::loadFinished, [this,page](bool success) {
        if (!success)
        {
            qDebug() << "Could not load page view 3";
        }
       if(success) {
            qDebug() << "Page  loaded view 3";

            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");
            QRegularExpressionMatchIterator i = re.globalMatch( page->mainFrame()->toPlainText());
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;
            connOpen();
            QSqlQuery qry;
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (!match.captured(0).isEmpty())
                    word = match.captured(0);
                if (!word.isEmpty()) {
                    words << word;
                    if (!words.value(num).isEmpty())

                    {
                        if(!qry.exec("insert into crawleremails values('"+words.value(num)+"')"))
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

void WebView::view4Page(QWebPage *page){

    connect(this, &QWebView::loadFinished, [this,page](bool success) {
        if (!success)
        {
            qDebug() << "Could not load page view 4";
        }
       if(success) {
            qDebug() << "Page  loaded view 5";

            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");
            QRegularExpressionMatchIterator i = re.globalMatch( page->mainFrame()->toPlainText());
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;
            connOpen();
            QSqlQuery qry;
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (!match.captured(0).isEmpty())
                    word = match.captured(0);
                if (!word.isEmpty()) {
                    words << word;
                    if (!words.value(num).isEmpty())

                    {
                        if(!qry.exec("insert into crawleremails values('"+words.value(num)+"')"))
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
    mydb.setDatabaseName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emails.db");
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

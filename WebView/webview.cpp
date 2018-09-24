#include "webview.h"
#include <QPaintEvent>
#include <QWebFrame>

WebView::WebView(QWidget *parent)
    : QWebView(parent)

{


    connOpen();
    workerCounterNum =0;
    workerCounterPtr = &workerCounterNum;

    proxyServerCounterNum =0;
    proxyServerCounterPtr =&proxyServerCounterNum;

    proxies = new QString();

    httpStatusNum = 0;
    httpStatusPtr =&httpStatusNum;
    connect(this,&WebView::emitWebViewLog,&this->worker,&Worker::receiverWebViewLog);

    initSettings();
    replyUrl = new QString();
}

WebView::~WebView(){

    delete replyUrl;
    delete proxies;
    connClose();
    harvestStatus("HARVEST_ABORTED");

}

void WebView::harvestStatus(QString status)
{
    QFile harvestStatusFile;
    QJsonDocument harvestStatusDocument;
    QJsonObject  harvestStatusObject;
    harvestStatusObject.insert("HarvestStatus",QJsonValue(status).toString());
    QString fileName= getRelativePath("harvesterstatus.json");

    harvestStatusFile.setFileName(fileName);
    if(!harvestStatusFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Error opening  harvestStatusFile in webview..... " << harvestStatusFile.errorString();
    }
    harvestStatusDocument.setObject(harvestStatusObject);
    harvestStatusFile.write(harvestStatusDocument.toJson());
    harvestStatusFile.close();

}


void WebView::paintEvent(QPaintEvent *event)
{

}


void WebView::initSettings()
{
    QList <QString> *proxyServersJson;
    proxyServersJson = new QList<QString>();
    QString val;
    QFile file;
    QString fileName= getRelativePath("params.json");

    file.setFileName(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening initProxyListSettings file in webview....." << file.errorString();
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
void WebView::proxyListRotater(QStringList proxyServersList)
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

//                QUrl proxy("http://" + *proxies);
//                networkProxy.setHostName(proxy.host());
//                networkProxy.setPort(static_cast<quint16>(proxy.port()));

//                emit emitWebViewLog(QString("Using Proxy: ") + QString(proxy.host()));
//                qDebug() << "List --> " << proxyServersList;

//                qDebug() << "Rotating Proxy--> " << *proxies;

//               if(!proxy.host().isEmpty())
//               {
//                   qDebug() << "Proxy host --> "<<proxy.host();
//               }
//               if(proxy.port() != -1)
//               {
//                   qDebug() << "Proxy port --> "<<proxy.port();
//               }
            }

    }// end of if proxy is empty

}

void WebView::view1Page(QString url)
{

        if(!proxyList1.isEmpty())
        {
             proxyListRotater(proxyList1);
        }
        CurlEasy *curl = new CurlEasy;
        std::string urlString = url.toStdString();

        curl->set(CURLOPT_URL, urlString.c_str());
        if (!proxies->isEmpty()) {
            curl->set(CURLOPT_PROXY, proxies->toStdString().c_str());
        }
        if (proxies->isEmpty()) {
            curl->set(CURLOPT_PROXY, NULL);
        }
        curl->set(CURLOPT_FOLLOWLOCATION, 1L); // Tells libcurl to follow HTTP 3xx redirects
        curl->set(CURLOPT_SSL_VERIFYPEER, FALSE);
        curl->perform();

        curl->setWriteFunction([this](char *data, size_t size)->size_t {


                //If response is not 200, obviously we wont get any emails,
                // but were only going to parse emails, only
                // so this means, we only get emails if::
                //1 reponse code is 200

                // qDebug() << "Data from google.com: " << QByteArray(data, static_cast<int>(size));
                QByteArray response(data, static_cast<int>(size));
                QString responseString = QString(response);
                QString plainText = QTextDocumentFragment::fromHtml(responseString).toPlainText();


                QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");

                QRegularExpressionMatchIterator i = re.globalMatch(plainText);
                QRegularExpressionMatch match;
                QStringList words;
                QString word;
                int num = 0;

                mydb = QSqlDatabase::addDatabase("QSQLITE");
                QString dbFileName = getRelativePath("emails.db");
                mydb.setDatabaseName(dbFileName);
                if(!mydb.open())
               {
                   qDebug() << "Error connecting to database";

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
                           if(!qry.exec("insert into crawleremails(email) values('"+words.value(num)+"')"))
                           {
                               qDebug() << "Error inserting crawleremails into database:: " <<
                                           qry.lastError().text();


                           }else
                           {
                               qDebug() <<  "Successfully inserted crawler emails-->" << words.value(num);
                           }
                           if(!qry.exec("insert into crawlermaileremails(maileremails) values('"+words.value(num)+"')"))
                           {
                               qDebug() << "Error inserting crawlermaileremails into database:: " <<
                                           qry.lastError().text();


                           }else
                           {
                              // qDebug() <<  "Successfully inserted crawlermaileremails-->" << words.value(num);
                           }

                       }
                       num++;

                   } // end words !empty

               } // end while
            // connClose();
               mydb.removeDatabase(QSqlDatabase::database().connectionName());
               mydb.close();


                return size;
        });// end setWriteFunction


        connect(curl, &CurlEasy::done, [curl, this](CURLcode result) {
                long httpResponseCode = curl->get<long>(CURLINFO_RESPONSE_CODE);

                QString effectiveUrl = curl->get<const char*>(CURLINFO_EFFECTIVE_URL);

                /*********HTTP CODE*******/
                // Log errors,or show message to user if response code is not 200
                if (httpResponseCode == 200 && result == CURLE_OK)
                {

                    qDebug() << " Thread 1 HTTP  Code-->" << httpResponseCode;
                    qDebug() << "Result--->" << result;

                    harvestStatus("HARVEST_SUCCESSFULLY_SCRAPING");
                    emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(effectiveUrl));

                }

                if (httpResponseCode == 503) {

                    qDebug() << "503 ERROR CODE ";
                    harvestStatus("HARVEST_HTTP_STATUS_ERROR");
                    emit emitWebViewLog(QString("HTTP Status: 503 Request Error")  +" "+ QString(effectiveUrl));

                }

                if (result != CURLE_OK)
                {

                    switch (result)
                    {
                    case 5: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                        emit emitWebViewLog( QString("Couldn't resolve proxy. The given proxy host could not be resolved." ) +" "+ QString(effectiveUrl));
                        break;
                    case 7: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;


                        emit emitWebViewLog( QString(" Failed to connect() to host or proxy.") +" "+ QString(effectiveUrl));
                        break;
                    case 35: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                        emit emitWebViewLog( QString("A problem occurred somewhere in the SSL/TLS handshake." ) +" "+ QString(effectiveUrl));
                        break;
                    case 56: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                        emit emitWebViewLog( QString("Failure with receiving network data." ) +" "+ QString(effectiveUrl));
                        break;


                    default: qDebug() << "Default Switch Statement Curl Code--> " << result;

                    }
                    /**************
                    *checks curl erros codes
                    *
                    * 5 -- Couldn't resolve proxy. The given proxy host could not be resolved.
                    * 7 -- Failed to connect() to host or proxy.
                    *
                    *********/
                    if (result == 5 || result == 7 || result == 35)
                    {
                        emit emitWebViewLog(QString("Proxy Error ")  +" "+ QString(effectiveUrl));

                    }

               }



          }); // end of lambda
        connect(curl, SIGNAL(done(CURLcode)), curl, SLOT(deleteLater()));


}


void WebView::view2Page(QString url)
{


    if(!proxyList2.isEmpty())
    {
         proxyListRotater(proxyList2);
    }
    CurlEasy *curl = new CurlEasy;
    std::string urlString = url.toStdString();

    curl->set(CURLOPT_URL, urlString.c_str());
    if (!proxies->isEmpty()) {
        curl->set(CURLOPT_PROXY, proxies->toStdString().c_str());
    }
    if (proxies->isEmpty()) {
        curl->set(CURLOPT_PROXY, NULL);
    }
    curl->set(CURLOPT_FOLLOWLOCATION, 1L); // Tells libcurl to follow HTTP 3xx redirects
    curl->set(CURLOPT_SSL_VERIFYPEER, FALSE);
    curl->perform();

    curl->setWriteFunction([this](char *data, size_t size)->size_t {


            //If response is not 200, obviously we wont get any emails,
            // but were only going to parse emails, only
            // so this means, we only get emails if::
            //1 reponse code is 200

            // qDebug() << "Data from google.com: " << QByteArray(data, static_cast<int>(size));
            QByteArray response(data, static_cast<int>(size));
            QString responseString = QString(response);
            QString plainText = QTextDocumentFragment::fromHtml(responseString).toPlainText();


            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");

            QRegularExpressionMatchIterator i = re.globalMatch(plainText);
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;

            mydb = QSqlDatabase::addDatabase("QSQLITE");
            QString dbFileName = getRelativePath("emails.db");
            mydb.setDatabaseName(dbFileName);
            if(!mydb.open())
           {
               qDebug() << "Error connecting to database";

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
                       if(!qry.exec("insert into crawleremails(email) values('"+words.value(num)+"')"))
                       {
                           qDebug() << "Error inserting crawleremails into database:: " <<
                                       qry.lastError().text();


                       }else
                       {
                           qDebug() <<  "Successfully inserted crawler emails-->" << words.value(num);
                       }
                       if(!qry.exec("insert into crawlermaileremails(maileremails) values('"+words.value(num)+"')"))
                       {
                           qDebug() << "Error inserting crawlermaileremails into database:: " <<
                                       qry.lastError().text();


                       }else
                       {
                          // qDebug() <<  "Successfully inserted crawlermaileremails-->" << words.value(num);
                       }

                   }
                   num++;

               } // end words !empty

           } // end while
        // connClose();
           mydb.removeDatabase(QSqlDatabase::database().connectionName());
           mydb.close();


            return size;
    });// end setWriteFunction


    connect(curl, &CurlEasy::done, [curl, this](CURLcode result) {
            long httpResponseCode = curl->get<long>(CURLINFO_RESPONSE_CODE);

            QString effectiveUrl = curl->get<const char*>(CURLINFO_EFFECTIVE_URL);

            /*********HTTP CODE*******/
            // Log errors,or show message to user if response code is not 200
            if (httpResponseCode == 200 && result == CURLE_OK)
            {

                qDebug() << " Thread 2 HTTP  Code-->" << httpResponseCode;
                qDebug() << "Result--->" << result;

                harvestStatus("HARVEST_SUCCESSFULLY_SCRAPING");
                emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(effectiveUrl));

            }

            if (httpResponseCode == 503) {

                qDebug() << "503 ERROR CODE ";
                harvestStatus("HARVEST_HTTP_STATUS_ERROR");
                emit emitWebViewLog(QString("HTTP Status: 503 Request Error")  +" "+ QString(effectiveUrl));

            }

            if (result != CURLE_OK)
            {

                switch (result)
                {
                case 5: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("Couldn't resolve proxy. The given proxy host could not be resolved." ) +" "+ QString(effectiveUrl));
                    break;
                case 7: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;


                    emit emitWebViewLog( QString(" Failed to connect() to host or proxy.") +" "+ QString(effectiveUrl));
                    break;
                case 35: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("A problem occurred somewhere in the SSL/TLS handshake." ) +" "+ QString(effectiveUrl));
                    break;
                case 56: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("Failure with receiving network data." ) +" "+ QString(effectiveUrl));
                    break;


                default: qDebug() << "Default Switch Statement Curl Code--> " << result;

                }
                /**************
                *checks curl erros codes
                *
                * 5 -- Couldn't resolve proxy. The given proxy host could not be resolved.
                * 7 -- Failed to connect() to host or proxy.
                *
                *********/
                if (result == 5 || result == 7 || result == 35)
                {
                    emit emitWebViewLog(QString("Proxy Error ")  +" "+ QString(effectiveUrl));

                }

           }



      }); // end of lambda
    connect(curl, SIGNAL(done(CURLcode)), curl, SLOT(deleteLater()));


}


void WebView::view3Page(QString url)
{


    if(!proxyList3.isEmpty())
    {
         proxyListRotater(proxyList3);
    }
    CurlEasy *curl = new CurlEasy;
    std::string urlString = url.toStdString();

    curl->set(CURLOPT_URL, urlString.c_str());
    if (!proxies->isEmpty()) {
        curl->set(CURLOPT_PROXY, proxies->toStdString().c_str());
    }
    if (proxies->isEmpty()) {
        curl->set(CURLOPT_PROXY, NULL);
    }
    curl->set(CURLOPT_FOLLOWLOCATION, 1L); // Tells libcurl to follow HTTP 3xx redirects
    curl->set(CURLOPT_SSL_VERIFYPEER, FALSE);
    curl->perform();

    curl->setWriteFunction([this](char *data, size_t size)->size_t {


            //If response is not 200, obviously we wont get any emails,
            // but were only going to parse emails, only
            // so this means, we only get emails if::
            //1 reponse code is 200

            // qDebug() << "Data from google.com: " << QByteArray(data, static_cast<int>(size));
            QByteArray response(data, static_cast<int>(size));
            QString responseString = QString(response);
            QString plainText = QTextDocumentFragment::fromHtml(responseString).toPlainText();


            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");

            QRegularExpressionMatchIterator i = re.globalMatch(plainText);
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;

            mydb = QSqlDatabase::addDatabase("QSQLITE");
            QString dbFileName = getRelativePath("emails.db");
            mydb.setDatabaseName(dbFileName);
            if(!mydb.open())
           {
               qDebug() << "Error connecting to database";

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
                       if(!qry.exec("insert into crawleremails(email) values('"+words.value(num)+"')"))
                       {
                           qDebug() << "Error inserting crawleremails into database:: " <<
                                       qry.lastError().text();


                       }else
                       {
                           qDebug() <<  "Successfully inserted crawler emails-->" << words.value(num);
                       }
                       if(!qry.exec("insert into crawlermaileremails(maileremails) values('"+words.value(num)+"')"))
                       {
                           qDebug() << "Error inserting crawlermaileremails into database:: " <<
                                       qry.lastError().text();


                       }else
                       {
                          // qDebug() <<  "Successfully inserted crawlermaileremails-->" << words.value(num);
                       }

                   }
                   num++;

               } // end words !empty

           } // end while
        // connClose();
           mydb.removeDatabase(QSqlDatabase::database().connectionName());
           mydb.close();


            return size;
    });// end setWriteFunction


    connect(curl, &CurlEasy::done, [curl, this](CURLcode result) {
            long httpResponseCode = curl->get<long>(CURLINFO_RESPONSE_CODE);

            QString effectiveUrl = curl->get<const char*>(CURLINFO_EFFECTIVE_URL);

            /*********HTTP CODE*******/
            // Log errors,or show message to user if response code is not 200
            if (httpResponseCode == 200 && result == CURLE_OK)
            {

                qDebug() << " Thread 3 HTTP  Code-->" << httpResponseCode;
                qDebug() << "Result--->" << result;

                harvestStatus("HARVEST_SUCCESSFULLY_SCRAPING");
                emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(effectiveUrl));

            }

            if (httpResponseCode == 503) {

                qDebug() << "503 ERROR CODE ";
                harvestStatus("HARVEST_HTTP_STATUS_ERROR");
                emit emitWebViewLog(QString("HTTP Status: 503 Request Error")  +" "+ QString(effectiveUrl));

            }

            if (result != CURLE_OK)
            {

                switch (result)
                {
                case 5: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("Couldn't resolve proxy. The given proxy host could not be resolved." ) +" "+ QString(effectiveUrl));
                    break;
                case 7: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;


                    emit emitWebViewLog( QString(" Failed to connect() to host or proxy.") +" "+ QString(effectiveUrl));
                    break;
                case 35: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("A problem occurred somewhere in the SSL/TLS handshake." ) +" "+ QString(effectiveUrl));
                    break;
                case 56: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("Failure with receiving network data." ) +" "+ QString(effectiveUrl));
                    break;


                default: qDebug() << "Default Switch Statement Curl Code--> " << result;

                }
                /**************
                *checks curl erros codes
                *
                * 5 -- Couldn't resolve proxy. The given proxy host could not be resolved.
                * 7 -- Failed to connect() to host or proxy.
                *
                *********/
                if (result == 5 || result == 7 || result == 35)
                {
                    emit emitWebViewLog(QString("Proxy Error ")  +" "+ QString(effectiveUrl));

                }

           }



      }); // end of lambda
    connect(curl, SIGNAL(done(CURLcode)), curl, SLOT(deleteLater()));


}

void WebView::view4Page(QString url)
{


    if(!proxyList4.isEmpty())
    {
         proxyListRotater(proxyList4);
    }
    CurlEasy *curl = new CurlEasy;
    std::string urlString = url.toStdString();

    curl->set(CURLOPT_URL, urlString.c_str());
    if (!proxies->isEmpty()) {
        curl->set(CURLOPT_PROXY, proxies->toStdString().c_str());
    }
    if (proxies->isEmpty()) {
        curl->set(CURLOPT_PROXY, NULL);
    }
    curl->set(CURLOPT_FOLLOWLOCATION, 1L); // Tells libcurl to follow HTTP 3xx redirects
    curl->set(CURLOPT_SSL_VERIFYPEER, FALSE);
    curl->perform();

    curl->setWriteFunction([this](char *data, size_t size)->size_t {


            //If response is not 200, obviously we wont get any emails,
            // but were only going to parse emails, only
            // so this means, we only get emails if::
            //1 reponse code is 200

            // qDebug() << "Data from google.com: " << QByteArray(data, static_cast<int>(size));
            QByteArray response(data, static_cast<int>(size));
            QString responseString = QString(response);
            QString plainText = QTextDocumentFragment::fromHtml(responseString).toPlainText();


            QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");

            QRegularExpressionMatchIterator i = re.globalMatch(plainText);
            QRegularExpressionMatch match;
            QStringList words;
            QString word;
            int num = 0;

            mydb = QSqlDatabase::addDatabase("QSQLITE");
            QString dbFileName = getRelativePath("emails.db");
            mydb.setDatabaseName(dbFileName);
            if(!mydb.open())
           {
               qDebug() << "Error connecting to database";

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
                       if(!qry.exec("insert into crawleremails(email) values('"+words.value(num)+"')"))
                       {
                           qDebug() << "Error inserting crawleremails into database:: " <<
                                       qry.lastError().text();


                       }else
                       {
                           qDebug() <<  "Successfully inserted crawler emails-->" << words.value(num);
                       }
                       if(!qry.exec("insert into crawlermaileremails(maileremails) values('"+words.value(num)+"')"))
                       {
                           qDebug() << "Error inserting crawlermaileremails into database:: " <<
                                       qry.lastError().text();


                       }else
                       {
                          // qDebug() <<  "Successfully inserted crawlermaileremails-->" << words.value(num);
                       }

                   }
                   num++;

               } // end words !empty

           } // end while
        // connClose();
           mydb.removeDatabase(QSqlDatabase::database().connectionName());
           mydb.close();


            return size;
    });// end setWriteFunction


    connect(curl, &CurlEasy::done, [curl, this](CURLcode result) {
            long httpResponseCode = curl->get<long>(CURLINFO_RESPONSE_CODE);

            QString effectiveUrl = curl->get<const char*>(CURLINFO_EFFECTIVE_URL);

            /*********HTTP CODE*******/
            // Log errors,or show message to user if response code is not 200
            if (httpResponseCode == 200 && result == CURLE_OK)
            {

                qDebug() << " Thread 4 HTTP  Code-->" << httpResponseCode;
                qDebug() << "Result--->" << result;

                harvestStatus("HARVEST_SUCCESSFULLY_SCRAPING");
                emit emitWebViewLog(QString("HTTP Status: 200 Request has succeeded")  +" "+ QString(effectiveUrl));

            }

            if (httpResponseCode == 503) {

                qDebug() << "503 ERROR CODE ";
                harvestStatus("HARVEST_HTTP_STATUS_ERROR");
                emit emitWebViewLog(QString("HTTP Status: 503 Request Error")  +" "+ QString(effectiveUrl));

            }

            if (result != CURLE_OK)
            {

                switch (result)
                {
                case 5: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("Couldn't resolve proxy. The given proxy host could not be resolved." ) +" "+ QString(effectiveUrl));
                    break;
                case 7: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;


                    emit emitWebViewLog( QString(" Failed to connect() to host or proxy.") +" "+ QString(effectiveUrl));
                    break;
                case 35: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("A problem occurred somewhere in the SSL/TLS handshake." ) +" "+ QString(effectiveUrl));
                    break;
                case 56: qDebug() << "Curl code-> " << result << " Message->" << curl->errbuf;
                    emit emitWebViewLog( QString("Failure with receiving network data." ) +" "+ QString(effectiveUrl));
                    break;

                default: qDebug() << "Default Switch Statement Curl Code--> " << result;

                }
                /**************
                *checks curl erros codes
                *
                * 5 -- Couldn't resolve proxy. The given proxy host could not be resolved.
                * 7 -- Failed to connect() to host or proxy.
                *
                *********/
                if (result == 5 || result == 7 || result == 35)
                {
                    emit emitWebViewLog(QString("Proxy Error ")  +" "+ QString(effectiveUrl));

                }

           }



      }); // end of lambda
    connect(curl, SIGNAL(done(CURLcode)), curl, SLOT(deleteLater()));




}






void WebView::connOpen()
{
    mydb = QSqlDatabase::addDatabase("QSQLITE");
    QString dbFileName = getRelativePath("emails.db");
    mydb.setDatabaseName(dbFileName);
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
    mydb.removeDatabase(QSqlDatabase::database().connectionName());
    mydb.close();

}

QString WebView::getRelativePath(QString fileName)
{
//    QDir tempCurrDir  = QDir::current();
//    tempCurrDir.cdUp();
//    QString root = tempCurrDir.path();
//    return root + "/" +fileName;
//C:/Users/<USER>/AppData/Local/<APPNAME>
    QString userName =  qgetenv("USERNAME");
    QString crawlerAppDataResourcesDir =  QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QString dir = "C:/Users/" + userName+"/AppData/Local/BeatCrawler/resources/";
    QDir resourcesDir(dir);
    QStringList args;
    if(RESOURCES_DIR == "RELEASE")
    {


        if(resourcesDir.exists())
        {
            return resourcesDir.path() + "/" +fileName;
        }

    }else
    {
        return "C:/Users/ace/Documents/QT_Projects/WebView/build-WebView-Desktop_Qt_5_9_4_MSVC2015_32bit2-Release/release/" + fileName;

    }

}

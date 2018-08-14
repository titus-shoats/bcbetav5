
/*
Copyright 2018 Titus Shoats

*/

#include "worker.h"
std::string buffer;
QStringList parsedEmailList1;
QStringList removeDupelicateEmailList;

Worker::Worker(QObject *parent) :
    QObject(parent)
{

    proxies = new QString();
    workerCounterNum = 0;
    workerCounterPtr = reinterpret_cast<int *>(&workerCounterNum);

    proxyServerCounterNum = 0;
    proxyServerCounterPtr = reinterpret_cast<int *>(&proxyServerCounterNum);

    harvesterTimerNum = 0;
    harvesterTimerPtr = reinterpret_cast<int *>(&harvesterTimerNum);
    httpRequestList = new QList<QStringList>();

    proxyRotateIntervalNum = 0;
    proxyRotateIntervalPtr = reinterpret_cast<int *>(&proxyRotateIntervalNum);

    curlHTTPRequestCounterNum = 0;
    curlHTTPRequestCounterPtr = reinterpret_cast<int *>(&curlHTTPRequestCounterNum);

    curlMultiProcessCounterNum = 0;
    curlMultiProcessPtrCounter = reinterpret_cast<int *>(&curlMultiProcessCounterNum);

    multi1CurrentKeywordPtrNum = 0;
    multi1CurrentKeywordPtr = &multi1CurrentKeywordPtrNum;

    multi2CurrentKeywordPtrNum = 0;
    multi2CurrentKeywordPtr = &multi2CurrentKeywordPtrNum;

    multi3CurrentKeywordPtrNum = 0;
    multi3CurrentKeywordPtr = &multi3CurrentKeywordPtrNum;

    multi4CurrentKeywordPtrNum = 0;
    multi4CurrentKeywordPtr = &multi4CurrentKeywordPtrNum;


     stopHarvestLoop = false;
    multiURLOptionString = "";
    multiURLOption = reinterpret_cast<QString *>(&multiURLOptionString);

    multiOptionOneURLString = "";
    multiOptionOneURL = reinterpret_cast<QString *>(&multiOptionOneURLString);



    proxyServers = new QList <QString>();
    fileList = new QStringList();

    fileListNum = 0;
    fileListPtr = &fileListNum;
    currentKeyword = "";
    currentKeywordPtr = &currentKeyword;
    curlSingleProcessCounterNum = 0;
    curlSingleProcessPtrCounter = &curlSingleProcessCounterNum;
    searchEngineNum = 0;
    searchEngineNumPtr = &searchEngineNum;
    emailOptionsNum = 0;
    emailOptionsNumPtr = &emailOptionsNum;
    socialNetWorkNum = 0;
    socialNetWorkNumPtr = &socialNetWorkNum;
    searchEnginePaginationCounterNum = 0;
    searchEnginePaginationCounterPtr = &searchEnginePaginationCounterNum;
    deleteKeywordCheckBoxTimer = new QTimer();

    //connect(&this->myThread, &Thread::emitEmailList, this, &Worker::receiverEmails);
    //connect(&this->myThread1, &Thread1::emitEmailList, this, &Worker::receiverEmails);
    //connect(&this->myThread3, &Thread3::emitEmailList, this, &Worker::receiverEmails);

}

Worker::~Worker()
{
    delete proxies;
    delete httpRequestList;
    delete fileList;
    delete proxyServers;
    delete deleteKeywordCheckBoxTimer;
    delete multi1CurrentKeywordPtr;
    delete multi2CurrentKeywordPtr;
    delete multi3CurrentKeywordPtr;
    delete multi4CurrentKeywordPtr;
    delete fileListPtr;
    delete curlHTTPRequestCounterPtr;
    delete curlMultiProcessPtrCounter;
    delete curlSingleProcessPtrCounter;

}


void Worker::requestWork()
{
    //emit workRequested();
}

void Worker::abort() {}

void Worker::stop()
{
     stopHarvestLoop = true;
}


void Worker::receiverRemoveThreadEmailList()
{
    parsedEmailList1.clear();
    for (;;) {
        QEventLoop loop;
        QTimer::singleShot(5000, &loop, SLOT(quit()));
        loop.exec();
        if (parsedEmailList1.isEmpty()) {
            emit emitsenderEnableDeleteEmailCheckBox();
            break;
        }
    }
}


void Worker::receiverRemoveThreadFileList()
{
    fileList->clear();
    multi1KeywordList.clear();
    multi2KeywordList.clear();
    for (;;) {
        QEventLoop loop;
        QTimer::singleShot(5000, &loop, SLOT(quit()));
        loop.exec();
        /**********
        Emit signal if fileList is empty or both multi keyword list are empty
        ********/
        if (fileList->isEmpty()) {
            emit emitsenderEnableDeleteKeywordCheckBox();
            break;
        }
        if (multi1KeywordList.isEmpty() && multi2KeywordList.isEmpty()) {
            emit emitsenderEnableDeleteKeywordCheckBox();
            break;
        }
    }
}

void Worker::receiverReadFile(QString fileName, bool isMultiSelected)
{
    fileList->clear();
    multi1KeywordList.clear();
    multi2KeywordList.clear();
    QFile file(fileName);
    QFileInfo fi(file.fileName());
    QString fileExt = fi.completeSuffix();
    QString strings;
    QString str;

    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "ERROR OPENING FILE" << file.error();
    }


    QTextStream ts(&file);
    while (!ts.atEnd())
    {
        str = ts.readLine();
        *fileList << str;
        //qDebug() << *fileList;

    }

    if (isMultiSelected)
    {
        // loop through main keyword list to get two seperate list, if multi option is checked
        for (int i = 0; i < fileList->size(); i++)
        {
            // if j is a even int, put keywords into this list
            if (i % 2 == 0)
            {
                multi1KeywordList << fileList->value(i);
                //qDebug() << "LIST 1--->" << multi1KeywordList;
            }
            // else put odd keywords into a new list
            else
            {
                multi2KeywordList << fileList->value(i);
                //qDebug() << "LIST 2--->" << multi2KeywordList;
            }
        }
    }

    file.close();
    // send signal to enable checkboxes  and keyword file load button
    //QThread::currentThread()->msleep(2000);
    emit emitfinishReadingKeywordFile();

}

void Worker::readEmailFile() {
    QFile file("emails.txt");
    QString strings;
    QString str;
    qDebug() << "Readig file";
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "ERROR OPENING EMAIL FILE" << file.error();
    }


    QTextStream ts(&file);
    while (!ts.atEnd())
    {
        str = ts.readLine();
    }
    file.close();
}

void Worker::initHarvest()
{


    /*********
     If json file keys hold params, get those values and call curlParams with those values.
   **********/
    QVector <QString>vectorSearchOptions;
    QVector <QString>vectorEmailOptions;
    QVector <QString>vectorSocailNetworkOptions;
    QList<int>timerOptionsJson;
    QList<QString>otherOptionsJson;
    QList <QString> *proxyServersJson;
    proxyServersJson = new QList<QString>();
    QString val;
    QFile file;
    file.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/params.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening json file in webview";
    }
    val = file.readAll();

    file.close();


    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObject = d.object();
    QJsonValue searchEngineOptions = jsonObject.value(QString("SearchEngineOptions"));
    QJsonArray searchEngineOptionsArray = searchEngineOptions.toArray();


    QJsonValue emailOptions = jsonObject.value(QString("EmailOptions"));
    QJsonArray emailOptionsArray = emailOptions.toArray();


    QJsonValue socialNetWorkOptions = jsonObject.value(QString("SocialNetworkOptions"));
    QJsonArray socialNetWorkOptionsArray = socialNetWorkOptions.toArray();


    QString lineEditKeyword = jsonObject.value(QString("LineEditKeywords")).toString();
    int isFileListUploaded = jsonObject.value(QString("FILE_LIST_UPLOADED")).toInt();

    QJsonValue timerOptions = jsonObject.value(QString("TimerOptions"));
    QJsonObject timerOptionsKey = timerOptions.toObject();
    int proxyRotate = timerOptionsKey["ProxyRotate"].toInt();
    int harvesterTimer = timerOptionsKey["HarvesterTimer"].toInt();


    QJsonValue otherOptions = jsonObject.value(QString("OtherOptions"));
    QJsonObject otherOptionsKey = otherOptions.toObject();
    int searchResultsPagesNumber = otherOptionsKey["searchResultsPagesNumber"].toInt();
    QString multiUrlOption = otherOptionsKey["MULTI_URL_OPTION"].toString();
    int multiUrlAmount = otherOptionsKey["MULTI_URL_AMOUNT"].toInt();
    int searchResultsOption = otherOptionsKey["SEARCH_RESULTS_OPTION"].toInt();
    QString userAgent = otherOptionsKey["USER_AGENT"].toString();

    QJsonValue proxyServers = jsonObject.value(QString("ProxyServers"));
    QJsonArray proxyServersArray = proxyServers.toArray();
    //QJsonArray itemArray = item1["SearchOptions"];
    for(int i =0; i < searchEngineOptionsArray.size(); i++ )
    {
        vectorSearchOptions.insert(i,searchEngineOptionsArray[i].toString());
    }

    for(int i =0; i < emailOptionsArray.size(); i++ )
    {
        vectorEmailOptions.insert(i,emailOptionsArray[i].toString());
    }

    for(int i =0; i < socialNetWorkOptionsArray.size(); i++ )
    {
        vectorSocailNetworkOptions.insert(i,socialNetWorkOptionsArray[i].toString());
    }

    for(int i =0; i < proxyServersArray.size(); i++ )
    {
        proxyServersJson->insert(i,proxyServersArray[i].toString());

    }

    timerOptionsJson.insert(0,harvesterTimer);
    timerOptionsJson.insert(1,proxyRotate);

    otherOptionsJson.insert(0,QString::number(searchResultsPagesNumber));
    otherOptionsJson.insert(1,multiUrlOption);
    otherOptionsJson.insert(2,QString::number(multiUrlAmount));
    otherOptionsJson.insert(2,QString::number(searchResultsOption));
    otherOptionsJson.insert(3,userAgent);


    startHarvest(vectorSearchOptions,vectorEmailOptions,vectorSocailNetworkOptions,
               lineEditKeyword,proxyServersJson,timerOptionsJson,otherOptionsJson,isFileListUploaded);

    delete proxyServersJson;

}

void Worker::startHarvest(QVector<QString>vectorSearchEngineOptions,
  QVector<QString>vectorEmailOptions,QVector<QString>vectorSocialNetworks2,
    QString lineEdit_keywords_search_box, QList <QString> *proxyServers,
    QList<int>timerOptions, QList<QString>otherOptions, int isFileListUploaded)
    //QList<QString>otherOptions
    // searchResultsPages
{
    QString searchEngineParam;
    QString searchEngineParamMulti1;
    QString searchEngineParamMulti2;
    QString searchEngineParamMulti3;
    QString searchEngineParamMulti4;


    QString castSearchQueryNumPtr;
    QString socialNetWork;
    QString email;
    QString searchEngine;
    QString vectorSearchEngineOptionsLastItem;
    QString currentKeywordSearchBoxKeyword;
    QString filterCurrentKeyword;
    QString filterMulti1CurrentKeyword;
    QString filterMulti2CurrentKeyword;
    QString filterMulti3CurrentKeyword;
    QString filterMulti4CurrentKeyword;

    QString itemsFound;
    int harvesterTimer = timerOptions.value(0);
    int proxyRotateInterval = timerOptions.value(1);
    QString searchResultsPagesNumber = otherOptions.value(0);
    int appTimer;
    QString userAgent = otherOptions.value(3);
    QString multiOptionURLAmount;
    QString searchResultsOptions;

    QFile currentKeywordFile;
    QJsonObject currentKeywordObj;
    QJsonDocument currentKeywordJsonDocument;
    QStringList currentKeywordsList;
    QVector<QString>vectorCurrentKeywordListJson;
    QString currentKeywordJson;

    QFile completeResultsFile;
    QJsonObject completeResultsObj;
    QJsonDocument completeResultsJsonDocument;

    QFile stopHarvestFile;
    QJsonObject stopHarvestObj;
    QJsonDocument stopHarvestJsonDocument;
    //QStringList currentKeywordsList;
    enum defaultSearchResult{DEFAULT_SEARCH_RESULT =0,BYPASS_SEARCH_RESULT=1};
    isMulti1KeywordListComplete = false;
    isMulti2KeywordListComplete = false;
    isMulti3KeywordListComplete = false;
    isMulti4KeywordListComplete = false;




    /*********
    Chrono is namespace, meaning a certain class will on be in its scope
    Within this namespace we have a class thats a template name seconds, that takes a long long type -- which is seconds
    in our case. So to create a instance of seconds class, we need to create a object, that takes a long. In our case s(harvesterTimer)
    The s object of type seconds, also has members as well
    *******/
    std::chrono::seconds s(harvesterTimer);
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(s);
    // cast long long to int
    appTimer = static_cast<int>(ms.count());
     stopHarvestLoop = false;
   // qDebug() << "app timer " << appTimer;


    // Checks if keyword box is empty, and if file list if empty, if so by default put keyword box value into filelist


    // MULTI URL OPTION
    // *multiURLOption will hold either MULT_URL or 1_SELECTED_URL
    if (!otherOptions.value(1).isEmpty())
    {
        *multiURLOption = otherOptions.value(1);
    }
    if (!otherOptions.value(2).isEmpty()) {

        multiOptionURLAmount = otherOptions.value(2);
        searchResultsOptions = otherOptions.value(2);
    }

    // if 1_URL_SELECTED option has been selected, and list is uploaded
    if(*multiURLOption== "1_URL_SELECTED" && isFileListUploaded ==1)
    {


        QFile keywordFile1("C:/Users/ace/Documents/QT_Projects/WebView/WebView/multi1KeywordList.txt");
        if(!keywordFile1.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Error opening keywordlist file 1 in worker, 1_URL_SELECTED";
            return;
        }

        // loops through keyword file 1 assigns keywords to QStringList1
        QTextStream textStream1KeywordFile(&keywordFile1);
        while (!textStream1KeywordFile.atEnd())
        {

            multi1KeywordList << textStream1KeywordFile.readAll().split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            finishReadingMulti1KeywordFile = false;
            stopHarvestLoop = true;
        }

        // if file 1 is done looping , start harvest loop
        while(textStream1KeywordFile.atEnd()){
            finishReadingMulti1KeywordFile =true;
            stopHarvestLoop = false;
            break;
        }
        keywordFile1.close();
  }




    // MULTI_URL_SELECTED has been selected.
    if(*multiURLOption== "MULTI_URL_SELECTED")
    {


        QFile keywordFile1("C:/Users/ace/Documents/QT_Projects/WebView/WebView/multi1KeywordList.txt");
        if(!keywordFile1.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Error opening keywordlist file 1 in worker";
            return;
        }

        QFile keywordFile2("C:/Users/ace/Documents/QT_Projects/WebView/WebView/multi2KeywordList.txt");
        if(!keywordFile2.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Error opening keywordlist file 2 in worker";
            return;
        }

        QFile keywordFile3("C:/Users/ace/Documents/QT_Projects/WebView/WebView/multi3KeywordList.txt");
        if(!keywordFile3.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Error opening keywordlist file 3 in worker";
            return;
        }

        QFile keywordFile4("C:/Users/ace/Documents/QT_Projects/WebView/WebView/multi4KeywordList.txt");
        if(!keywordFile4.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Error opening keywordlist file 4 in worker";
            return;
        }


        // loops through keyword file 1 assigns keywords to QStringList1
        QTextStream textStream1KeywordFile(&keywordFile1);
        while (!textStream1KeywordFile.atEnd())
        {

            multi1KeywordList << textStream1KeywordFile.readAll().split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            finishReadingMulti1KeywordFile = false;
            stopHarvestLoop = true;
        }

        // if file 1 is done looping , start harvest loop
        while(textStream1KeywordFile.atEnd()){
            finishReadingMulti1KeywordFile =true;
            stopHarvestLoop = false;
            break;
        }


        // loops through keyword file 2 assigns keywords to QStringList2
        QTextStream textStream2KeywordFile(&keywordFile2);
        while (! textStream2KeywordFile.atEnd())
        {
            multi2KeywordList <<  textStream2KeywordFile.readAll().split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

            finishReadingMulti2KeywordFile = false;
            stopHarvestLoop = true;
        }

        // if file is done looping , start harvest loop
        while(textStream2KeywordFile.atEnd()){
            finishReadingMulti2KeywordFile = true;
            stopHarvestLoop = false;
            break;
        }


        // loops through keyword file 3 assigns keywords to QStringList3
        QTextStream textStream3KeywordFile(&keywordFile3);
        while (! textStream3KeywordFile.atEnd())
        {
            multi3KeywordList <<  textStream3KeywordFile.readAll().split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

            finishReadingMulti3KeywordFile = false;
            stopHarvestLoop = true;
        }

        // if file is done looping , start harvest loop
        while(textStream3KeywordFile.atEnd()){
            finishReadingMulti3KeywordFile = true;
            stopHarvestLoop = false;
            break;
        }


        // loops through keyword file 4 assigns keywords to QStringList3
        QTextStream textStream4KeywordFile(&keywordFile4);
        while (! textStream4KeywordFile.atEnd())
        {
            multi4KeywordList <<  textStream4KeywordFile.readAll().split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

            finishReadingMulti4KeywordFile = false;
            stopHarvestLoop = true;
        }

        // if file is done looping , start harvest loop
        while(textStream4KeywordFile.atEnd()){
            finishReadingMulti4KeywordFile = true;
            stopHarvestLoop = false;
            break;
        }


//        if(!keywordFile1.flush())
//        {
//            qDebug() << "Error flushing keywordfile1 <<" << keywordFile1.errorString();
//        }
//        if(!keywordFile2.flush())
//        {
//            qDebug() << "Error flushing keywordfile2 <<" << keywordFile2.errorString();
//        }
//        if(!keywordFile3.flush())
//        {
//            qDebug() << "Error flushing keywordfile3 <<" << keywordFile3.errorString();
//        }
//        if(!keywordFile4.flush())
//        {
//            qDebug() << "Error flushing keywordfile4 <<" << keywordFile4.errorString();
//        }

//        keywordFile1.write(" ",keywordFile1.size());
//        keywordFile2.write(" ",keywordFile2.size());
//        keywordFile3.write(" ",keywordFile3.size());
//        keywordFile4.write(" ",keywordFile4.size());

        keywordFile1.close();
        keywordFile2.close();
        keywordFile3.close();
        keywordFile4.close();

    }

    // if we are done looping through all list, we can start harvest loop
    if(finishReadingMulti1KeywordFile == true && finishReadingMulti2KeywordFile == true &&
       finishReadingMulti3KeywordFile == true && finishReadingMulti4KeywordFile == true &&
        *multiURLOption == "MULTI_URL_SELECTED")
    {
         stopHarvestLoop = false;
    }

   // start loop if single url is selected automatically
   if(*multiURLOption == "1_URL_SELECTED")
   {
         stopHarvestLoop = false;
   }



   // start harvest
   if( stopHarvestLoop == false)
   {
       for (int i = 0; i < 999999; i++)
          {



              //parsedEmails();
              if ( stopHarvestLoop) {
                  logMessage.prepend("Harvested loop has canceled");
                  ///lineEdit_keywords_search_box.clear();

                  curlSingleProcessPtrCounter = &curlSingleProcessCounterNum;
                  curlMultiProcessPtrCounter = &curlMultiProcessCounterNum;
                  emailOptionsNumPtr = &emailOptionsNum;
                  searchEngineNumPtr = &searchEngineNum;
                  searchEnginePaginationCounterPtr = &searchEnginePaginationCounterNum;
                  proxyServerCounterPtr = &proxyServerCounterNum;
                  workerCounterPtr = &workerCounterNum;

                  *curlSingleProcessPtrCounter = 0;
                  *curlMultiProcessPtrCounter = 0;
                  *emailOptionsNumPtr = 0;
                  *searchEngineNumPtr = 0;
                  *searchEnginePaginationCounterPtr = 0;
                  *proxyServerCounterPtr = 0;
                  *workerCounterPtr = 0;
                  //fileList->clear();
                  break;
              }



              //PROXY CONFIGURATION

              if (proxyServers->isEmpty())
              {

                  isProxyEmpty = true;
              }

              if (!proxyServers->isEmpty())
              {
                  isProxyEmpty = false;

              }

              // if workerCounter == *proxyRotateIntervalPtr, reset workerCounter ; if certain number of
              // http request have been made rotate proxy
              if (*workerCounterPtr <= proxyRotateInterval)
              {
                  // only rotate each proxy if proxyCounterPtr is not greater than our proxyServer qlist
                  if ((*proxyServerCounterPtr) <= proxyServers->size())
                  {
                      // if proxy counter is not greater than proxyServer qlist, proxyCounter can increment
                      canProxyCounterIncrement = true;
                  }

                  // if proxy counter is equal to the size of proxyServer qlist, we cant increment
                  if ((*proxyServerCounterPtr) == proxyServers->size())
                  {
                      canProxyCounterIncrement = false;
                      // if proxyServerCounter is equal to the size of the proxyServer qlist, reset it to 0
                      *proxyServerCounterPtr = 0;
                  }


                  // if proxies contained in qlist empty in main thread, if so clear the proxylist in this thread also
                  if (isProxyEmpty == true && proxyServers->size() == 0)
                  {
                      //qDebug() << "Proxy Empty";
                      //qDebug() << *proxyServers;
                      //qDebug() <<proxyServers->size();
                      proxyServers->clear();
                      *proxies = "";
                  }

                  // if proxies contained in qlist are not empty, and we can keep incrementing,
                  // our proxies are good to use/rotate
                  if (isProxyEmpty == false && canProxyCounterIncrement == true)
                  {
                      *proxies = proxyServers->value(*proxyServerCounterPtr);
                      //qDebug() << "Counter-->" << *proxies;

                  }
                  //qDebug() << "Counter-->" << *proxyServerCounterPtr;
                  //qDebug() << "Proxies-->" << *proxies;

              }


              // if workerCounter is greater than *proxyRotateIntervalPtr/ amount of http request before proxy rotates
              if (*workerCounterPtr >= proxyRotateInterval)
              {
                  // restart workerCounter
                  *workerCounterPtr = 0;
                  // increment proxyServerPtr to go through each proxyServer index every interval
                  (*proxyServerCounterPtr) += 1;

              }

              // increment workerCounter if we have not hit our http request limit to rotate each proxy
              (*workerCounterPtr) += 1;





              /********If user uploads list, but selects 1_URL_SELECTED********/
              if (!multi1KeywordList.isEmpty() && *multiURLOption == "1_URL_SELECTED")
              {
                  // increments index value on each timer interval
                  *currentKeywordPtr = multi1KeywordList.value(*multi1CurrentKeywordPtr);

              }
              /********If user uploads list, and selects MULTI_URL_SELECTED********/
              if (!multi1KeywordList.isEmpty() &&  *multiURLOption == "MULTI_URL_SELECTED")
              {
                  // on load is 0 which is the first index value
                  // increments index value on each timer interval
                  multi1CurrentKeyword = multi1KeywordList.value(*multi1CurrentKeywordPtr);
              }
              /********If user uploads list, and selects MULTI_URL_SELECTED********/
              if (!multi2KeywordList.isEmpty() && *multiURLOption == "MULTI_URL_SELECTED")
              {
                  // on load is 0 which is the first index value
                  // increments index value on each timer interval
                  multi2CurrentKeyword = multi2KeywordList.value(*multi2CurrentKeywordPtr);
              }

              /********If user uploads list, and selects MULTI_URL_SELECTED********/
              if (!multi3KeywordList.isEmpty() &&  *multiURLOption == "MULTI_URL_SELECTED")
              {
                  // on load is 0 which is the first index value
                  // increments index value on each timer interval
                  multi3CurrentKeyword = multi3KeywordList.value(*multi3CurrentKeywordPtr);
              }
              /********If user uploads list, and selects MULTI_URL_SELECTED********/
              if (!multi4KeywordList.isEmpty() && *multiURLOption == "MULTI_URL_SELECTED")
              {
                  // on load is 0 which is the first index value
                  // increments index value on each timer interval
                  multi4CurrentKeyword = multi4KeywordList.value(*multi4CurrentKeywordPtr);
              }


        /*********Increments, and loops through each keyword during each timer interval********/
              (*curlSingleProcessPtrCounter) += 1;
              (*curlMultiProcessPtrCounter) += 1;





              /**********Search Engine Options ******/

              // if pointer is equal to size of vector were done
              if (*searchEngineNumPtr == vectorSearchEngineOptions.size())
              {
                  *searchEngineNumPtr = 0; // done
              }

              // if vector search engine contains current search engine, assign it it to search eng var
              if (vectorSearchEngineOptions.contains(vectorSearchEngineOptions.value(*searchEngineNumPtr)))
              {
                  searchEngine = vectorSearchEngineOptions.value(*searchEngineNumPtr);
              }

              /********End Search Engine Options*****/



              /*******Email Options******/

              if (searchEngine == "http://google.com")
              {

                  /*****Email Options******/

                  /******Social NetWork Options******/
                  if (*emailOptionsNumPtr == vectorEmailOptions.size()) {

                      *emailOptionsNumPtr = 0; // done
                  }



                  if (vectorEmailOptions.contains(vectorEmailOptions.value(*emailOptionsNumPtr)))
                  {
                      email = vectorEmailOptions.value(*emailOptionsNumPtr);
                  }



                  /******Social NetWork Options******/
                  if (*socialNetWorkNumPtr == vectorSocialNetworks2.size()) {

                      *socialNetWorkNumPtr = 0; // done
                  }


                  if (vectorSocialNetworks2.contains(vectorSocialNetworks2.value(*socialNetWorkNumPtr))) {
                      socialNetWork = vectorSocialNetworks2.value(*socialNetWorkNumPtr);
                  }

                  // search engines pagination number
                  (*searchEnginePaginationCounterPtr) += 10;

                  /*****Cast num to string to put inside query string******/
                  castSearchQueryNumPtr = QString::number(*searchEnginePaginationCounterPtr);




                  // if keyword list uploaded, and 1_URL_SELECTED is selected
                  if(lineEdit_keywords_search_box.isEmpty() && *multiURLOption == "1_URL_SELECTED"
                          && !multi1KeywordList.isEmpty()){
                      searchEngineParam = "https://www.google.com/search?q=" + socialNetWork + "%20"
                          + email + "%20" + currentKeywordPtr->replace(" ", "+") +
                          "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";
                  }


                  // if keyword box is not empty, and 1_URL_SELECTED has been selected
                  if ((!lineEdit_keywords_search_box.isEmpty() && *multiURLOption == "1_URL_SELECTED")
                         // || multi1KeywordList.isEmpty()
                     )
                  {

                      currentKeywordSearchBoxKeyword = lineEdit_keywords_search_box;
                      searchEngineParam = "https://www.google.com/search?q=" + socialNetWork + "%20"
                          + email + "%20" + currentKeywordSearchBoxKeyword.replace(" ", "+") +
                          "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";
                  }

                  // if list is uploaded, and MULTI_URL_SELECTED has been selected
                  if (!multi1KeywordList.isEmpty() && !multi2KeywordList.isEmpty() &&  *multiURLOption == "MULTI_URL_SELECTED")
                  {

                          // if keyword list is uploaded but user has selected multi  option
                          // if multi option is selected, assign url to multiparam1 and multiparam2
                          searchEngineParamMulti1 = "https://www.google.com/search?q=" + socialNetWork + "%20"
                              + email + "%20" + multi1CurrentKeyword.replace(" ", "+") +
                              "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";

                          searchEngineParamMulti2 = "https://www.google.com/search?q=" + socialNetWork + "%20"
                              + email + "%20" + multi2CurrentKeyword.replace(" ", "+") +
                              "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";

                          searchEngineParamMulti3 = "https://www.google.com/search?q=" + socialNetWork + "%20"
                              + email + "%20" + multi3CurrentKeyword.replace(" ", "+") +
                              "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";

                          searchEngineParamMulti4 = "https://www.google.com/search?q=" + socialNetWork + "%20"
                              + email + "%20" + multi4CurrentKeyword.replace(" ", "+") +
                              "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";


                  }



                  /****Continues email quene until its the last item in array***/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      if (*emailOptionsNumPtr == vectorEmailOptions.size() - 1) {

                          *emailOptionsNumPtr = 0;

                      }
                      else {
                          (*emailOptionsNumPtr) += 1;
                      }
                  }

                  /****Continues social network quene until its the last item in array***/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      if (*socialNetWorkNumPtr == vectorSocialNetworks2.size() - 1) {

                          *socialNetWorkNumPtr = 0;

                      }
                      else {
                          (*socialNetWorkNumPtr) += 1;
                      }
                  }





                  /*******
                  * If search engine pagination reaches 100
                  * Stops social network, email, ans search engine quene, and moves on to next search engine
                  ******/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      *searchEnginePaginationCounterPtr = 0;

                      // if social network pointer, and email options pointer is equal
                      //than the size of  socialNetworkOptions arrary,
                      //then were done, and move on
                      if (*socialNetWorkNumPtr == vectorSocialNetworks2.size() - 1)
                      {

                          /*******
                          if the last item in vector is true, and dosent match our current value
                          theres more elements after our current element, we need
                          this to make sure out pointer dosent get out of a range/QVector out of range.
                          *****/

                          if (!vectorSearchEngineOptions.last().isEmpty())
                          {
                              vectorSearchEngineOptionsLastItem = vectorSearchEngineOptions.last();
                              if (vectorSearchEngineOptionsLastItem != vectorSearchEngineOptions.value(*searchEngineNumPtr)) {
                                  (*searchEngineNumPtr) += 1;
                              }
                          }
                      }// end of checking if search eng pagination has reached 100


                      if (*emailOptionsNumPtr == vectorEmailOptions.size())
                      {
                          *emailOptionsNumPtr = 0;
                      }



                  }


                  //            qDebug() << "Search Engine Vector -->" <<vectorSearchEngineOptions;
                  //            qDebug() << "Search Engine Vector Size -->" <<vectorSearchEngineOptions.size();
                  //            qDebug() << "Search Engine Vector Pointer -->" << *searchEngineNumPtr;


                  //            qDebug() << "Email Vector  -->" <<vectorEmailOptions;
                  //            qDebug() << "Email Vector Size -->" <<vectorEmailOptions.size();
                  //            qDebug() << "Email Vector Pointer -->" << *emailOptionsNumPtr;


                  //            qDebug() << "Social Vector -->" <<vectorSocialNetworks2;
                  //            qDebug() << "Social Vector Size -->" <<vectorSocialNetworks2.size();
                  //            qDebug() << "Social Vector Pointer -->" << *socialNetWorkNumPtr;

                  //qDebug() << searchEngineParam;
                  // qDebug()<<  searchEngine;
                  // qDebug()<<  vectorSearchEngineOptions;
                  // qDebug() << *searchEnginePaginationCounterPtr;



              }// end of checking if search engine is Google



               /************Bing*************/

              if (searchEngine == "http://bing.com")
              {

                  /*****Email Options******/

                  /******Social NetWork Options******/
                  if (*emailOptionsNumPtr == vectorEmailOptions.size()) {

                      *emailOptionsNumPtr = 0; // done
                  }



                  if (vectorEmailOptions.contains(vectorEmailOptions.value(*emailOptionsNumPtr)))
                  {
                      email = vectorEmailOptions.value(*emailOptionsNumPtr);
                  }



                  /******Social NetWork Options******/
                  if (*socialNetWorkNumPtr == vectorSocialNetworks2.size()) {

                      *socialNetWorkNumPtr = 0; // done
                  }


                  if (vectorSocialNetworks2.contains(vectorSocialNetworks2.value(*socialNetWorkNumPtr))) {
                      socialNetWork = vectorSocialNetworks2.value(*socialNetWorkNumPtr);
                  }

                  // search engines pagination number
                  (*searchEnginePaginationCounterPtr) += 10;

                  /*****Cast num to string to put inside query string******/
                  castSearchQueryNumPtr = QString::number(*searchEnginePaginationCounterPtr);




                  // if keyword list uploaded, and 1_URL_SELECTED is selected
                  if(lineEdit_keywords_search_box.isEmpty() && *multiURLOption == "1_URL_SELECTED"
                          && !multi1KeywordList.isEmpty()){
                      searchEngineParam = "https://www.bing.com/search?q=" +
                     socialNetWork + "%20"
                     + email + "%20" + currentKeywordSearchBoxKeyword.replace(" ", "+") +
                     "&qs=n&sp=-1&pq=undefined&sc=0-45&sk=&cvid=6C577B0F2A1348BBB5AF38F9AC4CA13A&first="
                     + castSearchQueryNumPtr + "&FORM=PERE";
                  }


                  // if keyword box is not empty, and 1_URL_SELECTED has been selected
                  if ((!lineEdit_keywords_search_box.isEmpty() && *multiURLOption == "1_URL_SELECTED")
                         // || multi1KeywordList.isEmpty()
                     )
                  {

                      currentKeywordSearchBoxKeyword = lineEdit_keywords_search_box;
                      searchEngineParam = "https://www.bing.com/search?q=" +
                     socialNetWork + "%20"
                     + email + "%20" + currentKeywordSearchBoxKeyword.replace(" ", "+") +
                     "&qs=n&sp=-1&pq=undefined&sc=0-45&sk=&cvid=6C577B0F2A1348BBB5AF38F9AC4CA13A&first="
                     + castSearchQueryNumPtr + "&FORM=PERE";
                  }

                  // if list is uploaded, and MULTI_URL_SELECTED has been selected
                  if (!multi1KeywordList.isEmpty() && !multi2KeywordList.isEmpty() &&  *multiURLOption == "MULTI_URL_SELECTED")
                  {

                          // if keyword list is uploaded but user has selected multi  option
                          // if multi option is selected, assign url to multiparam1 and multiparam2
                        searchEngineParamMulti1 = "https://www.bing.com/search?q=" + socialNetWork + "%20"
                            + email + "%20" + multi1CurrentKeyword.replace(" ", "+") +
                            "&qs=n&sp=-1&pq=undefined&sc=0-45&sk=&cvid=6C577B0F2A1348BBB5AF38F9AC4CA13A&first="
                                + castSearchQueryNumPtr + "&FORM=PERE";

                        searchEngineParamMulti2 = "https://www.bing.com/search?q=" + socialNetWork + "%20"
                            + email + "%20" + multi2CurrentKeyword.replace(" ", "+") +
                            "&qs=n&sp=-1&pq=undefined&sc=0-45&sk=&cvid=6C577B0F2A1348BBB5AF38F9AC4CA13A&first=" +
                                castSearchQueryNumPtr + "&FORM=PERE";

                        searchEngineParamMulti3 = "https://www.bing.com/search?q=" + socialNetWork + "%20"
                            + email + "%20" + multi3CurrentKeyword.replace(" ", "+") +
                            "&qs=n&sp=-1&pq=undefined&sc=0-45&sk=&cvid=6C577B0F2A1348BBB5AF38F9AC4CA13A&first=" +
                                castSearchQueryNumPtr + "&FORM=PERE";

                        searchEngineParamMulti4 = "https://www.bing.com/search?q=" + socialNetWork + "%20"
                            + email + "%20" + multi4CurrentKeyword.replace(" ", "+") +
                            "&qs=n&sp=-1&pq=undefined&sc=0-45&sk=&cvid=6C577B0F2A1348BBB5AF38F9AC4CA13A&first=" +
                                castSearchQueryNumPtr + "&FORM=PERE";
                  }


                  /****Continues email quene until its the last item in array***/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      if (*emailOptionsNumPtr == vectorEmailOptions.size() - 1) {

                          *emailOptionsNumPtr = 0;

                      }
                      else {
                          (*emailOptionsNumPtr) += 1;
                      }
                  }

                  /****Continues social network quene until its the last item in array***/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      if (*socialNetWorkNumPtr == vectorSocialNetworks2.size() - 1) {

                          *socialNetWorkNumPtr = 0;

                      }
                      else {
                          (*socialNetWorkNumPtr) += 1;
                      }
                  }





                  /*******
                  * If search engine pagination reaches 100
                  * Stops social network, email, ans search engine quene, and moves on to next search engine
                  ******/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      *searchEnginePaginationCounterPtr = 0;

                      // if social network pointer, and email options pointer is equal
                      //than the size of  socialNetworkOptions arrary,
                      //then were done, and move on
                      if (*socialNetWorkNumPtr == vectorSocialNetworks2.size() - 1)
                      {

                          /*******
                          if the last item in vector is true, and dosent match our current value
                          theres more elements after our current element, we need
                          this to make sure out pointer dosent get out of a range/QVector out of range.
                          *****/

                          if (!vectorSearchEngineOptions.last().isEmpty())
                          {
                              vectorSearchEngineOptionsLastItem = vectorSearchEngineOptions.last();
                              if (vectorSearchEngineOptionsLastItem != vectorSearchEngineOptions.value(*searchEngineNumPtr)) {
                                  (*searchEngineNumPtr) += 1;
                              }
                          }
                      }// end of checking if search eng pagination has reached 100


                      if (*emailOptionsNumPtr == vectorEmailOptions.size())
                      {
                          *emailOptionsNumPtr = 0;
                      }



                  }


                  //            qDebug() << "Search Engine Vector -->" <<vectorSearchEngineOptions;
                  //            qDebug() << "Search Engine Vector Size -->" <<vectorSearchEngineOptions.size();
                  //            qDebug() << "Search Engine Vector Pointer -->" << *searchEngineNumPtr;


                  //            qDebug() << "Email Vector  -->" <<vectorEmailOptions;
                  //            qDebug() << "Email Vector Size -->" <<vectorEmailOptions.size();
                  //            qDebug() << "Email Vector Pointer -->" << *emailOptionsNumPtr;


                  //            qDebug() << "Social Vector -->" <<vectorSocialNetworks2;
                  //            qDebug() << "Social Vector Size -->" <<vectorSocialNetworks2.size();
                  //            qDebug() << "Social Vector Pointer -->" << *socialNetWorkNumPtr;

                  // qDebug() << searchEngineParam;
                  // qDebug()<<  searchEngine;
                  // qDebug()<<  vectorSearchEngineOptions;
                  // qDebug() << *searchEnginePaginationCounterPtr;



              }// end of checking if search engine is Bing




               /***************Aol***************/


              if (searchEngine == "http://aol.com")
              {

                  /*****Email Options******/

                  /******Social NetWork Options******/
                  if (*emailOptionsNumPtr == vectorEmailOptions.size()) {

                      *emailOptionsNumPtr = 0; // done
                  }



                  if (vectorEmailOptions.contains(vectorEmailOptions.value(*emailOptionsNumPtr)))
                  {
                      email = vectorEmailOptions.value(*emailOptionsNumPtr);
                  }



                  /******Social NetWork Options******/
                  if (*socialNetWorkNumPtr == vectorSocialNetworks2.size()) {

                      *socialNetWorkNumPtr = 0; // done
                  }


                  if (vectorSocialNetworks2.contains(vectorSocialNetworks2.value(*socialNetWorkNumPtr))) {
                      socialNetWork = vectorSocialNetworks2.value(*socialNetWorkNumPtr);
                  }

                  // search engines pagination number
                  (*searchEnginePaginationCounterPtr) += 10;

                  /*****Cast num to string to put inside query string******/
                  castSearchQueryNumPtr = QString::number(*searchEnginePaginationCounterPtr);


      //https://search.aol.com/aol/search;_ylt=AwrE1xRyCF5bUC8ATFFpCWVH;_ylu=X3oDMTEzajVvczlrBGNvbG8DYmYxBHBvcwMxBHZ0aWQDBHNlYwNwYWdpbmF0aW9u?q=site:instagram.com%40music%40@gmail.com&b=11&pz=20&bct=0&xargs=0&v_t=comsearch
      //https://search.aol.com/aol/search;_ylt=A0PDsBs7Il5b6O8A0HdpCWVH;_ylu=X3oDMTEzajVvczlrBGNvbG8DYmYxBHBvcwMxBHZ0aWQDBHNlYwNwYWdpbmF0aW9u?q=site%3Ainstagram.com+my+music+%40yahoo.com&b=11&pz=10&bct=0&xargs=0&v_t=comsearch
                  // if keyword list uploaded, and 1_URL_SELECTED is selected
                  if(lineEdit_keywords_search_box.isEmpty() && *multiURLOption == "1_URL_SELECTED"
                          && !multi1KeywordList.isEmpty()){
                      searchEngineParam = "https://search.aol.com/aol/search?q=" +socialNetWork + "%20"
                      + email + "%20" + currentKeywordSearchBoxKeyword.replace(" ", "+") +
                      "&b=11&pz=" + castSearchQueryNumPtr + "&bct=0&xargs=0&v_t=comsearch";
                  }


                  // if keyword box is not empty, and 1_URL_SELECTED has been selected
                  if ((!lineEdit_keywords_search_box.isEmpty() && *multiURLOption == "1_URL_SELECTED")
                         // || multi1KeywordList.isEmpty()
                     )
                  {

                      currentKeywordSearchBoxKeyword = lineEdit_keywords_search_box;
                      searchEngineParam = "https://search.aol.com/aol/search?q=" +socialNetWork + "%20"
                      + email + "%20" + currentKeywordSearchBoxKeyword.replace(" ", "+") +
                      "&b=11&pz=" + castSearchQueryNumPtr + "&bct=0&xargs=0&v_t=comsearch";
                  }

                  // if list is uploaded, and MULTI_URL_SELECTED has been selected
                  if (!multi1KeywordList.isEmpty() && !multi2KeywordList.isEmpty() &&  *multiURLOption == "MULTI_URL_SELECTED")
                  {

                          // if keyword list is uploaded but user has selected multi  option
                          // if multi option is selected, assign url to multiparam1 and multiparam2
                      searchEngineParamMulti1 = "https://search.aol.com/aol/search?q="+ socialNetWork + "%20"
                            + email +  multi1CurrentKeyword.replace(" ", "+") +
                             "&b=11&pz=" + castSearchQueryNumPtr + "&bct=0&xargs=0&v_t=comsearch";


                        searchEngineParamMulti2 = "https://search.aol.com/aol/search?q=" + socialNetWork + "%20"
                            +  email + multi2CurrentKeyword.replace(" ", "+") +
                            "&b=11&pz=" + castSearchQueryNumPtr + "&bct=0&xargs=0&v_t=comsearch";

                        searchEngineParamMulti3 = "https://search.aol.com/aol/search?q=" + socialNetWork + "%20"
                            +  email + multi3CurrentKeyword.replace(" ", "+") +
                            "&b=11&pz=" + castSearchQueryNumPtr + "&bct=0&xargs=0&v_t=comsearch";

                        searchEngineParamMulti4 = "https://search.aol.com/aol/search?q=" + socialNetWork + "%20"
                            +  email + multi4CurrentKeyword.replace(" ", "+") +
                            "&b=11&pz=" + castSearchQueryNumPtr + "&bct=0&xargs=0&v_t=comsearch";
                  }


                  /****Continues email quene until its the last item in array***/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      if (*emailOptionsNumPtr == vectorEmailOptions.size() - 1) {

                          *emailOptionsNumPtr = 0;

                      }
                      else {
                          (*emailOptionsNumPtr) += 1;
                      }
                  }

                  /****Continues social network quene until its the last item in array***/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      if (*socialNetWorkNumPtr == vectorSocialNetworks2.size() - 1) {

                          *socialNetWorkNumPtr = 0;

                      }
                      else {
                          (*socialNetWorkNumPtr) += 1;
                      }
                  }





                  /*******
                  * If search engine pagination reaches 100
                  * Stops social network, email, ans search engine quene, and moves on to next search engine
                  ******/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      *searchEnginePaginationCounterPtr = 0;

                      // if social network pointer, and email options pointer is equal
                      //than the size of  socialNetworkOptions arrary,
                      //then were done, and move on
                      if (*socialNetWorkNumPtr == vectorSocialNetworks2.size() - 1)
                      {

                          /*******
                          if the last item in vector is true, and dosent match our current value
                          theres more elements after our current element, we need
                          this to make sure out pointer dosent get out of a range/QVector out of range.
                          *****/

                          if (!vectorSearchEngineOptions.last().isEmpty())
                          {
                              vectorSearchEngineOptionsLastItem = vectorSearchEngineOptions.last();
                              if (vectorSearchEngineOptionsLastItem != vectorSearchEngineOptions.value(*searchEngineNumPtr)) {
                                  (*searchEngineNumPtr) += 1;
                              }
                          }
                      }// end of checking if search eng pagination has reached 100

                      if (*emailOptionsNumPtr == vectorEmailOptions.size())
                      {
                          *emailOptionsNumPtr = 0;
                      }
                  }


                  //            qDebug() << "Search Engine Vector -->" <<vectorSearchEngineOptions;
                  //            qDebug() << "Search Engine Vector Size -->" <<vectorSearchEngineOptions.size();
                  //            qDebug() << "Search Engine Vector Pointer -->" << *searchEngineNumPtr;


                  //            qDebug() << "Email Vector  -->" <<vectorEmailOptions;
                  //            qDebug() << "Email Vector Size -->" <<vectorEmailOptions.size();
                  //            qDebug() << "Email Vector Pointer -->" << *emailOptionsNumPtr;


                  //            qDebug() << "Social Vector -->" <<vectorSocialNetworks2;
                  //            qDebug() << "Social Vector Size -->" <<vectorSocialNetworks2.size();
                  //            qDebug() << "Social Vector Pointer -->" << *socialNetWorkNumPtr;

                  // qDebug() << searchEngineParam;
                  // qDebug()<<  searchEngine;
                  // qDebug()<<  vectorSearchEngineOptions;
                  // qDebug() << *searchEnginePaginationCounterPtr;

              }// end of checking if search engine is Aol


               /***************Ask***************/


              if (searchEngine == "http://ask.com")
              {

                  /*****Email Options******/

                  /******Social NetWork Options******/
                  if (*emailOptionsNumPtr == vectorEmailOptions.size()) {

                      *emailOptionsNumPtr = 0; // done
                  }



                  if (vectorEmailOptions.contains(vectorEmailOptions.value(*emailOptionsNumPtr)))
                  {
                      email = vectorEmailOptions.value(*emailOptionsNumPtr);
                  }



                  /******Social NetWork Options******/
                  if (*socialNetWorkNumPtr == vectorSocialNetworks2.size()) {

                      *socialNetWorkNumPtr = 0; // done
                  }


                  if (vectorSocialNetworks2.contains(vectorSocialNetworks2.value(*socialNetWorkNumPtr))) {
                      socialNetWork = vectorSocialNetworks2.value(*socialNetWorkNumPtr);
                  }

                  // search engines pagination number
                  (*searchEnginePaginationCounterPtr) += 10;

                  /*****Cast num to string to put inside query string******/
                  castSearchQueryNumPtr = QString::number(*searchEnginePaginationCounterPtr);


                  /**********
                  if search box is not empty assign search box value to currentKeywordSearchBoxKeyword
                  so incase keywordLoadListOptions is empty, we can allow it be appended to searchEngineParam
                  on its own.

                  --->>> If both keywordLoadListOptions, and searchbox keyword is NOT empty,
                  were going to add the search box keyword to the keywordLoadListOptions List
                  to be included for processing.

                  ************/
                  if (!lineEdit_keywords_search_box.isEmpty())
                  {

                      currentKeywordSearchBoxKeyword =  lineEdit_keywords_search_box;
                      searchEngineParam = "https://www.google.com/search?q=" + socialNetWork + "%20"
                          + email + "%20" + currentKeywordSearchBoxKeyword.replace(" ", "+") +
                          "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";

                  }

                  if (!multi1KeywordList.isEmpty() && !multi1KeywordList.isEmpty())
                  {

                      searchEngineParam = "https://www.google.com/search?q=" + socialNetWork + "%20"
                          + email + "%20" + currentKeywordPtr->replace(" ", "+") +
                          "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";

                      // if keyword list is uploaded but user has selected multi  option
                      if (*multiURLOption == "MULTI_URL_SELECTED")
                      {
                          // if multi option is selected, assign url to multiparam1 and multiparam2
                          searchEngineParamMulti1 = "https://www.google.com/search?q=" + socialNetWork + "%20"
                              + email + "%20" + multi1CurrentKeyword.replace(" ", "+") +
                              "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";

                          searchEngineParamMulti2 = "https://www.google.com/search?q=" + socialNetWork + "%20"
                              + email + "%20" + multi2CurrentKeyword.replace(" ", "+") +
                              "&ei=yv8oW8TYCOaN5wKImJ2YBQ&start=" + castSearchQueryNumPtr + "&sa=N&biw=1366&bih=613";
                      }

                  }



                  /****Continues email quene until its the last item in array***/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      if (*emailOptionsNumPtr == vectorEmailOptions.size() - 1) {

                          *emailOptionsNumPtr = 0;

                      }
                      else {
                          (*emailOptionsNumPtr) += 1;
                      }
                  }

                  /****Continues social network quene until its the last item in array***/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      if (*socialNetWorkNumPtr == vectorSocialNetworks2.size() - 1) {

                          *socialNetWorkNumPtr = 0;

                      }
                      else {
                          (*socialNetWorkNumPtr) += 1;
                      }
                  }





                  /*******
                  * If search engine pagination reaches 100
                  * Stops social network, email, ans search engine quene, and moves on to next search engine
                  ******/

                  if (*searchEnginePaginationCounterPtr == 100) {
                      *searchEnginePaginationCounterPtr = 0;

                      // if social network pointer, and email options pointer is equal
                      //than the size of  socialNetworkOptions arrary,
                      //then were done, and move on
                      if (*socialNetWorkNumPtr == vectorSocialNetworks2.size() - 1)
                      {

                          /*******
                          if the last item in vector is true, and dosent match our current value
                          theres more elements after our current element, we need
                          this to make sure out pointer dosent get out of a range/QVector out of range.
                          *****/

                          if (!vectorSearchEngineOptions.last().isEmpty())
                          {
                              vectorSearchEngineOptionsLastItem = vectorSearchEngineOptions.last();
                              if (vectorSearchEngineOptionsLastItem != vectorSearchEngineOptions.value(*searchEngineNumPtr)) {
                                  (*searchEngineNumPtr) += 1;
                              }
                          }
                      }// end of checking if search eng pagination has reached 100

                      if (*emailOptionsNumPtr == vectorEmailOptions.size())
                      {
                          *emailOptionsNumPtr = 0;
                      }
                  }


                  //            qDebug() << "Search Engine Vector -->" <<vectorSearchEngineOptions;
                  //            qDebug() << "Search Engine Vector Size -->" <<vectorSearchEngineOptions.size();
                  //            qDebug() << "Search Engine Vector Pointer -->" << *searchEngineNumPtr;


                  //            qDebug() << "Email Vector  -->" <<vectorEmailOptions;
                  //            qDebug() << "Email Vector Size -->" <<vectorEmailOptions.size();
                  //            qDebug() << "Email Vector Pointer -->" << *emailOptionsNumPtr;


                  //            qDebug() << "Social Vector -->" <<vectorSocialNetworks2;
                  //            qDebug() << "Social Vector Size -->" <<vectorSocialNetworks2.size();
                  //            qDebug() << "Social Vector Pointer -->" << *socialNetWorkNumPtr;

                  // qDebug() << searchEngineParam;
                  // qDebug()<<  searchEngine;
                  // qDebug()<<  vectorSearchEngineOptions;
                  // qDebug() << *searchEnginePaginationCounterPtr;

              }// end of checking if search engine is Ask







              /******Assigns single keyword/keyword box to json value******/
              currentKeywordJson =currentKeywordSearchBoxKeyword.replace("+"," ");

              /******Inserts multi keyword list values into current keyword json stringlist******/
              if(!multi1KeywordList.isEmpty())
              {
                  vectorCurrentKeywordListJson.prepend(multi1KeywordList.value(*multi1CurrentKeywordPtr).replace("+"," "));

              }
              if(!multi2KeywordList.isEmpty())
              {
                  vectorCurrentKeywordListJson.prepend(multi2KeywordList.value(*multi2CurrentKeywordPtr).replace("+"," "));

              }


              if(!multi3KeywordList.isEmpty())
              {
                  vectorCurrentKeywordListJson.prepend(multi3KeywordList.value(*multi3CurrentKeywordPtr).replace("+"," "));

              }
              if(!multi4KeywordList.isEmpty())
              {
                  vectorCurrentKeywordListJson.prepend(multi4KeywordList.value(*multi4CurrentKeywordPtr).replace("+"," "));

              }

              for(int i=0; i <vectorCurrentKeywordListJson.size(); i++)
              {
                  if(!vectorCurrentKeywordListJson.value(i).isEmpty())
                  {
                    currentKeywordsList.insert(i, vectorCurrentKeywordListJson.value(i));

                  }
              }


              /******
                If 1 url option is selected,and multi1KeywordList list is not empty, and keyword box
                is empty, inert 1 keyword into json object
              ******/
              if(*multiURLOption == "1_URL_SELECTED" && !multi1KeywordList.isEmpty()
                      && lineEdit_keywords_search_box.isEmpty())
              {
                  /*******Insert json data into json object********/
                  currentKeywordObj.insert("CurrentKeyword",QJsonValue(multi1KeywordList.value(*multi1CurrentKeywordPtr)).toString());
                  emit emitLoopWebViewParams(searchEngineParam);
              }

              /******
               If 1 url option is selected, and keyword box is not empty
              insert 1 keyword into json object
              ******/
              if(*multiURLOption == "1_URL_SELECTED" && !lineEdit_keywords_search_box.isEmpty())
              {
                  /*******Insert json data into json object********/
                  currentKeywordObj.insert("CurrentKeyword",QJsonValue(currentKeywordJson).toString());
                  emit emitLoopWebViewParams(searchEngineParam);
              }

              /******If multi url option is selected isert multiple keywords into json object******/
              if(*multiURLOption == "MULTI_URL_SELECTED")
              {

                  /*******Insert json data into json object********/
                 currentKeywordObj.insert("CurrentKeywords",QJsonArray::fromStringList(currentKeywordsList.toSet().toList()));

                 emit emitMultiLoopWebViewParams(searchEngineParamMulti1,searchEngineParamMulti2,
                                                 searchEngineParamMulti3,searchEngineParamMulti4);

              }

              /*******Write current keywords to json file*******/
               currentKeywordFile.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/currentkeywords.json");
               if(!currentKeywordFile.open(QIODevice::WriteOnly)){
                   qDebug() << "Error opening currentkeywords json file in worker";
                   return;
               }
               if(!currentKeywordFile.flush())
               {
                  qDebug() << "Error flusing currentKeywords json file " << currentKeywordFile.errorString();
               }
              currentKeywordJsonDocument.setObject(currentKeywordObj);
              currentKeywordFile.write(currentKeywordJsonDocument.toJson());
              currentKeywordFile.close();
              vectorCurrentKeywordListJson.clear();




              logMessage.prepend(QString("Multi Counter--> ") + QString::number(*curlMultiProcessPtrCounter));

              /**** If 1_URL_SELECTED is selected/ or if MULTI_URL_SELECTED is selected ,
               and timer is less or equal to search results combox box..
               Stop harvest if 1_URL_SELECTED is selected but no list was uploaded
               If list is uploaded, and 1_URL_SELECTED is selected, loop through all keywords, then stop harvest
               ***/
              if (QString::number(*curlSingleProcessPtrCounter) == QString(searchResultsPagesNumber) && *multiURLOption == "1_URL_SELECTED")
              {

                  //Stop harvest if 1_URL_SELECTED, DEFAULT_SEARCH_RESULT is selected, but no list was uploaded
                  if (multi1KeywordList.isEmpty() && searchResultsOptions == QString::number(DEFAULT_SEARCH_RESULT))
                  {
                      logMessage.prepend(QString("1_URL_SELECTED, FileList is Empty and Default Search Options is true"));
                      logMessage.prepend(QString("1_URL_SELECTED, using keywordbox, keyword--> " +currentKeywordSearchBoxKeyword.replace("+"," ")));
                      *curlSingleProcessPtrCounter = 0;
                      stopHarvestLoop = true;
                      emitFinishSenderHarvestResults("MULTI_SINGLE COMPLETED");
                      logHarvesterStatus.clear();
                      completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("1_URL_SELECTED_RESULTS_COMPLETED").toString());
                      completeResultsFile.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/complete.json");
                      if(!completeResultsFile.open(QIODevice::WriteOnly)){
                          qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                          return;
                      }
                     completeResultsJsonDocument.setObject(completeResultsObj);
                     completeResultsFile.write(completeResultsJsonDocument.toJson());
                     completeResultsFile.close();
                  }


                  //Stop harvest if 1_URL_SELECTED, DEFAULT_SEARCH_RESULT is selected,  but list was uploaded
                  if (!multi1KeywordList.isEmpty() && searchResultsOptions == QString::number(DEFAULT_SEARCH_RESULT))
                  {
                      logMessage.prepend(QString("1_URL_SELECTED, FileList is Not Empty and Default Search Options is true"));
                      logMessage.prepend(QString("1_URL_SELECTED, using multi1KeywordList, keyword--> " + QString(multi1KeywordList.value(*multi1CurrentKeywordPtr))));
                      *curlSingleProcessPtrCounter = 0;
                      stopHarvestLoop = true;
                      emitFinishSenderHarvestResults("MULTI_SINGLE COMPLETED");
                      logHarvesterStatus.clear();
                      completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("1_URL_SELECTED_RESULTS_COMPLETED").toString());
                      completeResultsFile.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/complete.json");
                      if(!completeResultsFile.open(QIODevice::WriteOnly)){
                          qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                          return;
                      }
                     completeResultsJsonDocument.setObject(completeResultsObj);
                     completeResultsFile.write(completeResultsJsonDocument.toJson());
                     completeResultsFile.close();
                  }

                  /*************
                   If list is uploaded, and 1_URL_SELECTED is selected,
                   loop through all keywords, then stop harvest

                   ************/
                  if (!multi1KeywordList.isEmpty() && searchResultsOptions == QString::number(BYPASS_SEARCH_RESULT))
                  {
                      logMessage.prepend(QString("1_URL_SELECTED , and FileList is true"));
                      logMessage.prepend(QString("1_URL_SELECTED , and FileList is true,  Filelist size--> ") +
                                         QString::number(multi1KeywordList.size()));


                      filterCurrentKeyword = *currentKeywordPtr;
                      filterCurrentKeyword = filterCurrentKeyword.replace("+", " ");


                      /***********
                      If we  have more elements in list, and if so move to the next item

                      -If current value dosent match the keyword
                      - If current value dosent match the last item
                      Then theres more elements in last
                      ********/


                      // If Current value does not matches last item, theres more items
                      if (multi1KeywordList.value(*multi1CurrentKeywordPtr) != multi1KeywordList.last()
                              && multi1KeywordList.value(*multi1CurrentKeywordPtr) != multi1KeywordList.last().isEmpty())
                      {

                          (*multi1CurrentKeywordPtr) += 1;
                          *curlSingleProcessPtrCounter = 0;
                          logMessage.prepend(QString("1_URL_SELECTED, next keyword in multi1KeywordList--> "
                             + QString(multi1KeywordList.value(*multi1CurrentKeywordPtr).replace("+"," "))));

                      }

                      // Current value matches the last keyword, no more items
                      if (multi1KeywordList.value(*multi1CurrentKeywordPtr) == multi1KeywordList.last())
                      {
                          if (*multi1CurrentKeywordPtr > multi1KeywordList.size()) {
                              *multi1CurrentKeywordPtr = 0;  // just in case the pointer goes beyond fileList size()
                          }
                          *curlSingleProcessPtrCounter = 0;
                          multi1KeywordList.clear();
                          *multi1CurrentKeywordPtr= 0;
                          stopHarvestLoop = true;
                          emitFinishSenderHarvestResults("MULTI_SINGLE COMPLETED");
                          logHarvesterStatus.clear();
                          logMessage.prepend("1_URL_SELECTED, ALL_LIST_COMPLETED");
                          completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("MULTI_URL_SELECTED_RESULTS_COMPLETED").toString());

                          completeResultsFile.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/complete.json");
                          if(!completeResultsFile.open(QIODevice::WriteOnly)){
                              qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                              return;
                          }
                         completeResultsJsonDocument.setObject(completeResultsObj);
                         completeResultsFile.write(completeResultsJsonDocument.toJson());
                         completeResultsFile.close();
                      }

                  }// end of checking if fileList is empty


              }else
              {

                  completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("").toString());
                  completeResultsFile.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/complete.json");
                  if(!completeResultsFile.open(QIODevice::WriteOnly)){
                      qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                      return;
                  }
                 completeResultsJsonDocument.setObject(completeResultsObj);
                 completeResultsFile.write(completeResultsJsonDocument.toJson());
                 completeResultsFile.close();

              } // end of checking searchResultsPagesNumber




              /****
               if MULTI_URL_SELECTED is selected ,
               and multi counter is equal to search results number,
               either stop harvest, or  loop through all keywords until all keywords are done
              ***/

              if (QString::number(*curlMultiProcessPtrCounter) == QString(searchResultsPagesNumber) && *multiURLOption == "MULTI_URL_SELECTED")

              {

                  /*********Get current keyword within json file*******/
//                  QString currentKeywordJsonVal;
//                  QFile currentKeywordFile;
//                   currentKeywordFile.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/currentkeywords.json");
//                  if(! currentKeywordFile.open(QIODevice::ReadOnly | QIODevice::Text))
//                  {
//                      qDebug() << "Error opening  currentKeywordFile in webview worker" <<  currentKeywordFile.errorString();
//                  }
//                  currentKeywordJsonVal =  currentKeywordFile.readAll();
//                  currentKeywordFile.close();

//                  QJsonDocument doc = QJsonDocument::fromJson(currentKeywordJsonVal.toUtf8());
//                  QJsonObject jsonObject = doc.object();
//                  QJsonValue currentJsonKeywords = jsonObject.value(QString("CurrentKeywords"));

                      filterMulti1CurrentKeyword = multi1CurrentKeyword;
                      filterMulti1CurrentKeyword = filterMulti1CurrentKeyword.replace("+", " ");

                      filterMulti2CurrentKeyword = multi2CurrentKeyword;
                      filterMulti2CurrentKeyword = filterMulti2CurrentKeyword.replace("+", " ");

                      filterMulti3CurrentKeyword = multi3CurrentKeyword;
                      filterMulti3CurrentKeyword = filterMulti3CurrentKeyword.replace("+", " ");

                      filterMulti4CurrentKeyword = multi4CurrentKeyword;
                      filterMulti4CurrentKeyword = filterMulti4CurrentKeyword.replace("+", " ");

                      if(searchResultsOptions == QString::number(DEFAULT_SEARCH_RESULT))
                      {
                         logMessage.prepend("Multi  is True");
                         *curlMultiProcessPtrCounter = 0;
                          stopHarvestLoop = true;

                          completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("MULTI_URL_SELECTED_COMPLETED_USING_DEFAULT_SEARCH_OPTION").toString());

                          completeResultsFile.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/complete.json");
                          if(!completeResultsFile.open(QIODevice::WriteOnly)){
                              qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                              return;
                          }
                         completeResultsJsonDocument.setObject(completeResultsObj);
                         completeResultsFile.write(completeResultsJsonDocument.toJson());
                         completeResultsFile.close();
                      }




                        if(searchResultsOptions == QString::number(BYPASS_SEARCH_RESULT))
                        {

                            /*****
                               Open json file to get size of currentkeywords as its increasing
                               to see if the size matches the total amount of keywords
                             *****/

                            logMessage.prepend("Multi Bypass is True");


                            /***********
                            If we  have more elements in list, and if so move to the next item
                            -If current value dosent match the keyword
                            - If current value dosent match the last item
                            Then theres more elements in last
                            ********/

                            // If Current value does not match last item, theres more items
                            if(!multi1KeywordList.isEmpty())
                            {
                                if (multi1KeywordList.value(*multi1CurrentKeywordPtr) != multi1KeywordList.last() &&
                                    multi1KeywordList.value(*multi1CurrentKeywordPtr) != multi1KeywordList.last().isEmpty())
                                {
                                    (*multi1CurrentKeywordPtr) += 1;
                                    *curlMultiProcessPtrCounter = 0;
                                    logMessage.prepend(QString("More items, next multi1KeywordList keyword --> " + QString(multi1CurrentKeyword.replace("+"," "))));
                                }
                                // Current value matches the last keyword, no more items
                                if (multi1KeywordList.value(*multi1CurrentKeywordPtr) == multi1KeywordList.last())
                                {
                                    if (*multi1CurrentKeywordPtr >multi1KeywordList.size())
                                    {
                                        *multi1CurrentKeywordPtr = 0;  // just in case the pointer goes beyond  size()
                                    }
                                    multi1KeywordList.clear();
                                    *multi1CurrentKeywordPtr = 0;
                                    emit emitFinishSenderHarvestResults("MULTI COMPLETED");
                                    logHarvesterStatus.clear();
                                    logMessage.prepend(QString("multi1keywordList harvest complete" ));
                                    isMulti1KeywordListComplete =true;
                                }
                            }


                            // If Current value does not matches last item, theres more items
                            if(!multi2KeywordList.isEmpty())
                            {
                                if (multi2KeywordList.value(*multi2CurrentKeywordPtr) != multi2KeywordList.last() &&
                                    multi2KeywordList.value(*multi2CurrentKeywordPtr) != multi2KeywordList.last().isEmpty())
                                {
                                    (*multi2CurrentKeywordPtr) += 1;
                                    *curlMultiProcessPtrCounter = 0;
                                    logMessage.prepend(QString("More items, next multi2KeywordList keyword -->" + QString(multi2CurrentKeyword.replace("+"," "))));

                                }

                                // Current value matches the last keyword, no more items
                                if (multi2KeywordList.value(*multi2CurrentKeywordPtr) == multi2KeywordList.last())
                                {
                                    if (*multi2CurrentKeywordPtr >multi2KeywordList.size())
                                    {
                                        *multi2CurrentKeywordPtr = 0;  // just in case the pointer goes beyond  size()
                                    }
                                    multi2KeywordList.clear();
                                    *multi2CurrentKeywordPtr = 0;
                                    emit emitFinishSenderHarvestResults("MULTI COMPLETED");
                                    logHarvesterStatus.clear();
                                    logMessage.prepend(QString("multi2keywordList harvest complete" ));
                                    isMulti2KeywordListComplete =true;


                                }

                            }


                            // If Current value does not matches last item, theres more items
                            if(!multi3KeywordList.isEmpty())
                            {
                                if (multi3KeywordList.value(*multi3CurrentKeywordPtr) != multi3KeywordList.last() &&
                                    multi3KeywordList.value(*multi3CurrentKeywordPtr) != multi3KeywordList.last().isEmpty())
                                {
                                    (*multi3CurrentKeywordPtr) += 1;
                                    *curlMultiProcessPtrCounter = 0;
                                    logMessage.prepend(QString("More items, next multi3KeywordList keyword --> " + QString(multi3CurrentKeyword.replace("+"," "))));


                                }
                                // Current value matches the last keyword, no more items
                                if (multi3KeywordList.value(*multi3CurrentKeywordPtr) == multi3KeywordList.last())
                                {
                                    if (*multi3CurrentKeywordPtr >multi3KeywordList.size()) {
                                        *multi3CurrentKeywordPtr = 0;  // just in case the pointer goes beyond  size()
                                    }
                                    *curlMultiProcessPtrCounter = 0;
                                    multi3KeywordList.clear();
                                    *multi3CurrentKeywordPtr = 0;
                                     stopHarvestLoop = true;
                                    emit emitFinishSenderHarvestResults("MULTI COMPLETED");
                                    logHarvesterStatus.clear();
                                    logMessage.prepend(QString("multi3keywordList harvest complete" ));
                                    isMulti3KeywordListComplete =true;


                                }
                            }


                            if(!multi4KeywordList.isEmpty())
                            {
                                // If Current value does not matches last item, theres more items
                                if (multi4KeywordList.value(*multi4CurrentKeywordPtr) != multi4KeywordList.last() &&
                                    multi4KeywordList.value(*multi4CurrentKeywordPtr) != multi4KeywordList.last().isEmpty())
                                {
                                    (*multi4CurrentKeywordPtr) += 1;
                                    *curlMultiProcessPtrCounter = 0;
                                    logMessage.prepend(QString("More items, next multi4KeywordList keyword --> " + QString(multi4CurrentKeyword.replace("+"," "))));


                                }
                                // Current value matches the last keyword, no more items
                                if (multi4KeywordList.value(*multi4CurrentKeywordPtr) == multi4KeywordList.last())
                                {
                                    if (*multi4CurrentKeywordPtr >multi4KeywordList.size())
                                    {
                                        *multi4CurrentKeywordPtr = 0;  // just in case the pointer goes beyond  size()
                                    }
                                    multi4KeywordList.clear();
                                    *multi4CurrentKeywordPtr = 0;
                                    emit emitFinishSenderHarvestResults("MULTI COMPLETED");
                                    logHarvesterStatus.clear();
                                    logMessage.prepend(QString("multi4keywordList harvest complete" ));
                                    isMulti4KeywordListComplete =true;

                                }
                            }


                            /*******If all list are completed******/
                            if(isMulti1KeywordListComplete == true && isMulti2KeywordListComplete == true
                              && isMulti3KeywordListComplete == true && isMulti4KeywordListComplete == true)
                            {

                                logMessage.prepend("ALL_LIST_COMPLETED");
                                *curlMultiProcessPtrCounter = 0;
                                stopHarvestLoop = true;
                                completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("MULTI_URL_SELECTED_ALL_LIST_COMPLETED_USING_BYPASS_OPTION").toString());

                                completeResultsFile.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/complete.json");
                                if(!completeResultsFile.open(QIODevice::WriteOnly)){
                                    qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                                    return;
                                }
                               completeResultsJsonDocument.setObject(completeResultsObj);
                               completeResultsFile.write(completeResultsJsonDocument.toJson());
                               completeResultsFile.close();


                            }

                        }// end if bypass option is true


              }// end if multi counter matches search result number





              QEventLoop loop;
              QTimer::singleShot(9000, &loop, SLOT(quit()));
              loop.exec();

              QFile logFile_("C:/Users/ace/Documents/QT_Projects/WebView/WebView/log.txt");
              if (!logFile_.open(QIODevice::WriteOnly))
              {
                  qDebug() << "unable to open file"<< logFile_.errorString();
                  return;
              }
              QTextStream outStream(&logFile_);
              for(int i=0; i < logMessage.size(); i++)
              {
                  outStream << logMessage.value(i) << "\n";
              }
              logFile_.close();

          }// end of for loop

   }

     //emit finished();
}


void Worker::myLogFile(QString logMessage,int logInt)
{
//    logFile = new QFile();
//    logFile->setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/log.txt");
//    if(!logFile->open(QIODevice::WriteOnly | QIODevice::Text)){
//        qDebug() << "Error opening logfile in webview worker";
//        return;
//    }
//    textStreamLog = new QTextStream();
//    textStreamLog->setDevice(logFile);
//    textStreamLog->operator<<(QString("Log:: ") + logMessage +" "+ QString::number(logInt)+ QString("\n")) ;
//    *textStreamLog <<  logMessage << "\n";
//    logFile->close();

    QFile file;
    file.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/log.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error opening logfile in webview worker";
        return;
    }
    QTextStream ts(&file);
    ts<< logMessage << "\n";
  //  file.close();
}


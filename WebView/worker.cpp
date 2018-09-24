
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
    QString logFileName = getRelativePath(".log");

   logFile = new QFile(logFileName);
   if (!logFile->open(QIODevice::WriteOnly))
   {
       qDebug() << "unable to open file"<< logFile->errorString();
       return;
   }

   QString httpStatusLogFileName = getRelativePath("httpstatus.log");
   httpStatusFile = new QFile(httpStatusLogFileName);
   if (!httpStatusFile->open(QIODevice::WriteOnly))
   {
       qDebug() << "unable to open file"<< httpStatusFile->errorString();
       return;
   }

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
    logMessage = new QStringList();
    isResultsComplete =false;

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

    delete logMessage;
    logFile->close();
    delete logFile;

    httpStatusFile->close();
    delete httpStatusFile;

    harvestStatus("HARVEST_ABORTED");

}

void Worker::stop()
{
     stopHarvestLoop = true;
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
    QString paramsJsonFile = getRelativePath("params.json");

    file.setFileName(paramsJsonFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening json file in webview " <<  file.errorString();
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
    harvestStatus("INITIALIZING_HARVEST");
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
  //  std::chrono::seconds s(harvesterTimer);
  //  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(s);
    // cast long long to int
  //  appTimer = static_cast<int>(ms.count());
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


        QString multi1KeywordFileName = getRelativePath("multi1KeywordList.txt");

        QFile keywordFile1(multi1KeywordFileName);
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

        QString multi1KeywordFileName = getRelativePath("multi1KeywordList.txt");


        QFile keywordFile1(multi1KeywordFileName);
        if(!keywordFile1.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Error opening keywordlist file 1 in worker";
            return;
        }


        QString multi2KeywordFileName = getRelativePath("multi2KeywordList.txt");

        QFile keywordFile2(multi2KeywordFileName);
        if(!keywordFile2.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Error opening keywordlist file 2 in worker";
            return;
        }

        QString multi3KeywordFileName = getRelativePath("multi3KeywordList.txt");

        QFile keywordFile3(multi3KeywordFileName);
        if(!keywordFile3.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Error opening keywordlist file 3 in worker";
            return;
        }

        QString multi4KeywordFileName = getRelativePath("multi4KeywordList.txt");

        QFile keywordFile4(multi4KeywordFileName);
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
       for (;;)
          {

           harvestStatus("HARVEST_BUSY");


              //parsedEmails();
              if ( stopHarvestLoop) {
                  logMessage->prepend("Harvested loop has canceled");
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
               logMessage->prepend(QString::number(*curlMultiProcessPtrCounter));




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




               /***************Yahoo***************/


              if (searchEngine == "http://yahoo.com")
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
                      searchEngineParam = "https://search.yahoo.com/search;_ylt=A2KIbNDlJS1b7nIAYNNx.9w4;_ylu=X3oDMTFjN3E2bWhuBGNvbG8DYmYxBHBvcwMxBHZ0aWQDVUkyRkJUMl8xBHNlYwNwYWdpbmF0aW9u?p="
                           + socialNetWork + "%20"
                          + email + "%20" + currentKeywordSearchBoxKeyword.replace(" ", "+") +
                          "&ei=UTF-8&fr=yfp-hrmob&fr2=p%3Afp%2Cm%3Asb&_tsrc=yfp-hrmob&fp=1&b=11&pz=" + castSearchQueryNumPtr + "&xargs=0";

                  }

                  if (!multi1KeywordList.isEmpty() && !multi1KeywordList.isEmpty())
                  {

                      searchEngineParam = "https://search.yahoo.com/search;_ylt=A2KIbNDlJS1b7nIAYNNx.9w4;_ylu=X3oDMTFjN3E2bWhuBGNvbG8DYmYxBHBvcwMxBHZ0aWQDVUkyRkJUMl8xBHNlYwNwYWdpbmF0aW9u?p="
                          + socialNetWork + "%20"
                          + email + "%20" + currentKeywordPtr->replace(" ", "+") +
                          "&ei=UTF-8&fr=yfp-hrmob&fr2=p%3Afp%2Cm%3Asb&_tsrc=yfp-hrmob&fp=1&b=11&pz=" + castSearchQueryNumPtr + "&xargs=0";

                      // if keyword list is uploaded but user has selected multi  option
                      if (*multiURLOption == "MULTI_URL_SELECTED")
                      {
                          // if multi option is selected, assign url to multiparam1 and multiparam2
                          searchEngineParamMulti1 = "https://search.yahoo.com/search;_ylt=A2KIbNDlJS1b7nIAYNNx.9w4;_ylu=X3oDMTFjN3E2bWhuBGNvbG8DYmYxBHBvcwMxBHZ0aWQDVUkyRkJUMl8xBHNlYwNwYWdpbmF0aW9u?p="
                              + socialNetWork + "%20"
                              + email + "%20" + multi1CurrentKeyword.replace(" ", "+") +
                              "&ei=UTF-8&fr=yfp-hrmob&fr2=p%3Afp%2Cm%3Asb&_tsrc=yfp-hrmob&fp=1&b=11&pz=" + castSearchQueryNumPtr + "&xargs=0";

                          searchEngineParamMulti2 = "https://search.yahoo.com/search;_ylt=A2KIbNDlJS1b7nIAYNNx.9w4;_ylu=X3oDMTFjN3E2bWhuBGNvbG8DYmYxBHBvcwMxBHZ0aWQDVUkyRkJUMl8xBHNlYwNwYWdpbmF0aW9u?p="
                               + socialNetWork + "%20"
                              + email + "%20" + multi2CurrentKeyword.replace(" ", "+") +
                              "&ei=UTF-8&fr=yfp-hrmob&fr2=p%3Afp%2Cm%3Asb&_tsrc=yfp-hrmob&fp=1&b=11&pz=" + castSearchQueryNumPtr + "&xargs=0";
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

              }// end of checking if search engine is Yahoo







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
              QString currentKeywordFileName = getRelativePath("currentkeywords.json");

               currentKeywordFile.setFileName(currentKeywordFileName);
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



               harvestStatus("HARVEST_SUCCESSFULLY_SCRAPING");

               myLogFile();
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
                      logMessage->prepend(QString("1_URL_SELECTED, FileList is Empty and Default Search Options is true"));
                      logMessage->prepend(QString("1_URL_SELECTED, using keywordbox, keyword--> " +currentKeywordSearchBoxKeyword.replace("+"," ")));
                      logMessage->prepend(QString("Single Counter--> ") + QString::number(*curlSingleProcessPtrCounter));
                      completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("1_URL_SELECTED_RESULTS_COMPLETED").toString());
                      QString completeFileName = getRelativePath("complete.json");

                      completeResultsFile.setFileName(completeFileName );
                      if(!completeResultsFile.open(QIODevice::WriteOnly)){
                          qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                          return;
                      }
                     completeResultsJsonDocument.setObject(completeResultsObj);
                     completeResultsFile.write(completeResultsJsonDocument.toJson());
                     completeResultsFile.close();
                     *curlSingleProcessPtrCounter = 0;
                     stopHarvestLoop = true;
                  }


                  //Stop harvest if 1_URL_SELECTED, DEFAULT_SEARCH_RESULT is selected,  but list was uploaded
                  if (!multi1KeywordList.isEmpty() && searchResultsOptions == QString::number(DEFAULT_SEARCH_RESULT))
                  {
                      logMessage->prepend(QString("1_URL_SELECTED, FileList is Not Empty and Default Search Options is true"));
                      logMessage->prepend(QString("1_URL_SELECTED, using multi1KeywordList, keyword--> " + QString(multi1KeywordList.value(*multi1CurrentKeywordPtr))));

                      completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("1_URL_SELECTED_RESULTS_COMPLETED").toString());
                      QString completeFileName = getRelativePath("complete.json");

                      completeResultsFile.setFileName(completeFileName);
                      if(!completeResultsFile.open(QIODevice::WriteOnly)){
                          qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                          return;
                      }
                     completeResultsJsonDocument.setObject(completeResultsObj);
                     completeResultsFile.write(completeResultsJsonDocument.toJson());
                     completeResultsFile.close();
                     *curlSingleProcessPtrCounter = 0;
                     stopHarvestLoop = true;
                  }

                  /*************
                   If list is uploaded, and 1_URL_SELECTED is selected,
                   loop through all keywords, then stop harvest

                   ************/
                  if (!multi1KeywordList.isEmpty() && searchResultsOptions == QString::number(BYPASS_SEARCH_RESULT))
                  {
                      logMessage->prepend(QString("1_URL_SELECTED , and FileList is true"));
                      logMessage->prepend(QString("1_URL_SELECTED , and FileList is true,  Filelist size--> ") +
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
                          logMessage->prepend(QString("1_URL_SELECTED, next keyword in multi1KeywordList--> "
                             + QString(multi1KeywordList.value(*multi1CurrentKeywordPtr).replace("+"," "))));

                      }

                      // Current value matches the last keyword, no more items
                      if (multi1KeywordList.value(*multi1CurrentKeywordPtr) == multi1KeywordList.last())
                      {
                          if (*multi1CurrentKeywordPtr > multi1KeywordList.size()) {
                              *multi1CurrentKeywordPtr = 0;  // just in case the pointer goes beyond fileList size()
                          }
                          multi1KeywordList.clear();
                          *multi1CurrentKeywordPtr= 0;
                          logMessage->prepend("1_URL_SELECTED, ALL_LIST_COMPLETED");
                          completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("MULTI_URL_SELECTED_RESULTS_COMPLETED").toString());

                          QString completeFileName = getRelativePath("complete.json");

                          completeResultsFile.setFileName(completeFileName);
                          if(!completeResultsFile.open(QIODevice::WriteOnly)){
                              qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                              return;
                          }
                         completeResultsJsonDocument.setObject(completeResultsObj);
                         completeResultsFile.write(completeResultsJsonDocument.toJson());
                         completeResultsFile.close();
                         stopHarvestLoop = true;
                         *curlSingleProcessPtrCounter = 0;

                      }

                  }// end of checking if fileList is empty

                  myLogFile();

              }else
              {


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
                         logMessage->prepend("Multi  is True");

                          completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("MULTI_URL_SELECTED_COMPLETED_USING_DEFAULT_SEARCH_OPTION").toString());

                          QString completeFileName = getRelativePath("complete.json");

                          completeResultsFile.setFileName(completeFileName);
                          if(!completeResultsFile.open(QIODevice::WriteOnly)){
                              qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                              return;
                          }
                         completeResultsJsonDocument.setObject(completeResultsObj);
                         completeResultsFile.write(completeResultsJsonDocument.toJson());
                         completeResultsFile.close();
                         *curlMultiProcessPtrCounter = 0;
                         stopHarvestLoop = true;

                      }




                        if(searchResultsOptions == QString::number(BYPASS_SEARCH_RESULT))
                        {

                            /*****
                               Open json file to get size of currentkeywords as its increasing
                               to see if the size matches the total amount of keywords
                             *****/

                            logMessage->prepend("Multi Bypass is True");


                            /***********
                            If we  have more elements in list, and if so move to the next item
                            -If current value dosent match the keyword
                            - If current value dosent match the last item
                            Then theres more elements in last
                            ********/

                            // If Current value does not match last item, theres more items
                            if(!multi1KeywordList.isEmpty())
                            {
                                if (multi1KeywordList.value(*multi1CurrentKeywordPtr) != multi1KeywordList.last())
                                {
                                    (*multi1CurrentKeywordPtr) += 1;
                                    *curlMultiProcessPtrCounter = 0;
                                    logMessage->prepend(QString("More items, next multi1KeywordList keyword --> " + QString(multi1KeywordList.value(*multi1CurrentKeywordPtr).replace("+"," "))));
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
                                    logMessage->prepend(QString("multi1keywordList harvest complete" ));
                                    isMulti1KeywordListComplete =true;
                                }
                            }


                            // If Current value does not matches last item, theres more items
                            if(!multi2KeywordList.isEmpty())
                            {
                                if (multi2KeywordList.value(*multi2CurrentKeywordPtr) != multi2KeywordList.last())
                                {
                                    (*multi2CurrentKeywordPtr) += 1;
                                    *curlMultiProcessPtrCounter = 0;
                                    logMessage->prepend(QString("More items, next multi2KeywordList keyword -->"
                                    + QString(multi2KeywordList.value(*multi2CurrentKeywordPtr).replace("+"," "))));

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
                                    logMessage->prepend(QString("multi2keywordList harvest complete" ));
                                    isMulti2KeywordListComplete =true;


                                }

                            }


                            // If Current value does not matches last item, theres more items
                            if(!multi3KeywordList.isEmpty())
                            {
                                if (multi3KeywordList.value(*multi3CurrentKeywordPtr) != multi3KeywordList.last())
                                {
                                    (*multi3CurrentKeywordPtr) += 1;
                                    *curlMultiProcessPtrCounter = 0;
                                    logMessage->prepend(QString("More items, next multi3KeywordList keyword --> " +
                                                    QString(multi3KeywordList.value(*multi3CurrentKeywordPtr).replace("+"," "))));


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
                                    logMessage->prepend(QString("multi3keywordList harvest complete" ));
                                    isMulti3KeywordListComplete =true;


                                }
                            }


                            if(!multi4KeywordList.isEmpty())
                            {
                                // If Current value does not matches last item, theres more items
                                if (multi4KeywordList.value(*multi4CurrentKeywordPtr) != multi4KeywordList.last() )
                                {
                                    (*multi4CurrentKeywordPtr) += 1;
                                    *curlMultiProcessPtrCounter = 0;
                                    logMessage->prepend(QString("More items, next multi4KeywordList keyword --> " +
                                                                QString(multi2KeywordList.value(*multi2CurrentKeywordPtr).replace("+"," "))));


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
                                    logMessage->prepend(QString("multi4keywordList harvest complete" ));
                                    isMulti4KeywordListComplete =true;

                                }
                            }


                            /*******If all list are completed******/
                            if(isMulti1KeywordListComplete == true && isMulti2KeywordListComplete == true
                              && isMulti3KeywordListComplete == true && isMulti4KeywordListComplete == true)
                            {

                                logMessage->prepend("ALL_LIST_COMPLETED");
                                *curlMultiProcessPtrCounter = 0;
                                stopHarvestLoop = true;
                                completeResultsObj.insert("COMPLETED_RESULTS",QJsonValue("MULTI_URL_SELECTED_ALL_LIST_COMPLETED_USING_BYPASS_OPTION").toString());

                                QString completeFileName = getRelativePath("complete.json");

                                completeResultsFile.setFileName(completeFileName);
                                if(!completeResultsFile.open(QIODevice::WriteOnly)){
                                    qDebug() << "Error opening complete.json in webview worker" << completeResultsFile.errorString();
                                    return;
                                }
                               completeResultsJsonDocument.setObject(completeResultsObj);
                               completeResultsFile.write(completeResultsJsonDocument.toJson());
                               completeResultsFile.close();


                            }

                            myLogFile();
                        }// end if bypass option is true



              }// end if multi counter matches search result number


              QEventLoop loop;

              QTimer::singleShot(harvesterTimer*1000, &loop, SLOT(quit()));
              loop.exec();



          }// end of for loop

   }

     //emit finished();
}

void Worker::harvestStatus(QString status)
{
    QFile harvestStatusFile;
    QJsonDocument harvestStatusDocument;
    QJsonObject  harvestStatusObject;
    harvestStatusObject.insert("HarvestStatus",QJsonValue(status).toString());
    QString completeFileName = getRelativePath("complete.json");

    harvestStatusFile.setFileName(completeFileName);
    if(!harvestStatusFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Error opening json file in webview " << harvestStatusFile.errorString();
    }
    harvestStatusDocument.setObject(harvestStatusObject);
    harvestStatusFile.write(harvestStatusDocument.toJson());
    harvestStatusFile.close();

}

void Worker::myLogFile()
{
    QTextStream outStream(logFile);
    for(int i=0; i < logMessage->size(); i++)
    {
        outStream << logMessage->value(i) << "\n";
    }
}

void Worker::receiverWebViewLog(QString webViewLogMessage)
{

   httpStatusList <<  webViewLogMessage;
   QTextStream outStream(httpStatusFile);
   for(int i=0; i < httpStatusList.size(); i++)
   {
       outStream <<httpStatusList.value(i) << "\n";
       httpStatusList.removeAt(i);
   }


}

void Worker::myLogFile_(QString logMessage,int logInt)
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
    QString httpStatusFileName = getRelativePath("log.txt");

    file.setFileName(httpStatusFileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error opening logfile in webview worker";
        return;
    }
    QTextStream ts(&file);
    ts<< logMessage << "\n";
  //  file.close();
}

QString Worker::getRelativePath(QString fileName)
{
    /*********
    RECAP OF RECONFIGURING WBVIEW BUILD LOCATION
    Webview build is now in
    C:\Users\ace\Documents\QT_Projects\WebView Controller\build-BeatCrawler-Desktop_Qt_5_9_4_MSVC2015_64bit2-Debug\debug\resources\webview\debug
    - this is so when we eventually build and deploy the web view controller we can easily access the web crawler like /resources/Webview.exe etc..

    all json,.log, and .txt files are read using getRelativePath() function
    this function either moves up a dir, or is the root dir of the .exe,
    depending on whether its being used in webcrawler controller, or webview

    For some strange reason, when debugging, several .log, .json, and .txt files
    are being copied outside of the application folder, in webview controller.
    Once we fix this, we are good to deploy program.


    **********/
//    QDir tempCurrDir  = QDir::current();
//    tempCurrDir.cdUp();
//    QString root = tempCurrDir.path();
//    return root + "/" +fileName;

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

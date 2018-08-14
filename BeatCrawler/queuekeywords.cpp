#include "queuekeywords.h"
#include "ui_queuekeywords.h"

QueueKeywords::QueueKeywords(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueueKeywords),
    fileList(new QStringList()),
    keywordQueueItem(new QTableWidgetItem())
{
    ui->setupUi(this);
    QSize size = this->size();
    this->setFixedSize(size);
    setWindowTitle("Queue Keywords Status");


    connect(this,SIGNAL(emitDisplayCurrentKeywords(QString,QString,QString,QString)),
            this,SLOT(receiverDisplayCurrentKeywords(QString,QString,QString,QString)));

    connect(this,SIGNAL(emitDisplayCurrentKeyword(QString)),
            this,SLOT(receiverDisplayCurrentKeyword(QString)));

    connect(this,SIGNAL(emitKeywordsQueueList()),this,SLOT(receiverKeywordsQueueList()));



}

QueueKeywords::~QueueKeywords()
{
    delete ui;
    delete fileList;
    delete keywordQueueItem;
}

void QueueKeywords::openCurrentKeywordJsonFile()
{
    QString val;
    QFile file;
    file.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/currentkeywords.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening json file in queue keywords table";
    }
    val = file.readAll();
    file.close();

    QVector<QString>vectorCurrentKeywords;
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObject = doc.object();
    QJsonValue currentKeywords = jsonObject.value(QString("CurrentKeywords"));
    QJsonValue currentKeyword = jsonObject.value(QString("CurrentKeyword")).toString();

    QJsonArray currentOptionsArray = currentKeywords.toArray();


    for(int i =0; i <  currentOptionsArray .size(); i++ )
    {
        vectorCurrentKeywords.insert(i, currentOptionsArray [i].toString());
    }

    if(!vectorCurrentKeywords.isEmpty())
    {
       // qDebug() <<"Vector--" <<vectorCurrentKeywords;
        emit emitDisplayCurrentKeywords(vectorCurrentKeywords.value(0),vectorCurrentKeywords.value(1),
                               vectorCurrentKeywords.value(2),vectorCurrentKeywords.value(3));

    }

    if(!currentKeyword.toString().isEmpty())
    {
       // qDebug() << "String--"<< currentKeyword.toString();
        emit emitDisplayCurrentKeyword( currentKeyword.toString());
    }
    vectorCurrentKeywords.clear();

}


void QueueKeywords::receiverDisplayCurrentKeywords(QString keyword1,QString keyword2, QString keyword3, QString keyword4)
{

    currentKeywordList1 =keyword1;
    currentKeywordList2 =keyword2;
    currentKeywordList3 =keyword3;
    currentKeywordList4 =keyword4;

    /*********
    Prevents dupelicates from inserting list widget.
    If last  currentKeyword that was added to listwidget is  equal to the keyword,
    from the keyword argument, clear it, and make it null.
    ********/
    if(lastListKeyword1 == currentKeywordList1)
    {
        currentKeywordList1.clear();
    }
    if(lastListKeyword2 == currentKeywordList2)
    {
        currentKeywordList2.clear();
    }
    if(lastListKeyword3 == currentKeywordList3)
    {
        currentKeywordList3.clear();
    }
    if(lastListKeyword4 == currentKeywordList4)
    {
        currentKeywordList4.clear();
    }
}

void QueueKeywords::receiverDisplayCurrentKeyword(QString keyword)
{

     currentKeyword =keyword;
     if(lastKeyword == currentKeyword)
     {
         currentKeyword.clear();
     }
}



void QueueKeywords::receiverKeywordsQueueList()
{

   if(!currentKeywordList1.isEmpty() && !currentKeywordList1.isEmpty()
           && !currentKeywordList1.isEmpty() && !currentKeywordList1.isEmpty())
   {

       ui->listWidget_Keywords_Queue->addItem(currentKeywordList1 + QString(": Completed"));
       ui->listWidget_Keywords_Queue->addItem(currentKeywordList2 + QString(": Completed"));
       ui->listWidget_Keywords_Queue->addItem(currentKeywordList3 + QString(": Completed"));
       ui->listWidget_Keywords_Queue->addItem(currentKeywordList4 + QString(": Completed"));

       lastListKeyword1 = currentKeywordList1;
       lastListKeyword2 = currentKeywordList2;
       lastListKeyword3 = currentKeywordList3;
       lastListKeyword4 = currentKeywordList4;
   }

   if(!currentKeyword.isEmpty())
   {
       ui->listWidget_Keywords_Queue->addItem(currentKeyword +QString(": Completed"));
       lastKeyword = currentKeyword;
   }

}

void QueueKeywords::receiverQueueKeywordListFile(QString keywords)
{
  // *fileList << keywords;

//    for (int row = 0; row < fileList->size(); row++)
//    {
//        for (int col = 0; col < 2; col++)
//        {
//            if (col == 0) {
//                ui->tableWidget_Keywords_Queue->setItem(row, col, new QTableWidgetItem(fileList->value(row)));
//            }
//            if (col == 1) {
//                ui->tableWidget_Keywords_Queue->setItem(row, col, new QTableWidgetItem(""));
//            }
//        }


//    }// end for loop


}

void QueueKeywords::getCurrentKeywords()
{
    wStop = false;

    for(;;)
    {

        if(wStop){
            ui->listWidget_Keywords_Queue->clear();

            break;
        }
        openCurrentKeywordJsonFile();
        emit emitKeywordsQueueList();
        QEventLoop loop;
        QTimer::singleShot(5000,&loop,SLOT(quit()));
        loop.exec();

    }

}

void QueueKeywords::receiverStopQueueKeywords()
{
    wStop = true;
}

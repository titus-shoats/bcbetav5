#include "worker.h"

Worker::Worker(QObject * parent)
    :QObject (parent),
     httpStatusLogFile(new QFile()),
      queryModel(new QSqlQueryModel()),
      socket(new QLocalSocket(this))

{
   openhttpStatusFile(httpStatusLogFile);
   wStop = false;
   connOpen();
   in.setDevice(socket);
   in.setVersion(QDataStream::Qt_5_9);
   connect(socket,&QLocalSocket::readyRead,this,&Worker::readModel);
   connect(socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
                this, &Worker::displaySocketError);


}

Worker::~Worker()
{
   httpStatusLogFile->close();
   delete httpStatusLogFile;
   connClose();
   delete queryModel;
   delete socket;
}

void Worker::openhttpStatusFile(QFile * httpStatusFile) const
{

    httpStatusFile->setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/httpstatus.log");
    if(! httpStatusFile->open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening keyword httpstatus.log " <<  httpStatusFile->errorString();
        return;
    }
}

void Worker::openCurrentKeywordJsonFile()
{
    QString val;
    QFile file;
    file.setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/currentkeywords.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening json file in beat crawler worker";
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
     //   qDebug() <<"Vector--" <<vectorCurrentKeywords;
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

void Worker::getEmailCount()
{

}

void Worker::getCurrentKeywords()
{
    QString str;
       wStop = false;
       for(;;)
       {

           if(wStop)
           {
               break;
           }
           QEventLoop loop;
           QTimer::singleShot(5000,&loop,SLOT(quit()));
           loop.exec();
           openCurrentKeywordJsonFile();
           emit emitLogHarvesterStatus(httpStatusLogFile->readLine());

       }
}



void Worker::testTimer()
{
//    for(int i = 0; i < 999999; i++)
//       {

//           QThread::currentThread()->msleep(15000);

//           for (int row =0; row < fileList->size(); row++)
//           {
//               for (int col = 0; col < 2; col++)
//               {
//                   emit emitKeywordsQueueTable(row,col);
//               }
//           }// end of for outer loop

//       }
}

void Worker::receiverStopWorker(){
    wStop = true;
}

void Worker::connOpen()
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

void Worker::connClose()
{
    mydb.close();
    mydb.removeDatabase(QSqlDatabase::defaultConnection);
}

void Worker::displaySocketError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError) {
        case QLocalSocket::ServerNotFoundError:
             qDebug() << "The host was not found. Please make sure "
                         "that the server is running and that the "
                         "server name is correct.";

            break;
        case QLocalSocket::ConnectionRefusedError:
             qDebug() << "The connection was refused by the peer. "
                         "Make sure the fortune server is running, "
                         "and check that the server name "
                         "is correct.";
            break;
        case QLocalSocket::PeerClosedError:
            qDebug() << "Peer closed";
            break;
        default:
           qDebug() << socket->errorString();
        }
}

void Worker::requestNewModel()
{
//  blockSize =0;
//  socket->abort();
//  socket->connectToServer("_server_");

//  if(socket->error() == QLocalSocket::UnknownSocketError)
//  {
//      qDebug() << "Error connectiong to QLocalSocket";
//  }else
//  {
//      qDebug() << socket->errorString();
//  }
}

void Worker::readModel()
{
    if(blockSize ==0)
    {
        // Relies on the fact that QDataStream serializes a quint32 into
        // sizeof(quint32) bytes
        if(socket->bytesAvailable() == (int)sizeof(quint32))
        {
            return;
        }
        /*******
         If blockSize block data is 0, and sockets bytes available is equal to 32 bytes return
         function, and exit function.
         Input socket stream from the server into the block  using: in >> blockSize
        ********/
        in >> blockSize;
    }

       /*******

         If socket bytes available is less than blockSize block data, or input socket stream is
         done inputing data, and its the end, exit function
       ********/
    if(socket->bytesAvailable() < blockSize || in.atEnd())
    {
        return;
    }

    /******Read input socket stream into a string, which is nextEmail******/
    QString nextModel;
    in >> nextModel;

    if(nextModel == currentString)
    {
       // QTimer::singleShot(0,this,&Worker::requestNewModel);
    }

    currentString = nextModel;
    qDebug() << currentString;


}

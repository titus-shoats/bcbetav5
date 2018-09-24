#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include <QTimer>
#include <QEventLoop>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QLocalSocket>
#include "config.h"
#include <QStandardPaths>
#include <QDir>




class Worker: public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent =0);
    ~Worker();
    void getCurrentKeywords() ;
    void openhttpStatusFile(QFile * httpStatusFile);
    void openCurrentKeywordJsonFile();
    void connOpen();
    void connClose();
    void getEmailCount();
    void testTimer();
    QString getRelativePath(QString fileName);



private:
    bool wStop;
    QFile *httpStatusLogFile;
    QTimer *timer;
    QStringList emailList;
    QSqlDatabase mydb;
    QSqlQueryModel *queryModel;
    QSqlQuery *qry;
    QLocalSocket *socket;
    QDataStream in;
    quint32 blockSize;
    QSqlQueryModel currentQueryModel;
    QStringList *fileList;
    QString currentString;


public slots:
    void receiverStopWorker();
    void readModel();
    void requestNewModel();
    void displaySocketError(QLocalSocket::LocalSocketError socketError);


signals:
    void emitEmailList(QString emailList);
    void emitDisplayCurrentKeywords(QString keyword1,QString keyword2, QString keyword3, QString keyword4);
    void emitDisplayCurrentKeyword(QString keyword);
    void emitKeywordsQueueTable();
    void emitEmailTableModel(QSqlQueryModel *queryModel);
    void emitLogHarvesterStatus(QString logStatus);


};

#endif // WORKER_H

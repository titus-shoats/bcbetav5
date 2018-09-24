/*
Copyright 2013 Fabien Pierre-Nicolas.
- Primarily authored by Fabien Pierre-Nicolas

This file is part of simple-qt-thread-example, a simple example to demonstrate how to use threads.
This example is explained on http://fabienpn.wordpress.com/qt-thread-simple-and-stable-with-sources/

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This progra is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QMutex>
#include <cstdio>
#include <string>
#include <iostream>
#include <QVector>
#include <stdio.h>
#include <stdlib.h>

#include <QTimer>
#include <QEventLoop>
#include <QThread>
#include <QDebug>
#include <QRegularExpressionMatchIterator>
#include <QTextDocumentFragment>
#include <QList>
#include <chrono>
#include <QFileInfo>
#include <QtConcurrent/QtConcurrent>
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVector>
#include "config.h"
#include <QStandardPaths>
#include <QDir>


class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(QObject *parent = 0);
    ~Worker();

    void startProcess();
    void startHarvest(QVector<QString>vectorSearchEngineOptions,
                    QVector<QString>vectorEmailOptions,QVector<QString>vectorSocialNetworks2,
                      QString lineEdit_keywords_search_box, QList <QString> *proxyServers,
                      QList<int>timerOptions, QList<QString>otherOptions,int isFileListUploaded);
    void initHarvest();
    void myLogFile_(QString logMessage,int logInt =NULL);
    void myLogFile();
    void harvestStatus(QString status);
    QString getRelativePath(QString fileName);


private:
    QString *proxies;
    int *workerCounterPtr;
    int workerCounterNum;

    int *proxyServerCounterPtr;
    int proxyServerCounterNum;

    bool isProxyEmpty;
    bool canProxyCounterIncrement;
    bool isStopStartThreadCounter;
    int * harvesterTimerPtr;
    int  harvesterTimerNum;

    int * proxyRotateIntervalPtr;
    int  proxyRotateIntervalNum;
    //static QStringList *parsedEmailList;
    //static const Worker *theWorker = new Worker();
    QList<QStringList>*httpRequestList;
    bool stopHarvestLoop;

    QVector<QString>vectorSearchEngineOptions;
    QVector<QString>vectorEmailOptions;
    QVector<QString>vectorSocialNetworks2;
    QList <QString> *proxyServers;

    QStringList *fileList;
    int *fileListPtr;
    int fileListNum;

    QStringList multi1KeywordList;
    QString multi1CurrentKeyword;
    int *multi1CurrentKeywordPtr;
    int multi1CurrentKeywordPtrNum;

    QStringList multi2KeywordList;
    QString multi2CurrentKeyword;
    int *multi2CurrentKeywordPtr;
    int multi2CurrentKeywordPtrNum;

    QStringList multi3KeywordList;
    QString multi3CurrentKeyword;
    int *multi3CurrentKeywordPtr;
    int multi3CurrentKeywordPtrNum;


    QStringList multi4KeywordList;
    QString multi4CurrentKeyword;
    int *multi4CurrentKeywordPtr;
    int multi4CurrentKeywordPtrNum;



    QString *currentKeywordPtr;
    QString currentKeyword;

    int *curlSingleProcessPtrCounter;
    int curlSingleProcessCounterNum;

    int *curlMultiProcessPtrCounter;
    int curlMultiProcessCounterNum;


    int *searchEngineNumPtr;
    int searchEngineNum;

    int *emailOptionsNumPtr;
    int emailOptionsNum;

    int *socialNetWorkNumPtr;
    int socialNetWorkNum;

    int *searchEnginePaginationCounterPtr;
    int searchEnginePaginationCounterNum;


    QList<QString>curlHTTPRequestList;
    int *curlHTTPRequestCounterPtr;
    int curlHTTPRequestCounterNum;
    QTimer *deleteKeywordCheckBoxTimer;
    QString *multiURLOption;
    QString multiURLOptionString;
    QString *multiOptionOneURL;
    QString multiOptionOneURLString;
    QTimer *timer;
    bool finishReadingMulti1KeywordFile;
    bool finishReadingMulti2KeywordFile;
    bool finishReadingMulti3KeywordFile;
    bool finishReadingMulti4KeywordFile;
    QFile *logFile;
    QFile *httpStatusFile;
    QTextStream *textStreamLog;
    QStringList *logMessage;
    bool isMulti1KeywordListComplete;
    bool isMulti2KeywordListComplete;
    bool isMulti3KeywordListComplete;
    bool isMulti4KeywordListComplete;
    QStringList httpStatusList;
    bool isResultsComplete;

signals:
    /**
    * @brief This signal is emitted when the Worker request to Work
    * @sa requestWork()
    */
    void workRequested();
    /**
    * @brief This signal is emitted when counted value is changed (every sec)
    */
    void valueChanged(const QString &value);

    void workRequested1();


    /**
    * @brief This signal is emitted when process is finished (either by counting 60 sec or being aborted)
    */
    void finished();

    void emitParameters();
    void emitEmailList1(QString emails, QString threadName);
    void emitMulti1EmailList(QString emails,QString threadName);
    void emitMulti2EmailList(QString emails, QString threadName);

    void emitKeywordQueue();
    void senderCurlResponseInfo(QString);
    void emitFinishSenderHarvestResults(QString results);
    void emitsendCurrentKeyword(QString keyword);
    void emitMultiEmailList(QString emails);
    void emitLoopWebViewParams(QString params);
    void emitMultiLoopWebViewParams(QString searchEngineParamMulti1,QString searchEngineParamMulti2,
                                    QString searchEngineParamMulti3,QString searchEngineParamMulti4);

public slots:

    void stop();
    void receiverWebViewLog(QString logMesssage);

};

#endif // WORKER_H

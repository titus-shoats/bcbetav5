#ifndef QueueKeywords_H
#define QueueKeywords_H

#include <QDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QTableWidgetItem>
#include <QTimer>
#include <QEventLoop>
#include <QMutex>
#include <QThread>
#include <QTextStream>
#include "config.h"
#include <QStandardPaths>
#include <QDir>



namespace Ui {
class QueueKeywords;
}

class QueueKeywords : public QDialog
{
    Q_OBJECT

public:
    explicit QueueKeywords(QWidget *parent = nullptr);
    ~QueueKeywords();
    void openCurrentKeywordJsonFile();
    void getCurrentKeywords();
    QString getRelativePath(QString fileName);



private:
    Ui::QueueKeywords *ui;
    QStringList * fileList;
    QString currentKeywordList1;
    QString currentKeywordList2;
    QString currentKeywordList3;
    QString currentKeywordList4;
    QString currentKeyword;
    QTableWidgetItem *keywordQueueItem;
    bool wStop;
    QMutex mutex;
    QStringList fileList1;
    QStringList fileList2;
    QStringList fileList3;
    QStringList fileList4;
    QStringList fileList5;
    QStringList fileList6;
    QStringList fileList7;
    QStringList fileList8;
    QStringList fileList9;
    QStringList fileList10;
    QByteArray ba;
    QList<QByteArray>byteArrayList;
    QFile file;
    QString lastListKeyword1;
    QString lastListKeyword2;
    QString lastListKeyword3;
    QString lastListKeyword4;
    QString lastKeyword;
    QStringList removeDuplicatKeywords;


public slots:
    void receiverKeywordsQueueList();
    void receiverQueueKeywordListFile(QString keywords);
    void receiverDisplayCurrentKeywords(QString keyword1,QString keyword2, QString keyword3, QString keyword4);
    void receiverDisplayCurrentKeyword(QString keyword);
    void receiverStopQueueKeywords();

signals:
    void emitKeywordsQueueList();
    void emitDisplayCurrentKeywords(QString keyword1,QString keyword2, QString keyword3, QString keyword4);
    void emitDisplayCurrentKeyword(QString keyword);

};

#endif // QueueKeywords_H

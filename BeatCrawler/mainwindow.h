#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include "options.h"
#include <QStandardItemModel>
#include <QEvent>
#include <cstdio>
#include <string>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QUrl>
#include <QThread>
#include <QItemSelection>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QtConcurrent/QtConcurrent>
#include <QListWidgetItem>
#include <QItemSelection>
#include <QJsonObject>
#include <QProcess>
#include <QDir>
#include "worker.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "queuekeywords.h"
#include <QMutex>
#include <QCloseEvent>
#include <config.h>

namespace Ui {
    class MainWindow;
}

typedef Options* OptionsPtr;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setSearchResults();
    void receiverParameters();
    void createMultiKeywordList();
    void connOpen();
    void connClose();
    void clearFiles();
    void closeEvent(QCloseEvent *event);
    QString getRelativePath(QString fileName);

private slots:


    void on_pushButton_Start_clicked(bool checked);
    void on_pushButton_Load_Keyword_List_clicked();
    void on_pushButton_Add_Proxy_clicked();
    void on_radioButton_Parse_Multi_URL_clicked(bool checked);
    void on_pushButton_Next_Email_Pagination_clicked();
    void on_pushButton_Previous_Email_Pagination_clicked();
    void on_checkBox_Delete_Emails_clicked();
    void on_checkBox_Delete_Keywords_clicked();
    void on_pushButton_Queue_Keywords_Table_clicked();
    void on_pushButton_Save_Emails_clicked();

public slots:
    void reEnableStartButton();
    void recieverProxyTableSelection(const QItemSelection &selected, const QItemSelection &deselected);
    void populateEmailTable();
    void completeHarvesterTimer();
    void deleteEmailsListTable();
    void receiverRemoveEmailList();
    void receiverEnableDeleteEmailCheckBox();
    void receiverEnableDeleteKeywordCheckBox();
    void deleteKeyordsListTable();
    void receiverRemoveKeywordList();
    void receiverDisplayCurrentKeywords(QString keyword1,QString keyword2, QString keyword3, QString keyword4);
    void receiverDisplayCurrentKeyword(QString keyword);
    void emailCount();
    void receiverEmailTableModel(QSqlQueryModel *queryModel);
    void receiverLogHarvesterStatus(QString logStatus);

signals:

    void emitStopQueueKeywords();
    void emitStopWorker();
    void emitRemoveEmailList();
    void emitSenderEnableDeleteEmailCheckBox();
    void emitRemoveKeywordList();
    void emitSenderEnableKeywordCheckBox();
    void emitEmailCount(int emailCount);
    void emitQueueKeywordListFile(QString keywords);


private:
    Ui::MainWindow *ui;
    bool *startScrape;
    bool stopScrape;
    bool changeBool;
    const int numOptions = 67;
    // Options pointer
    OptionsPtr *options;
    // Options objects
    Options *opt;
    static QStringList * emails;
    QTimer *  emailTableTimer;
    QTimer *  completeHarvestTimer;

    QTimer *keywordsQueueTableTimer;
    QTimer * emailCountTimer;
    int *emailOptionsNumPtr;
    int *searchEngineNumPtr;
    int *searchEngineNumPtr1[3];
    int *socialNetWorkNumPtr;
    int *keywordLoadListOptionsNumPtr;

    int *addProxyCounterPtr;
    int addProxyCounterNum;

    int *keywordBoxNumPtrCounter;
    int keywordBoxNumPtrNum;

    int *keywordListSearchEngineCounterPtr;
    int keywordListSearchEngineCounterNum;

    int *keywordSearchBoxSearchEngineCounterPtr;
    int keywordSearchBoxSearchEngineCounterNum;

    int keywordListNumPtrNum;
    int * keywordListNumPtrCounter;

    bool isSearchEngineSelectOne;
    bool  isSocialNetworkSelectOne;
    bool isEmailSelectOne;
    bool isKeywordsSelect;

    bool  isProxyPortValid;
    bool  isProxyHostValid;

    bool isMulti_KeywordBox_Selected;
    bool isMulti_KeywordBox_Bypass_Results_Selected;


    // sets QSet back to list
    // this is because were setting a qlist to qset to remove dups, then from a qset to qlist again
    QList<QString> setEmailList;
    QList <QString> *proxyServers;
    QList<QVector <QString>>vectorSearchOptions;
    QList<int>timerOptions;
    QList<QString>otherOptions;


    int emailOptionsNum;
    int searchEngineNum;
    int socialNetWorkNum;
    int keywordLoadListOptionsNum;
    bool * searchEngineNumPtrAdded;
    bool searchEngineNumPtrAddedBool;
    int *searchEngineNumPtrCounter;
    int  searchEngineNumPtrNum;
    QVector<QString>vectorSocialNetworks2;
    QVector<QString>vectorEmailOptions;
    QVector<QString>vectorSearchEngineOptions;
    QVector<QString>vector;
    QString *data;

    QUrl url;
    QString testLabel;
    // gets callback data from curl
    static QString * fetchWriteCallBackCurlData;
    QString fetchWriteCallBackCurlDataString;
    int * nextEmailPaginationPtr;
    int * previousEmailPaginationPtr;
    int nextEmailPagination;
    int previousEmailPagination;
    bool clickedStartStopButton;
    QTableWidgetItem *emailListItem;
    QTableWidgetItem *keywordQueueItem;
    QTableWidgetItem *addProxyItem;
    QStringList FileReadKeywordList;
    QVector<QString> keywordListLastItem;
    QStringList *fileList;
    int *fileListPtr;
    int fileListNum;
    bool keywordCompleted;
    QStandardItemModel *emailTableModel;
    int *previousPagesPtr;
    int previousPagesNum;
    QString *currentKeyword_;
    QString currentKeywordString_;
    int *logStatusCounterPtr;
    int logStatusCounterNum;
    bool finishReadingKeywordFile;
    QProcess *webViewProcess;

    Worker *worker;
    QSqlDatabase mydb;
    QSqlQueryModel *queryModel;
    QSqlQuery *qry;
    QStringList emailsInTable;
    int *emailRowStartPtr;
    int emailRowStartNum;
    int *emailRowEndPtr;
    int emailRowEndNum;
    QStringList multi1KeywordList;
    QStringList multi2KeywordList;
    QString currentKeywordList1;
    QString currentKeywordList2;
    QString currentKeywordList3;
    QString currentKeywordList4;
    QString currentKeyword;
    QMutex mutex;
    QueueKeywords *queueKeywords;
    QList<QString>removeDuplicatesFileList;
    int emailCountNum;
    QStringList emailListCount;


};

#endif // MainWindow_H

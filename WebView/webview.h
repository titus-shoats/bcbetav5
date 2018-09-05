#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include <QTime>
#include <QNetworkReply>
#include <QtDebug>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QThread>
#include <QTimer>
#include <QTextDocumentFragment>
#include <QRegularExpressionMatchIterator>
#include <QFile>
#include <QTextStream>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "worker.h"

class WebView : public QWebView
{
    Q_OBJECT
public:
   WebView(QWidget *parent = 0);
   ~WebView();
   void setViewProxy(QNetworkProxy proxy);
   void setParams();
   void openEmailListFile(QFile * emailListFile);
   void readEmailListFile(QFile * readEmailFile) const;
   void view1Page(QWebPage *page);
   void view2Page(QWebPage *page);
   void view3Page(QWebPage *page);
   void view4Page(QWebPage *page);
   void connOpen();
   void connClose();
   void proxyListRotater(QStringList proxyServersList,QWebPage *page);
   void initProxyListSettings();
   void harvestStatus(QString status);
   QString getRelativePath(QString fileName);


protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void newPageLoading();
    void pageLoaded(bool ok);
public slots:
    void insertEmailsJson();
    void view1HttpResponseFinished(QNetworkReply * reply);
    void view2HttpResponseFinished(QNetworkReply * reply);
    void view3HttpResponseFinished(QNetworkReply * reply);
    void view4HttpResponseFinished(QNetworkReply * reply);

private:
    QTime loadingTime;
    bool inLoading;
    QFile * emailListFile;
    bool finishLoading;
    QStringList *emailList;
    QFile * readEmailFile;
    QSqlDatabase mydb;
    QStringList logMessage;
    int *httpStatusPtr;
    int httpStatusNum;
    Worker worker;
    bool viewPage1ProxyRotate;
    bool viewPage2ProxyRotate;
    bool viewPage3ProxyRotate;
    bool viewPage4ProxyRotate;
    int *workerCounterPtr;
    int workerCounterNum;
    int proxyServerCounterNum;
    int *proxyServerCounterPtr;
    QStringList proxyList1;
    QStringList proxyList2;
    QStringList proxyList3;
    QStringList proxyList4;

    QJsonArray proxyServersArray;
    int proxyRotateInterval;
    QString *proxies;
    bool isProxyEmpty;
    bool canProxyCounterIncrement;
    bool isMultiURLSelected;
    QString *replyUrl;

signals:
    void emitSenderTimer();
    void emitWebViewLog(QString logMessage);
};

#endif // WEBVIEW_H

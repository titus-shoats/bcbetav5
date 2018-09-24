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
#include "config.h"
#include <QStandardPaths>
#include <QDir>
#include "curleasy.h"


class WebView : public QWebView
{
    Q_OBJECT
public:
   WebView(QWidget *parent = 0);
   ~WebView();
   void view1Page(QString url);
   void view2Page(QString url);
   void view3Page(QString url);
   void view4Page(QString url);
   void connOpen();
   void connClose();
   void proxyListRotater(QStringList proxyServersList);
   void initSettings();
   void harvestStatus(QString status);
   QString getRelativePath(QString fileName);


protected:
    void paintEvent(QPaintEvent *event);

private:
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
    QString excludeEmailDuplicates;

signals:
    void emitSenderTimer();
    void emitWebViewLog(QString logMessage);
};

#endif // WEBVIEW_H

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

class WebView : public QWebView
{
    Q_OBJECT
public:
   WebView(QWidget *parent = 0);
   ~WebView();
   //void setViewProxy(QString proxyHost,quint16 proxyPort);
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


protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void newPageLoading();
    void pageLoaded(bool ok);

public slots:
    void insertEmailsJson();

private:
    QTime loadingTime;
    bool inLoading;
   // QNetworkRequest *request;
    QFile * emailListFile;
    bool finishLoading;
    QStringList *emailList;
    QFile * readEmailFile;
    QSqlDatabase mydb;



signals:
    void emitSenderTimer();
};

#endif // WEBVIEW_H

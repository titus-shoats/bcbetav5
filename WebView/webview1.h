#ifndef WEBVIEW1_H
#define WEBVIEW1_H

#include <QWebView>
#include <QWebPage>
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

class WebView1 : public QWebView
{
    Q_OBJECT
public:
   WebView1(QWidget *parent = 0);
   ~WebView1();
   //void setViewProxy(QString proxyHost,quint16 proxyPort);
   void setViewProxy(QNetworkProxy proxy);
   void setParams();
   void loadPage(const QNetworkRequest & request);
   void openEmailListFile(QFile * emailListFile);
   void view1Page(QWebPage *page);


protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void newPageLoading();
    void pageLoaded(bool ok);

private:
    QTime loadingTime;
    bool inLoading;
   // QNetworkRequest *request;
    QFile * emailListFile;

signals:
    void emitSenderTimer();
};

#endif // WEBVIEW1_H

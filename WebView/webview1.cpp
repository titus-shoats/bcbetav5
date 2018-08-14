#include "webview1.h"
#include <QPaintEvent>
#include <QWebFrame>

WebView1::WebView1(QWidget *parent)
    : QWebView(parent)
    , inLoading(false),emailListFile(nullptr)
{
    connect(this, SIGNAL(loadStarted()), this, SLOT(newPageLoading()));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(pageLoaded(bool)));
    page()->setPreferredContentsSize(QSize(600, 768));
    emailListFile = new QFile();
    openEmailListFile(emailListFile);

   // request = new QNetworkRequest();

}

WebView1::~WebView1(){
   // delete request;
    emailListFile->close();
    delete emailListFile;
}

void WebView1::openEmailListFile(QFile * emailListFile){

    emailListFile->setFileName("C:/Users/ace/Documents/QT_Projects/WebView/WebView/emailList.txt");
    if(! emailListFile->open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Error opening keyword file in worker";
        return;
    }
}

void WebView1::loadPage(const QNetworkRequest & request)
{

   // load(request);
}
void WebView1::paintEvent(QPaintEvent *event)
{
    if (inLoading && loadingTime.elapsed() < 750) {
        QPainter painter(this);
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        foreach (const QRect &rect, event->region().rects()) {
            painter.drawRect(rect);
        }
    } else {
        QWebView::paintEvent(event);
    }
}

void WebView1::newPageLoading()
{
    inLoading = true;
    loadingTime.start();
}

void WebView1::pageLoaded(bool successLoaded)
{
    inLoading = false;
    update();
    if(successLoaded)
    {
      qDebug() << "Page  loaded";

      QRegularExpression re("[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}");
      QRegularExpressionMatchIterator i = re.globalMatch( page()->mainFrame()->toPlainText());
      QRegularExpressionMatch match;
      QStringList words;
      QString word;
      QList<QString> list;
      QString emailList;
      int num = 0;
      QTextStream outStream(emailListFile);
      while (i.hasNext()) {

          QRegularExpressionMatch match = i.next();
          if (!match.captured(0).isEmpty())
              word = match.captured(0);
          if (!word.isEmpty()) {
              words << word;
              list << words;
              if (!words.value(num).isEmpty())
              {
                  //qDebug() << words.value(num);
                 outStream << words.value(num) << "\n";
              }
              num++;

          } // end words !empty

      } // end while

    }else
    {
      qDebug() << "Page not loaded";
    }
}

//void WebView::setViewProxy(QString proxyHost,quint16 proxyPort)
void WebView1::setViewProxy(QNetworkProxy proxy)
{
    // QNetworkProxy proxy;
     //proxy.setType(QNetworkProxy::Socks5Proxy);
     // proxy.setUser("username");
     // proxy.setPassword("password");
     //proxy.setType(QNetworkProxy::HttpProxy);
    // proxy.setHostName(proxyHost);
     //proxy.setPort(proxyPort);
     //QNetworkProxy::setApplicationProxy(proxy);
     page()->networkAccessManager()->setProxy(proxy);
}

void WebView1::setParams()
{
  // request->setUrl(url);
  // request->setRawHeader("User-Agent",userAgent);
    for(;;){
        QEventLoop loop;
        QTimer::singleShot(5000,&loop,SLOT(quit()));
        loop.exec();
        emit emitSenderTimer();

    }
}

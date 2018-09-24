#-------------------------------------------------
#
# Project created by QtCreator 2018-08-06T20:33:15
#
#-------------------------------------------------

QT       += core gui widgets webkitwidgets webkit sql network concurrent

TARGET = WebView
TEMPLATE = app

INCLUDEPATH += C:\curl-7.34.0-devel-mingw32\include
LIBS += C:\curl-7.34.0-devel-mingw32\lib\libcurldll.a
LIBS += C:\curl-7.34.0-devel-mingw32\lib\libcurl.a


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    webview.cpp \
    worker.cpp \
    curleasy.cpp \
    curlmulti.cpp

HEADERS += \
        mainwindow.h \
    worker.h \
    webview.h \
    config.h \
    curleasy.h \
    curlmulti.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    emailList.txt

TEMPLATE = app
TARGET = ISOWebBrowser
QT += webenginewidgets network widgets printsupport

CONFIG += debug_and_release

HEADERS  += browsermainwindow.h \
            browserapplication.h \
    tabwidget.h \
    webview.h \
    urllineedit.h \
    browsertypes.h

SOURCES += main.cpp\
        browsermainwindow.cpp \
        browserapplication.cpp \
    tabwidget.cpp \
    webview.cpp \
    urllineedit.cpp

FORMS += \
    mainwindow.ui

RESOURCES += \
    data/data.qrc

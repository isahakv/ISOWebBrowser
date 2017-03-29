TEMPLATE = app
TARGET = ISOWebBrowser
QT += webenginewidgets network widgets svg printsupport

CONFIG += debug_and_release

HEADERS  += browsermainwindow.h \
            browserapplication.h \
    tabwidget.h \
    webview.h \
    urllineedit.h \
    browsertypes.h \
    settings.h \
    inspectelement.h \
    webviewwrapper.h \
    browserhelpers.h \
    history.h

SOURCES += main.cpp\
        browsermainwindow.cpp \
        browserapplication.cpp \
    tabwidget.cpp \
    webview.cpp \
    urllineedit.cpp \
    settings.cpp \
    inspectelement.cpp \
    webviewwrapper.cpp \
    browserhelpers.cpp \
    history.cpp

FORMS += \
    mainwindow.ui \
    settings.ui \
    history.ui

RESOURCES += \
    data/data.qrc

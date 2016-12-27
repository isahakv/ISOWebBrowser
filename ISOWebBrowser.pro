TEMPLATE = app
TARGET = ISOWebBrowser
QT += webenginewidgets network widgets printsupport

CONFIG += debug_and_release

HEADERS  += browsermainwindow.h \
            browserapplication.h \
    tabwidget.h \
    webview.h \
    urllineedit.h \
    browsertypes.h \
    settings.h

SOURCES += main.cpp\
        browsermainwindow.cpp \
        browserapplication.cpp \
    tabwidget.cpp \
    webview.cpp \
    urllineedit.cpp \
    settings.cpp

FORMS += \
    mainwindow.ui \
    settings.ui

RESOURCES += \
    data/data.qrc

TEMPLATE = app
TARGET = ISOWebBrowser
QT += webenginewidgets network widgets printsupport

HEADERS  += browsermainwindow.h \
            browserapplication.h \
    tabwidget.h \
    webview.h

SOURCES += main.cpp\
        browsermainwindow.cpp \
        browserapplication.cpp \
    tabwidget.cpp \
    webview.cpp

FORMS += \
    mainwindow.ui

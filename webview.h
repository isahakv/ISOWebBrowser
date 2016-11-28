#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>

class WebPage : public QWebEnginePage
{
	Q_OBJECT

public:
};

class WebView : public QWebEngineView
{
	Q_OBJECT

public:
	WebView();
};

#endif // WEBVIEW_H

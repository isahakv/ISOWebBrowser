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
	WebView(QWidget* parent = 0);

	void LoadUrl(const QUrl& url);
	QUrl GetUrl() const;

private:
	QUrl initialUrl;
};

#endif // WEBVIEW_H

#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEnginePage>
#include <QWebEngineView>

#include <QtWidgets/QMainWindow>

class QWebEngineProfile;

class WebPage : public QWebEnginePage
{
	Q_OBJECT

public:
	WebPage(QWebEngineProfile* profile, QObject* parent = 0);

	void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message,
								  int lineNumber, const QString &sourceID);
};

class InspectElement;
class WebViewWrapper;

class WebView : public QWebEngineView
{
	Q_OBJECT

public:
	WebView(QWidget* parent = 0, WebViewWrapper* _ownerWebViewWrapper = 0);
	~WebView();
	QSize sizeHint() const;

	inline WebViewWrapper* GetOwnerWebViewWrapper() { return ownerWebViewWrapper; }

	void LoadUrl(const QUrl& url);
	QUrl GetUrl() const;
	inline bool IsWebPageLoading() const { return isWebPageLoading; }

protected:
	WebViewWrapper* ownerWebViewWrapper;

private slots:
	void SlotLoadStarted();
	void SlotLoadFinished();

private:
	QUrl initialUrl;
	bool isWebPageLoading;
};

#endif // WEBVIEW_H

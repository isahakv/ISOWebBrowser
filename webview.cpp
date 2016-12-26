#include "webview.h"

#include <QWebEngineProfile>

WebPage::WebPage(QWebEngineProfile* profile, QObject* parent)
	: QWebEnginePage(profile, parent)
{

}

void WebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message,
							  int lineNumber, const QString &sourceID)
{
	//if (level == JavaScriptConsoleMessageLevel::ErrorMessageLevel)
		qWarning((QString("Js Error: \"") + message + QString("\" ; lineNumber: ") + lineNumber).toStdString().c_str());
}

WebView::WebView(QWidget* parent)
	: QWebEngineView(parent)
{
	isWebPageLoading = false;

	connect(this, SIGNAL(loadStarted()), this, SLOT(SlotLoadStarted()));
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(SlotLoadFinished()));
}

void WebView::LoadUrl(const QUrl& url)
{
	initialUrl = url;
	load(url);
}

QUrl WebView::GetUrl() const
{
	QUrl url = QWebEngineView::url();
	if (!url.isEmpty())
		return url;

	return initialUrl;
}

void WebView::SlotLoadStarted()
{
	isWebPageLoading = true;
}

void WebView::SlotLoadFinished()
{
	isWebPageLoading = false;
}

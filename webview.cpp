#include "webview.h"

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

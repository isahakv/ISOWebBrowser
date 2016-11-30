#include "webview.h"

WebView::WebView(QWidget* parent)
	: QWebEngineView(parent)
{

}

void WebView::LoadUrl(const QUrl& url)
{
	initialUrl = url;
	load(url);
}

QUrl WebView::GetUrl() const
{
	QUrl url = QWebEngineView::url();
	if (url.isEmpty())
		return url;

	return initialUrl;
}

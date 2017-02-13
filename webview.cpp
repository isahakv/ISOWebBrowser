#include "webview.h"

#include <QWebEngineProfile>

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "webviewwrapper.h"
#include "inspectelement.h"

WebPage::WebPage(QWebEngineProfile* profile, QObject* parent)
	: QWebEnginePage(profile, parent)
{
}

void WebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message,
							  int lineNumber, const QString &sourceID)
{
	WebView* webView = qobject_cast<WebView*>(view());
	if (!webView)
		return;

	webView->GetOwnerWebViewWrapper()->GetInspectElement()->AddJavaScriptConsoleMessage(level, message,
																						lineNumber, sourceID);
}

WebView::WebView(QWidget* parent, WebViewWrapper* _ownerWebViewWrapper)
	: QWebEngineView(parent)
	, ownerWebViewWrapper(_ownerWebViewWrapper)
	, isWebPageLoading(false)
{
	QSizePolicy sp = sizePolicy();
	sp.setHorizontalPolicy(QSizePolicy::Preferred);
	//sp.setVerticalStretch(2);
	setSizePolicy(sp);

	connect(this, SIGNAL(loadStarted()), this, SLOT(SlotLoadStarted()));
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(SlotLoadFinished()));
}

WebView::~WebView()
{
}

WebPage* WebView::GetWebPage() const
{
	return qobject_cast<WebPage*>(page());
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

InspectElement* WebView::GetInspectElement() const
{
	return ownerWebViewWrapper->GetInspectElement();
}

void WebView::SlotLoadStarted()
{
	isWebPageLoading = true;
}

void WebView::SlotLoadFinished()
{
	isWebPageLoading = false;
}

#ifndef WEBVIEWWRAPPER_H
#define WEBVIEWWRAPPER_H

#include <QWidget>

class WebView;
class InspectElement;

class WebViewWrapper : public QWidget
{
	Q_OBJECT
public:
	explicit WebViewWrapper(QWidget *parent = 0);

	inline WebView* GetWebView() const { return webView; }
	inline InspectElement* GetInspectElement() const { return inspectElement; }

protected:
	WebView* webView;
	InspectElement* inspectElement;
};

#endif // WEBVIEWWRAPPER_H

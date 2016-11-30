#ifndef URLLINEEDIT_H
#define URLLINEEDIT_H

#include <QtCore/QUrl>
#include <QtWidgets/QWidget>

class QLineEdit;
class WebView;

class UrlLineEdit : public QWidget
{
	Q_OBJECT
public:
	UrlLineEdit(QWidget *parent = 0);
	void SetWebView(WebView* _webView);

	inline QLineEdit* GetLineEdit() const { return lineEdit; }

	QSize SizeHint() const;

protected:
	QLineEdit* lineEdit;

private slots:
	void WebViewUrlChanged(const QUrl& url);
	void WebViewIconChanged(const QIcon& icon);

private:
	WebView* webView;
};

#endif // URLLINEEDIT_H

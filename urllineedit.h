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
	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);
	void keyPressEvent(QKeyEvent* event);

protected:
	QLineEdit* lineEdit;

private slots:
	void WebViewUrlChanged(const QUrl& url);

private:
	WebView* webView;
};

#endif // URLLINEEDIT_H

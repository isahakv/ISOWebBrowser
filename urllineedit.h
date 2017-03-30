#ifndef URLLINEEDIT_H
#define URLLINEEDIT_H

#include <QtCore/QUrl>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QPushButton>

class QLineEdit;
class WebView;
class BrowserMainWindow;

/*
 * Search icon on the left hand side of the search widget
 * When a menu is set a down arrow appears
*/
class SearchButton : public QAbstractButton
{
	Q_OBJECT

public:
	SearchButton(QWidget* parent = 0, int _size = 16);
	void paintEvent(QPaintEvent* event);
	QSize sizeHint() const;

	QMenu* menu;

protected:
	void mousePressEvent(QMouseEvent* event);

	int size;
};

/*
 * Clear Buuton on the right hand side of the BrowserLineEdit.
 * Hidden by Default
 * "A circle with an X in it"
*/
class ClearButton : public QAbstractButton
{
	Q_OBJECT

public:
	ClearButton(QWidget* parent = 0, int _size = 16);
	void paintEvent(QPaintEvent* event);
	QSize sizeHint() const;

public slots:
	void TextChanged(const QString& text);

protected:
	int size;
};

class BrowserLineEdit : public QWidget
{
	Q_OBJECT

signals:
	void textChanged(const QString&);

public:
	BrowserLineEdit(QWidget* parent = 0);
	inline void SetOwnerBrowserMainWindow(BrowserMainWindow* ownerMainWindow) { ownerBrowserMainWindow = ownerMainWindow; }

	inline QLineEdit* GetLineEdit() const { return lineEdit; }

	inline QWidget* GetLeftWidget() const { return leftWidget; }
	inline void SetLeftWidget(QWidget* widget) { leftWidget = widget; }

	QSize sizeHint() const;

	QVariant inputMethodQuery(Qt::InputMethodQuery property) const;

	inline BrowserMainWindow* GetOwnerBrowserMainWindow() const { return ownerBrowserMainWindow; }

	int heightOfLineEdit;

protected:
	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void resizeEvent(QResizeEvent* event);
	void inputMethodEvent(QInputMethodEvent* event);

	QWidget* leftWidget;
	QLineEdit* lineEdit;
	ClearButton* clearButton;

	BrowserMainWindow* ownerBrowserMainWindow;
};

class UrlLineEdit : public BrowserLineEdit
{
	Q_OBJECT
public:
	UrlLineEdit(QWidget *parent);
	void SetWebView(WebView* _webView);

protected:
	void focusOutEvent(QFocusEvent* event);

private slots:
	void WebViewUrlChanged(const QUrl& url);

private:
	WebView* webView;
};

#endif // URLLINEEDIT_H

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
	SearchButton(QWidget* parent = 0);
	void paintEvent(QPaintEvent* event);

	QMenu* menu;

protected:
	void mousePressEvent(QMouseEvent* event);
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
	ClearButton(QWidget* parent = 0);
	void paintEvent(QPaintEvent* event);

public slots:
	void TextChanged(const QString& text);
};

class BrowserLineEdit : public QWidget
{
	Q_OBJECT

public:
	BrowserLineEdit(QWidget* parent = 0, BrowserMainWindow* ownerMainWindow = 0);

	inline QLineEdit* GetLineEdit() const { return lineEdit; }

	inline QWidget* GetLeftWidget() const { return leftWidget; }
	inline void SetLeftWidget(QWidget* widget) { leftWidget = widget; }

	QSize SizeHint() const;

	QVariant inputMethodQuery(Qt::InputMethodQuery property) const;

	inline BrowserMainWindow* GetOwnerBrowserMainWindow() const { return ownerBrowserMainWindow; }

protected:
	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void resizeEvent(QResizeEvent* event);
	void inputMethodEvent(QInputMethodEvent* event);

	virtual void UpdateGeometries();

	QWidget* leftWidget;
	QLineEdit* lineEdit;
	ClearButton* clearButton;

	BrowserMainWindow* ownerBrowserMainWindow;
};

class UrlLineEdit : public BrowserLineEdit
{
	Q_OBJECT
public:
	UrlLineEdit(QWidget *parent, BrowserMainWindow* ownerMainWindow);
	void SetWebView(WebView* _webView);

protected:
	void focusOutEvent(QFocusEvent* event);

private slots:
	void WebViewUrlChanged(const QUrl& url);

private:
	WebView* webView;
};

class QStringListModel;

class SearchLineEdit : public BrowserLineEdit
{
	Q_OBJECT
public:
	SearchLineEdit(QWidget* parent, BrowserMainWindow* ownerMainWindow);

	inline QString GetInactiveText() const { return inactiveText; }
	inline void setInactiveText(QString text) { inactiveText = text; }

	QMenu* GetMenu() const;

signals:
	void Search(const QUrl& url);

protected:
	void paintEvent(QPaintEvent* event);

	virtual void UpdateGeometries() override;

public slots:
	void ClearRecentSearches();
	void SlotSearch();

private slots:
	void AboutToShowMenu();
	void TriggeredMenuAction(QAction* action);
	void SaveSearchHistory();

private:
	void LoadSearchHistory();

	SearchButton* searchButton;
	QString inactiveText;
	int maxSavedSearches;
	QStringListModel* stringListModel;
};

#endif // URLLINEEDIT_H

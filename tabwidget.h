#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QtWidgets/QTabBar>

/*
	Tab bar with a few more features such as a context menu and shortcuts
 */
class TabBar : public QTabBar
{
	Q_OBJECT

public:
	TabBar(QWidget* parent = 0);

signals:
	void NewTab();
	void CloseTab(int index);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private slots:
	void CloseTab();
	void ContextMenuRequested(const QPoint& position);

private:
	friend class TabWidget;

	QPoint dragStartPos;
};

#include <QWebEnginePage>
/*!
	A proxy object that connects a single browser action
	to one child webpage action at a time.

	Example usage: used to keep the main window stop action in sync with
	the current tabs webview's stop action.
 */
class WebActionMapper : public QObject
{
	Q_OBJECT

public:

};

#include <QtWidgets/QTabWidget>

class WebView;
class QLineEdit;
class QStackedWidget;

/*!
	TabWidget that contains WebViews and a stack widget of associated line edits.

	Connects up the current tab's signals to this class's signal and uses WebActionMapper
	to proxy the actions.
 */
class TabWidget : public QTabWidget
{
	Q_OBJECT

public:
	TabWidget(QWidget* parent = 0);
	~TabWidget();

	QWidget* GetLineEditStack() const;
	QLineEdit* GetCurrentLineEdit() const;
	QLineEdit* GetLineEdit(int index) const;
	WebView* GetCurrentWebView() const;
	WebView* GetWebView(int index) const;
	int GetWebViewIndex(WebView* webView) const;

signals:
	// tab widget signals
	void LoadPage(const QString& url);

	// current tab signals
	void geometryChangeRequested(const QRect& geometry);

public slots:
	void LoadUrlInCurrentTab(const QUrl& url);
	WebView* NewTab(bool makeCurrent = true);
	void RequestCloseTab(int index = -1);
	void CloseTab(int index);

protected:
	void mouseDoubleClickEvent(QMouseEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private slots:
	void CurrentTabChanged(int index);
	void LineEditReturnPressed();
	void WindowCloseRequested();

private:
	void SetupPage(QWebEnginePage* page);

	QStackedWidget* lineEdits;
	TabBar* tabBar;
};

#endif // TABWIDGET_H

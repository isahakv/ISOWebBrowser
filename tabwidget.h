#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QtWidgets/QTabBar>

class QLabel;

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
	QList<QLabel*> tabLabels;
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
	WebActionMapper(QAction* root, QWebEnginePage::WebAction webAction, QObject* parent);
	void UpdateCurrentPage(QWebEnginePage* newCurrentPage);

private slots:
	void RootTriggered();
	void RootDistroyed();
	void CurrentPageActionChanged();
	void CurrentPageDestroyed();

private:
	QWebEnginePage* currentPage;
	QAction* rootAction;
	QWebEnginePage::WebAction webAction;
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

	void AddWebAction(QAction* action, QWebEnginePage::WebAction webAction);

	QWidget* GetLineEditStack() const;
	QLineEdit* GetCurrentLineEdit() const;
	QLineEdit* GetLineEdit(int index) const;
	WebView* GetCurrentWebView() const;
	WebView* GetWebView(int index) const;
	int GetWebViewIndex(WebView* webView) const;

signals:
	// tab widget signals
	void LoadPage(const QString& url);
	void LastTabClosed();

	// current tab signals
	void ShowStatusBarMessage(const QString& message);
	void WebViewLoadStarted(WebView* webView);
	void WebViewLoadFinished(WebView* webView);
	void WebPageLinkHovered(const QString& link);
	void CurrentTabChanged(int index);
	void geometryChangeRequested(const QRect& geometry);

public slots:
	void LoadHomePage(WebView* tab);
	void LoadNewTabPage(WebView* tab);
	void LoadUrlInCurrentTab(const QUrl& url);
	WebView* NewTab(bool makeCurrent = true, bool loadHomePage = false);
	void RequestCloseTab(int index = -1);
	void CloseTab(int index);

protected:
	void mouseDoubleClickEvent(QMouseEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private slots:
	void SlotCurrentTabChanged(int index);
	void SlotWebViewLoadStarted();
	void SlotWebViewLoadFinished(bool b);
	void WebViewIconChanged(const QIcon& icon);
	void WebViewTitleChanged(const QString& title);
	void WebViewUrlChanged(const QUrl& url);
	void WebPageMutedOrAudibleChanged();
	void LineEditReturnPressed();
	void WindowCloseRequested();

private:
	void SetupPage(QWebEnginePage* page);
	void SetTabIconToImage(int tabIndex, const QIcon& icon);
	void SetTabIconToGif(int tabIndex, const QString& path);

	QList<WebActionMapper*> webActionMappers;

	QStackedWidget* lineEdits;
	TabBar* tabBar;
};

#endif // TABWIDGET_H

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QtWidgets/QTabBar>

#include "browsertypes.h"

class QLabel;
class QWebEngineProfile;
class BrowserMainWindow;

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
	void CloneTab(int index);
	void CloseTab(int index);
	void CloseOtherTabs(int index);
	void ReloadTab(int index);
	void MuteTab(int index, bool mute);
	void ReloadAllTabs();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private slots:
	void CloneTab();
	void CloseTab();
	void CloseOtherTabs();
	void ReloadTab();
	void MuteTab();
	void UnmuteTab();
	void ContextMenuRequested(const QPoint& position);

private:
	QList<QLabel*> tabLabels;
	friend class TabWidget;

	QPoint dragStartPos;
};

#include "webview.h"
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
	QWebEnginePage* currentPage; // Note: make this property static
	QAction* rootAction;
	QWebEnginePage::WebAction webAction;
};



#include <QtWidgets/QTabWidget>

class WebView;
class WebViewWrapper;
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
	TabWidget(QWidget* parent = 0, BrowserMainWindow* ownerMainWindow = 0);
	~TabWidget();

	void AddWebAction(QAction* action, QWebEnginePage::WebAction webAction);

	QWidget* GetLineEditStack() const;
	QLineEdit* GetCurrentLineEdit() const;
	QLineEdit* GetLineEdit(int index) const;
	WebView* GetCurrentWebView() const;
	WebView* GetWebView(int index) const;
	int GetWebViewIndex(WebView* webView) const;
	inline BrowserMainWindow* GetOwnerBrowserMainWindow() const { return ownerBrowserMainWindow; }

	static int MaxSymbolsInTabTitle;

	inline QWebEngineProfile* GetProfile() const { return profile; }
	void SetProfile(QWebEngineProfile* newProfile);

signals:
	// tab widget signals
	void LoadPage(WebView* tab, const QString& url);
	void LastTabClosed();

	// current tab signals
	void ShowStatusBarMessage(const QString& message);
	void WebViewLoadStarted(WebView* webView);
	void WebViewLoadFinished(WebView* webView);
	void WebPageLinkHovered(const QString& link);
	void CurrentTabChanged(int index);
	void geometryChangeRequested(const QRect& geometry);

public slots:
	void LoadHomePage(WebView* tab = 0, HomePageType homePageType = HomePageType::None);
	void LoadNewTabPage(WebView* tab);
	void LoadUrl(WebView* tab, const QUrl& url);
	WebViewWrapper* NewTab(bool makeCurrent = true, HomePageType homePageType = HomePageType::None);
	void CloneTab(int index = -1);
	void RequestCloseTab(int index = -1);
	void CloseTab(int index);
	void CloseOtherTabs(int index);
	void ReloadTab(int index);
	void ReloadAllTabs();
	void SetAudioMutedForTab(int index, bool mute);

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
	void MoveTab(int fromIndex, int toIndex);

private:
	void SetupPage(QWebEnginePage* page);
	void SetTabIconToImage(int tabIndex, const QIcon& icon);
	void SetTabIconToGif(int tabIndex, const QString& path);
	QString NormalizeTabTitle(const QString& title);

	QList<WebActionMapper*> webActionMappers;

	QStackedWidget* lineEdits;
	TabBar* tabBar;
	QWebEngineProfile* profile;

	BrowserMainWindow* ownerBrowserMainWindow;
};

#endif // TABWIDGET_H

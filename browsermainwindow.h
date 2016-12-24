#ifndef BROWSERMAINWINDOW_H
#define BROWSERMAINWINDOW_H

#include <QtWidgets/QMainWindow>

class TabWidget;
class WebView;
class QWebEngineProfile;
class SearchLineEdit;

/*!
	The MainWindow of the Browser Application.

	Handles the tab widget and all the actions
 */
class BrowserMainWindow : public QMainWindow
{
    Q_OBJECT

public:
	BrowserMainWindow(QWidget *parent = 0, bool isPrivateWindow = false);
    ~BrowserMainWindow();

	static const QString defaultHomePage;
	static const QString defaultSearchEngine;

	inline TabWidget* GetTabWidget() const { return tabWidget; }
	WebView* GetCurrentTab() const;

	inline bool IsPrivateBrowsing() const { return isPrivateBrowsing; }

public slots:
	void LoadPage(const QString& url);
	void SetPrivateBrowsing(bool);

private slots:
	void SlotWebPageLoadStarted(WebView* webView);
	void SlotWebPageLoadFinished(WebView* webView);
	void SlotCurrentWebPageChanged();
	void SlotUpadateStatusBarText(const QString& text);

	void LoadUrl(const QUrl& url);

	void SlotFileNew();
	void SlotFileOpen();
	void SlotFileSaveAs();
	void SlotNewPrivateWindow();
	void SlotEditFind();
	void SlotEditFindNext();
	void SlotEditFindPrevious();
	void SlotPreferences();
	void SlotViewZoomIn();
	void SlotViewZoomOut();
	void SlotViewResetZoom();
	void SlotViewPageSource();

	void SlotAboutApplication();

	void SlotAboutToShowBackMenu();
	void SlotAboutToShowForwardMenu();
	void SlotOpenActionUrl(QAction* action);

private:
	void SetupMenu();
	void SetupToolBar();
	void HandleFindTextResult(bool isFound);

	void ActivatePageStopAction();
	void ActivatePageReloadAction();

private:
	QToolBar* navigationBar;
	SearchLineEdit* toolbarSearch;
	TabWidget* tabWidget;

	QAction* historyBack;
	QMenu* historyBackMenu;
	QAction* historyForward;
	QMenu* historyForwardMenu;

	QAction* reload;
	QAction* stop;
	QAction* stopReload;

	QIcon reloadIcon;
	QIcon stopIcon;

	QString lastSearch;

	QWebEngineProfile* privateProfile;
	bool isPrivateBrowsing;
};

#endif // BROWSERMAINWINDOW_H

#ifndef BROWSERMAINWINDOW_H
#define BROWSERMAINWINDOW_H

#include <QtWidgets/QMainWindow>

class TabWidget;
class WebView;

/*!
	The MainWindow of the Browser Application.

	Handles the tab widget and all the actions
 */
class BrowserMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    BrowserMainWindow(QWidget *parent = 0);
    ~BrowserMainWindow();

	static const char* defaultHomePage;

	inline TabWidget* GetTabWidget() const { return tabWidget; }
	WebView* GetCurrentTab() const;

public slots:
	void LoadPage(const QString& url);

private slots:
	void SlotWebPageLoadStarted();
	void SlotWebPageLoadFinished(bool b);

	void LoadUrl(const QUrl& url);

	void SlotFileNew();

	void SlotAboutToShowBackMenu();
	void SlotAboutToShowForwardMenu();
	void SlotOpenActionUrl(QAction* action);

private:
	void SetupMenu();
	void SetupToolBar();

private:
	QToolBar* navigationBar;
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

	//HomePageType homePageType;
};

#endif // BROWSERMAINWINDOW_H

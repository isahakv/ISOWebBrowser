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

	inline TabWidget* GetTabWidget() const { return tabWidget; }
	WebView* GetCurrentTab() const;

public slots:
	void LoadPage(const QString& url);

private slots:
	void LoadUrl(const QUrl& url);

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
};

#endif // BROWSERMAINWINDOW_H

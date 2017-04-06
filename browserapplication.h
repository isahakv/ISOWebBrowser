#ifndef BROWSERAPPLICATION_H
#define BROWSERAPPLICATION_H

#include <QtWidgets/QApplication>

#include <QtCore/QPointer>

class BrowserMainWindow;
class HistoryManager;
class DownloadManager;

class BrowserApplication : public QApplication
{
	Q_OBJECT

public:
	BrowserApplication(int &argc, char **argv);
	~BrowserApplication();
	static BrowserApplication* GetInstance();

	bool IsTheOnlyBrowser() const;
	BrowserMainWindow* GetCurrentMainWindow();
	QList<BrowserMainWindow*> GetMainWindows();

	static HistoryManager* GetHistoryManager();
	static DownloadManager* GetDownloadManager();

public slots:
	BrowserMainWindow* newMainWindow(bool isPrivateWindow = false);

signals:

private slots:

private:
	void CleanInvalidMainWindows();
	QString GetCommandLineUrlArgument() const;

	QList<QPointer<BrowserMainWindow> > mainWindows;

	static HistoryManager* historyManager;
	static DownloadManager* downloadManager;
};

#endif // BROWSERAPPLICATION_H

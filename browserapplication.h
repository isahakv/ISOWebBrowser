#ifndef BROWSERAPPLICATION_H
#define BROWSERAPPLICATION_H

#include <QtWidgets/QApplication>

#include <QtCore/QPointer>

class BrowserMainWindow;
class HistoryManager;

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
	QIcon GetIconByUrl(const QUrl& url);

	static HistoryManager* GetHistoryManager();

public slots:
	BrowserMainWindow* newMainWindow(bool isPrivateWindow = false);

signals:

private slots:

private:
	void CleanInvalidMainWindows();
	QString GetCommandLineUrlArgument() const;

	QList<QPointer<BrowserMainWindow> > mainWindows;

	static HistoryManager* historyManager;
};

#endif // BROWSERAPPLICATION_H

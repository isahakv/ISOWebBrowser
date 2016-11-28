#ifndef BROWSERAPPLICATION_H
#define BROWSERAPPLICATION_H

#include <QtWidgets/QApplication>

#include <QtCore/QPointer>

class BrowserMainWindow;

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

public slots:
	BrowserMainWindow* newMainWindow();

signals:

private slots:

private:
	void CleanInvalidMainWindows();

	QList<QPointer<BrowserMainWindow> > mainWindows;
	bool isPrivateBrowsing;
};

#endif // BROWSERAPPLICATION_H

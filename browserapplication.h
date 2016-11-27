#ifndef BROWSERAPPLICATION_H
#define BROWSERAPPLICATION_H

#include <QtWidgets/QApplication>

class BrowserMainWindow;

class BrowserApplication : public QApplication
{
	Q_OBJECT

public:
	BrowserApplication(int &argc, char **argv);
	~BrowserApplication();

	bool isTheOnlyBrowser() const;

public slots:
	BrowserMainWindow* newMainWindow();
};

#endif // BROWSERAPPLICATION_H

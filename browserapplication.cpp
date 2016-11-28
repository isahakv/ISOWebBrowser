#include "browserapplication.h"

#include "browsermainwindow.h"

BrowserApplication::BrowserApplication(int &argc, char **argv)
	: QApplication(argc, argv),
	  isPrivateBrowsing(false)
{

}

BrowserApplication::~BrowserApplication()
{
	for (int i = 0; i < mainWindows.size(); i++)
	{
		BrowserMainWindow* window = mainWindows.at(i);
		delete window;
	}
}

bool BrowserApplication::IsTheOnlyBrowser() const
{
	return true;
}

BrowserMainWindow* BrowserApplication::GetCurrentMainWindow()
{
	// cleanup any deleted main windows first
	CleanInvalidMainWindows();
	if (mainWindows.isEmpty())
		newMainWindow();

	return mainWindows[0];
}

QList<BrowserMainWindow*> BrowserApplication::GetMainWindows()
{
	// cleanup any deleted main windows first
	CleanInvalidMainWindows();
	QList<BrowserMainWindow*> list;
	for (int i = 0; i < mainWindows.count(); i++)
		list.append(mainWindows.at(i));
	return list;
}

BrowserMainWindow* BrowserApplication::newMainWindow()
{
	BrowserMainWindow* browser = new BrowserMainWindow();
	mainWindows.prepend(browser);
	browser->show();
	return browser;
}

void BrowserApplication::CleanInvalidMainWindows()
{
	for (int i = mainWindows.count() - 1; i >= 0; i--)
		if (mainWindows.at(i).isNull())
			mainWindows.removeAt(i);
}

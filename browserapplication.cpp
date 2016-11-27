#include "browserapplication.h"

#include "browsermainwindow.h"

BrowserApplication::BrowserApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{

}

BrowserApplication::~BrowserApplication()
{

}

bool BrowserApplication::isTheOnlyBrowser() const
{
	return true;
}

BrowserMainWindow* BrowserApplication::newMainWindow()
{
	return NULL;
}

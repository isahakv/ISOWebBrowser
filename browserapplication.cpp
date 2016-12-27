#include "browserapplication.h"

#include "browsermainwindow.h"

#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

#include <QUrl>

BrowserApplication::BrowserApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{
	QApplication::setQuitOnLastWindowClosed(true);

	// PostLaunch
	setWindowIcon(QIcon(QLatin1String(":Images/64x64/browser_icon.png")));

	if (mainWindows.count() > 0)
	{
		const QString url = GetCommandLineUrlArgument();
		if (!url.isEmpty())
			GetCurrentMainWindow()->LoadPage(NULL, url);
		//else
		//	GetCurrentMainWindow()->SlotLoadHomePage();
	}
}

BrowserApplication::~BrowserApplication()
{
	CleanInvalidMainWindows();
	for (int i = 0; i < mainWindows.size(); i++)
	{
		BrowserMainWindow* window = mainWindows.at(i);
		delete window;
	}
}

BrowserApplication* BrowserApplication::GetInstance()
{
	return static_cast<BrowserApplication *>(QCoreApplication::instance());
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

QIcon BrowserApplication::GetIconByUrl(const QUrl& url)
{
	// QIcon icon = QWebEngineSettings::
	return QIcon(":Images/16x16/defaulticon.png");
}

BrowserMainWindow* BrowserApplication::newMainWindow(bool isPrivateWindow)
{
	BrowserMainWindow* browser = new BrowserMainWindow(0, isPrivateWindow);
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

QString BrowserApplication::GetCommandLineUrlArgument() const
{
	const QStringList args = QCoreApplication::arguments();
	if (args.count() > 1)
	{
		const QString lastArg = args.last();
		const bool isValidUrl = QUrl::fromUserInput(lastArg).isValid();
		if (isValidUrl)
			return lastArg;
	}
	return QString();
}

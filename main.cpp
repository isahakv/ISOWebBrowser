#include "browserapplication.h"

#include <qtwebenginewidgetsglobal.h>

int main(int argc, char *argv[])
{
	BrowserApplication application(argc, argv);
	if (!application.IsTheOnlyBrowser())
		return 0;

	application.setApplicationVersion(QString("V0.0.11_PreAlpha"));
	application.setApplicationName(QLatin1String("ISOBrowser"));
	application.setApplicationDisplayName(QLatin1String("ISOBrowser"));

	application.newMainWindow();
	return application.exec();
}

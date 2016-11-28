#include "browserapplication.h"

#include <qtwebenginewidgetsglobal.h>

int main(int argc, char *argv[])
{
	BrowserApplication application(argc, argv);
	if (!application.IsTheOnlyBrowser())
		return 0;

	application.newMainWindow();
	return application.exec();
}

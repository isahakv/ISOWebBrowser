#ifndef DEFINES_H
#define DEFINES_H

#include <QString>

/** Defines for storing default values */
const QString defaultHomePage = "http://google.com/";
const QString defaultSearchEngine = "http://google.com/search";

/** Application Defines */
#define APP_NAME QLatin1String("ISOBrowser")
#define APP_VERSION QLatin1String("V0.0.12")

/** Registry Key Names */
#define BROWSER_GENERAL_REGISTRY_GROUP_KEY QLatin1String("BrowserMainWindow")
#define BROWSER_MAIN_WINDOW_REGISTRY_GROUP_KEY QLatin1String("General")
#define BROWSER_TOOLBAR_SEARCH_GROUP_KEY QLatin1String("ToolbarSearch")

#define BROWSER_HOME_PAGE_REGISTRY_KEY QLatin1String("HomePage")
#define BROWSER_SEARCH_ENGINE_REGISTRY_KEY QLatin1String("SearchEngine")
#define BROWSER_DEFAULT_STATE_REGISTRY_KEY QLatin1String("DefaultState")
#define BROWSER_RECENT_SEARCHES_KEY QLatin1String("RecentSearches")
#define BROWSER_MAX_SAVED_SEARCHES_KEY QLatin1String("MaxSavedSearches")

#endif // DEFINES_H

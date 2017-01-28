#ifndef BROWSERTYPES_H
#define BROWSERTYPES_H

enum class HomePageType : short
{
	None = -1,
	NewTabPage,
	SpecificPage
};

Q_DECLARE_METATYPE(HomePageType)

#endif // BROWSERTYPES_H

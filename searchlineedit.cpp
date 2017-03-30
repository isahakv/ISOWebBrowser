#include "searchlineedit.h"

#include "browsermainwindow.h"

#include <QStringListModel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMenu>
#include <QSettings>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

SearchLineEdit::SearchLineEdit(QWidget* parent)
	: BrowserLineEdit(parent)
	, searchButton(new SearchButton(this))
	, maxSavedSearches(10)
	, stringListModel(new QStringListModel(this))
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(5);

	layout->addWidget(searchButton);
	layout->addWidget(lineEdit);
	layout->addWidget(clearButton);
	setLayout(layout);

	lineEdit->setPlaceholderText(QString("Search"));

	QMenu* m = GetMenu();
	connect(m, SIGNAL(aboutToShow()), this, SLOT(AboutToShowMenu()));
	connect(m, SIGNAL(triggered(QAction*)), this, SLOT(TriggeredMenuAction(QAction*)));

	connect(lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(SlotSearch()));
	LoadSearchHistory();
}

QMenu* SearchLineEdit::GetMenu() const
{
	if (!(searchButton->menu))
		searchButton->menu = new QMenu(searchButton);

	return searchButton->menu;
}

void SearchLineEdit::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)
}

void SearchLineEdit::ClearRecentSearches()
{
	stringListModel->setStringList(QStringList());
	SaveSearchHistory();
}

void SearchLineEdit::SlotSearch()
{
	QString searchText = lineEdit->text();
	QStringList newList = stringListModel->stringList();
	if (newList.contains(searchText))
		newList.removeAt(newList.indexOf(searchText));
	newList.prepend(searchText);

	if (newList.count() >= maxSavedSearches)
		newList.removeLast();

	if (ownerBrowserMainWindow && !(ownerBrowserMainWindow->IsPrivateBrowsing()))
	{
		stringListModel->setStringList(newList);
		SaveSearchHistory();
	}

	QSettings settings(QString("ISOBrowser"));
	settings.beginGroup(QString("General"));
	QUrl url(settings.value(QString("DefaultSearchEngine"),
							BrowserMainWindow::GetDefaultSearchEngine()).toString());
	settings.endGroup();

	QUrlQuery urlQuery;
	urlQuery.addQueryItem(QLatin1String("q"), searchText);
	url.setQuery(urlQuery);
	emit Search(url);
}

void SearchLineEdit::AboutToShowMenu()
{
	QMenu* m = GetMenu();
	m->clear();
	QStringList& list = stringListModel->stringList();
	if (list.isEmpty())
	{
		m->addAction(tr("No Recent Searches"));
		return;
	}

	QAction* recent = m->addAction(tr("Recent Searches"));
	recent->setEnabled(false);
	for (int i = 0; i < list.count(); i++)
	{
		QString text = list.at(i);
		m->addAction(text)->setData(text);
	}
	m->addSeparator();
	m->addAction(tr("Clear Recent Searches"), this, SLOT(ClearRecentSearches()));
}

void SearchLineEdit::TriggeredMenuAction(QAction* action)
{
	QVariant v = action->data();
	if (v.canConvert<QString>())
	{
		QString text = v.toString();
		lineEdit->setText(text);
		SlotSearch();
	}
}

void SearchLineEdit::SaveSearchHistory()
{
	QSettings settings(QLatin1String("ISOBrowser"));
	settings.beginGroup(QLatin1String("ToolbarSearch"));
	settings.setValue(QLatin1String("RecentSearches"), stringListModel->stringList());
	settings.setValue(QLatin1String("MaxSavedSearches"), maxSavedSearches);
	settings.endGroup();

	settings.sync();
}

void SearchLineEdit::LoadSearchHistory()
{
	QSettings settings(QLatin1String("ISOBrowser"));
	settings.beginGroup(QLatin1String("ToolbarSearch"));
	QStringList list = settings.value(QLatin1String("RecentSearches")).toStringList();
	maxSavedSearches = settings.value(QLatin1String("MaxSavedSearches"), maxSavedSearches).toInt();
	settings.endGroup();
	stringListModel->setStringList(list);
}

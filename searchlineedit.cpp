#include "searchlineedit.h"

#include "defines.h"
#include "browsermainwindow.h"

#include <QStringListModel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMenu>
#include <QSettings>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

SearchLineEdit::SearchLineEdit(QWidget* parent, bool _haveHistory)
	: BrowserLineEdit(parent)
	, searchButton(new SearchButton(this))
	, haveHistory(_haveHistory)
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

	connect(lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(SlotSearch()));

	if (haveHistory)
	{
		QMenu* m = GetMenu();
		connect(m, SIGNAL(aboutToShow()), this, SLOT(AboutToShowMenu()));
		connect(m, SIGNAL(triggered(QAction*)), this, SLOT(TriggeredMenuAction(QAction*)));

		LoadSearchHistory();
	}
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

	if (haveHistory)
	{
		QStringList newList = stringListModel->stringList();
		if (newList.contains(searchText))
			newList.removeAt(newList.indexOf(searchText));
		newList.prepend(searchText);

		if (newList.count() > maxSavedSearches)
			newList.removeLast();

		if (ownerBrowserMainWindow && !(ownerBrowserMainWindow->IsPrivateBrowsing()))
		{
			stringListModel->setStringList(newList);
			SaveSearchHistory();
		}
	}

	QUrl url(BrowserMainWindow::GetDefaultSearchEngine());
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
	QVariant data = action->data();
	if (data.canConvert<QString>())
	{
		QString text = data.toString();
		lineEdit->setText(text);
		SlotSearch();
	}
}

void SearchLineEdit::SaveSearchHistory()
{
	QSettings settings(APP_NAME);
	settings.beginGroup(BROWSER_TOOLBAR_SEARCH_GROUP_KEY);
	settings.setValue(BROWSER_RECENT_SEARCHES_KEY, stringListModel->stringList());
	settings.setValue(BROWSER_MAX_SAVED_SEARCHES_KEY, maxSavedSearches);
	settings.endGroup();

	settings.sync();
}

void SearchLineEdit::LoadSearchHistory()
{
	QSettings settings(APP_NAME);
	settings.beginGroup(BROWSER_TOOLBAR_SEARCH_GROUP_KEY);
	QStringList list = settings.value(BROWSER_RECENT_SEARCHES_KEY).toStringList();
	maxSavedSearches = settings.value(BROWSER_MAX_SAVED_SEARCHES_KEY, maxSavedSearches).toInt();
	settings.endGroup();
	stringListModel->setStringList(list);
}

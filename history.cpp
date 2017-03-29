#include "history.h"

#include <QCloseEvent>
#include <QFileInfo>

#include "browserapplication.h"
#include "browsermainwindow.h"

HistoryManager::HistoryManager(QObject* parent)
	: QObject(parent)
	, historyModel(0)
	, historyProxyModel(0)
{
	historyModel = new HistoryModel(this, this);
	historyProxyModel = new HistoryProxyModel(this);
	historyProxyModel->setSourceModel(historyModel);
}

void HistoryManager::AddHistoryEntry(const QString& url, const QString& title, const QIcon& icon)
{
	if (BrowserApplication::GetInstance()->GetCurrentMainWindow()->IsPrivateBrowsing())
		return;

	//QUrl cleanUrl(url);
	//cleanUrl.setHost(cleanUrl.host());
	HistoryItem item(url, QDateTime::currentDateTime(), title, icon);
	emit EntryAdded(item);
}

void HistoryManager::RemoveHistoryEntry(const QString& url)
{
	if (BrowserApplication::GetInstance()->GetCurrentMainWindow()->IsPrivateBrowsing())
		return;

	//QUrl cleanUrl(url);
	//cleanUrl.setHost(cleanUrl.host());
	HistoryItem item(url, QDateTime::currentDateTime());
	emit EntryRemoved(item);
}

void HistoryManager::SetHistoryEntryIcon(const QString& url, const QIcon& icon)
{
	HistoryItem* historyItem = GetHistoryItemByUrl(url);
	if(historyItem)
		historyItem->icon = icon;
}

HistoryItem* HistoryManager::GetHistoryItemByUrl(const QString& url)
{
	for (int i = 0; i < history.length(); i++)
	{
		if (history[i].url == url)
			return &(history[i]);
	}
	return 0;
}

HistoryModel::HistoryModel(HistoryManager* _historyManager, QObject* parent)
	: QAbstractTableModel(parent)
	, historyManager(_historyManager)
{
	Q_ASSERT(historyManager);

	connect(historyManager, SIGNAL(EntryAdded(HistoryItem)),
			this, SLOT(EntryAdded(HistoryItem)));
	connect(historyManager, SIGNAL(EntryRemoved(HistoryItem)),
			this, SLOT(EntryRemoved(HistoryItem)));
}

void HistoryModel::EntryAdded(const HistoryItem& item)
{
	beginInsertRows(QModelIndex(), 0, 0);
	historyManager->history.prepend(item);
	endInsertRows();
}

void HistoryModel::EntryRemoved(const HistoryItem& item)
{
	int index = historyManager->history.indexOf(item);
	Q_ASSERT(index > -1);
	beginRemoveRows(QModelIndex(), index, index);
	historyManager->history.takeAt(index);
	endRemoveRows();
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
		case 0: return tr("DateTime");
		case 1: return tr("Title");
		case 2: return tr("Address");
		}
	}
	return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
	QList<HistoryItem> history = historyManager->GetHistory();
	if (index.row() < 0 || index.row() >= history.size())
		return QVariant();

	const HistoryItem& item = history[index.row()];
	switch(role)
	{
	case DateTimeRole:
		return item.dateTime;
	case DateRole:
		return item.dateTime.date();
	case UrlRole:
		return QUrl(item.url);
	case UrlStringRole:
		return item.url;
	case Qt::DisplayRole:
	case Qt::EditRole:
	{
		switch (index.column())
		{
		case 0:
			return item.dateTime;
		case 1:
			if (item.title.isEmpty())
			{
				// when there is no title try to generate one from the url
				QString page = QFileInfo(QUrl(item.url).path()).fileName();
				if (!page.isEmpty())
					return page;
				else
					return item.url;
			}
			return item.title;
		case 2:
			return item.url;		
		}
	}
	case Qt::DecorationRole:
		if (index.column() == 1)
		{
			return item.icon;
		}
	}
	return QVariant();
}

int HistoryModel::columnCount(const QModelIndex &parent) const
{
	return (parent.isValid()) ? 0 : 3;
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
	return (parent.isValid()) ? 0 : historyManager->history.count();
}

bool HistoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid())
		return false;

	int lastRow = row + count - 1;
	beginRemoveRows(parent, row, lastRow);
	QList<HistoryItem>& history = historyManager->history;
	for (int i = lastRow; i >= row; i--)
		history.removeAt(i);
	endRemoveRows();
	return true;
}

HistoryProxyModel::HistoryProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
{
	setSortRole(HistoryModel::DateTimeRole);
	setFilterCaseSensitivity(Qt::CaseInsensitive);
}

HistoryDialog::HistoryDialog(HistoryManager* historyManager, QWidget* parent)
	: QDialog(parent)
{
	Q_ASSERT(historyManager);
	setupUi(this);

	treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	HistoryProxyModel* proxyModel = historyManager->GetHistoryProxyModel();
	proxyModel->setFilterKeyColumn(1);
	connect(searchLineEdit, SIGNAL(textChanged(QString)),
			proxyModel, SLOT(setFilterFixedString(QString)));
	treeView->setModel(proxyModel);
	treeView->setSortingEnabled(true);
}

void HistoryDialog::closeEvent(QCloseEvent* event)
{
	event->accept();
	deleteLater();
}

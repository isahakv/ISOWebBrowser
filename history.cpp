#include "history.h"

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "historytreeview.h"

#include <QCloseEvent>
#include <QFileInfo>
#include <QMenu>
#include <QClipboard>

#include <QWebEngineSettings>

/** HistoryManager */
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

	int index = GetHistoryItemIndexByUrl(url);
	if (index != -1)
		RemoveHistoryEntry(url);

	HistoryItem item(url, QDateTime::currentDateTime(), title, icon);
	emit EntryAdded(item);
}

void HistoryManager::RemoveHistoryEntry(const QString& url)
{
	if (BrowserApplication::GetInstance()->GetCurrentMainWindow()->IsPrivateBrowsing())
		return;

	emit EntryRemoved(*GetHistoryItemByUrl(url));
}

void HistoryManager::RemoveAllHistory()
{
	if (BrowserApplication::GetInstance()->GetCurrentMainWindow()->IsPrivateBrowsing())
		return;

	emit AllEntriesRemoved();
}

void HistoryManager::SetHistoryEntryIcon(const QString& url, const QIcon& icon)
{
	HistoryItem* historyItem = GetHistoryItemByUrl(url);
	if(historyItem)
		historyItem->icon = icon;
}

void HistoryManager::SetHistoryEntryTitle(const QString& url, const QString& title)
{
	HistoryItem* historyItem = GetHistoryItemByUrl(url);
	if(historyItem)
		historyItem->title = title;
}

HistoryItem* HistoryManager::GetHistoryItemByUrl(const QString& url)
{
	int index = GetHistoryItemIndexByUrl(url);
	if (index == -1)
		return 0;

	return &(history[index]);
}

int HistoryManager::GetHistoryItemIndexByUrl(const QString& url) const
{
	for (int i = 0; i < history.length(); i++)
	{
		if (history[i].url == url)
			return i;
	}
	return -1;
}

/** HistoryModel */
HistoryModel::HistoryModel(HistoryManager* _historyManager, QObject* parent)
	: QAbstractTableModel(parent)
	, historyManager(_historyManager)
{
	Q_ASSERT(historyManager);

	connect(historyManager, SIGNAL(EntryAdded(HistoryItem)),
			this, SLOT(EntryAdded(HistoryItem)));
	connect(historyManager, SIGNAL(EntryRemoved(HistoryItem)),
			this, SLOT(EntryRemoved(HistoryItem)));
	connect(historyManager, SIGNAL(AllEntriesRemoved()),
			this, SLOT(AllEntriesRemoved()));
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

void HistoryModel::AllEntriesRemoved()
{
	beginResetModel();
	historyManager->history.clear();
	endResetModel();
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
	const QList<HistoryItem>& history = historyManager->GetHistory();
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
		if (index.column() == 0)
			return item.icon;
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

/** HistoryProxyModel */
HistoryProxyModel::HistoryProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
{
	setSortRole(HistoryModel::DateTimeRole);
	setFilterCaseSensitivity(Qt::CaseInsensitive);
}

/** HistoryDialog */
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
	connect(removeButton, SIGNAL(clicked(bool)), treeView, SLOT(RemoveOne()));
	connect(removeAllButton, SIGNAL(clicked(bool)), treeView, SLOT(RemoveAll()));
	treeView->setModel(proxyModel);
	treeView->setSortingEnabled(true);
	treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(treeView, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(CustomContextMenuRequested(QPoint)));
}

void HistoryDialog::closeEvent(QCloseEvent* event)
{
	event->accept();
	deleteLater();
}

void HistoryDialog::CustomContextMenuRequested(const QPoint& pos)
{
	QMenu menu;
	QModelIndex index = treeView->indexAt(pos);
	if (index.isValid())
	{
		menu.addAction("Open", this, SLOT(OpenHistoryItem()));
		menu.addAction("Copy", this, SLOT(CopyHistoryItem()));
		menu.addAction("Remove", treeView, SLOT(RemoveOne()));
	}
	menu.addAction("Remove All", treeView, SLOT(RemoveAll()));
	menu.exec(QCursor::pos());
}

void HistoryDialog::OpenHistoryItem()
{
	QModelIndex index = treeView->currentIndex();
	if (!index.isValid())
		return;

	QUrl url = index.data(HistoryModel::UrlRole).toUrl();
	BrowserApplication::GetInstance()->GetCurrentMainWindow()->LoadUrlInCurrentTab(url); // Note: Change this to signal???
}

void HistoryDialog::CopyHistoryItem()
{
	QModelIndex index = treeView->currentIndex();
	if (!index.isValid())
		return;

	QString url = index.data(HistoryModel::UrlStringRole).toString();
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(url);
}

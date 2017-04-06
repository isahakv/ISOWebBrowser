#ifndef HISTORY_H
#define HISTORY_H

#include <QtCore/QObject>
#include <QDateTime>
#include <QDialog>

#include <QtCore/QSortFilterProxyModel>

#include "ui_history.h"

class HistoryItem
{
public:
	HistoryItem() {}
	HistoryItem(const QString& _url, const QDateTime& _dateTime,
				const QString& _title = QString(), const QIcon& _icon = QIcon())
		: url(_url), dateTime(_dateTime), title(_title), icon(_icon) {}

	inline bool operator==(const HistoryItem& other) const
	{
		return title == other.title && url == other.url && dateTime == other.dateTime;
	}

	QString title;
	QString url;
	QIcon icon;
	QDateTime dateTime;
};

class HistoryModel;
class HistoryProxyModel;

class HistoryManager : public QObject
{
	Q_OBJECT

signals:
	void EntryAdded(const HistoryItem& item);
	void EntryRemoved(const HistoryItem& item);
	void AllEntriesRemoved();

public:
	HistoryManager(QObject* parent = 0);

	void AddHistoryEntry(const QString& url, const QString& title, const QIcon& icon);
	void RemoveHistoryEntry(const QString& url);
	void RemoveAllHistory();
	void SetHistoryEntryIcon(const QString& url, const QIcon& icon);
	void SetHistoryEntryTitle(const QString& url, const QString& title);

	HistoryItem* GetHistoryItemByUrl(const QString& url);
	int GetHistoryItemIndexByUrl(const QString& url) const;

	inline QList<HistoryItem>& GetHistory() { return history; }

	inline HistoryModel* GetHistoryModel() const { return historyModel; }
	inline HistoryProxyModel* GetHistoryProxyModel() const { return historyProxyModel; }

	QList<HistoryItem> history;

private:
	HistoryModel* historyModel;
	HistoryProxyModel* historyProxyModel;
};

class HistoryModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum Roles
	{
		DateRole = Qt::UserRole + 1,
		DateTimeRole = Qt::UserRole + 2,
		UrlRole = Qt::UserRole + 3,
		UrlStringRole = Qt::UserRole + 4
	};

	HistoryModel(HistoryManager* _historyManager, QObject* parent = 0);
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

public slots:
	void EntryAdded(const HistoryItem& item);
	void EntryRemoved(const HistoryItem& item);
	void AllEntriesRemoved();

private:
	HistoryManager* historyManager;
};

class HistoryProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	HistoryProxyModel(QObject* parent = 0);

protected:
};

class HistoryDialog : public QDialog, public Ui_HistoryDialog
{
	Q_OBJECT

public:
	HistoryDialog(HistoryManager* historyManager, QWidget* parent = 0);

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void CustomContextMenuRequested(const QPoint& pos);
	void OpenHistoryItem();
	void CopyHistoryItem();
};

#endif // HISTORY_H

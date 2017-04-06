#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QDialog>
#include <QAbstractListModel>

class DownloadWidget
{

};

class DownloadManager
{
public:
	DownloadManager();
};

class DownloadModel : public QAbstractListModel
{
	Q_OBJECT

public:
	DownloadModel(DownloadManager* _downloadManager, QObject* parent = 0);
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:
	DownloadManager* downloadManager;
};

class DownloadDialog : public QDialog
{

};

#endif // DOWNLOADMANAGER_H

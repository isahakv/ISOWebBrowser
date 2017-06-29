#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "ui_downloads.h"
#include "ui_downloaditem.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QUrl>

#include <QWebEngineDownloadItem>

class DownloadManager;
class DownloadWidget : public QWidget, public Ui_DownloadItem
{
	Q_OBJECT

public:
	DownloadWidget(QWebEngineDownloadItem* _download, QWidget* parent = 0);

protected:
	QUrl url;
	QFileInfo file;
	qint64 bytesReceived;
	QTime downloadTime;
	bool stopped;

	QScopedPointer<QWebEngineDownloadItem> download;
};

class DownloadModel;

class DownloadManager : public QObject
{
	Q_OBJECT

signals:
	void OnItemAdded(int row, DownloadWidget* item);

public:
	DownloadManager(QObject* parent = 0);

	inline DownloadModel* GetDownloadModel() { return model; }
	inline QList<DownloadWidget*>& GetDownloads() { return downloads; }

public slots:
	void download(QWebEngineDownloadItem *download);
	void cleanup();

private slots:
	void updateRow();

private:
	void addItem(DownloadWidget* item);

	DownloadModel* model;
	QList<DownloadWidget*> downloads;

	friend class DownloadModel;
};

class DownloadModel : public QAbstractListModel
{
	friend class DownloadManager;
	Q_OBJECT

public:
	DownloadModel(DownloadManager* _downloadManager, QObject* parent = 0);
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

private:
	DownloadManager* downloadManager;
};

class DownloadDialog : public QDialog, public Ui_DownloadDialog
{
	Q_OBJECT

public:
	DownloadDialog(DownloadManager* _downloadManager, QWidget *parent = 0);

protected slots:
	void ItemAdded(int row, DownloadWidget* item);

private:
	DownloadManager* downloadManager;
};

#endif // DOWNLOADMANAGER_H

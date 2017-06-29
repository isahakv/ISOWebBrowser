#include "downloadmanager.h"

#include "browserapplication.h"

#include <math.h>

#include <QtCore/QMetaEnum>
#include <QtCore/QSettings>

#include <QtGui/QDesktopServices>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QFileIconProvider>

#include <QtCore/QDebug>

#include <QWebEngineSettings>
#include <QWebEngineDownloadItem>

/** DownloadWidget */
DownloadWidget::DownloadWidget(QWebEngineDownloadItem* _download, QWidget* parent)
	: QWidget(parent)
	, download(_download)
	, bytesReceived(0)
{
	setupUi(this);

	if (download)
	{
		file.setFile(download->path());
		url = download->url();
	}
}

/** DownloadManager */
DownloadManager::DownloadManager(QObject* parent)
	: QObject(parent)
{
	model = new DownloadModel(this, this);
}

void DownloadManager::download(QWebEngineDownloadItem *download)
{
	DownloadWidget* downloadItem = new DownloadWidget(download, 0);
	addItem(downloadItem);
}

void DownloadManager::cleanup()
{

}

void DownloadManager::updateRow()
{

}

void DownloadManager::addItem(DownloadWidget* item)
{
	int row = downloads.count();
	model->beginInsertRows(QModelIndex(), row, row);
	downloads.append(item);
	model->endInsertRows();

	emit OnItemAdded(row, item);
}

/** DownloadModel */
DownloadModel::DownloadModel(DownloadManager *_downloadManager, QObject *parent)
	: QAbstractListModel(parent)
	, downloadManager(_downloadManager)
{
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= rowCount(index.parent()))
		return QVariant();

	//if (role == Qt::ToolTipRole)
	//	if (!downloadManager->GetDownloads().at(index.row())->downloadedSuccessfully())
	//		return downloadManager->GetDownloads().at(index.row())->downloadInfoLabel->text();
	return QVariant();
}

int DownloadModel::rowCount(const QModelIndex &parent) const
{
	return (parent.isValid()) ? 0 : downloadManager->GetDownloads().count();
}

bool DownloadModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid())
		return false;
	return false;
}

/** DownloadDialog */
DownloadDialog::DownloadDialog(DownloadManager *_downloadManager, QWidget *parent)
	: QDialog(parent)
	, downloadManager(_downloadManager)
{
	setupUi(this);
	downloadsView->setShowGrid(false);
	downloadsView->verticalHeader()->hide();
	downloadsView->horizontalHeader()->hide();
	downloadsView->setAlternatingRowColors(true);
	downloadsView->horizontalHeader()->setStretchLastSection(true);
	downloadsView->setModel(downloadManager->GetDownloadModel());

	connect(downloadManager, SIGNAL(OnItemAdded(int,DownloadWidget*)),
			this, SLOT(ItemAdded(int,DownloadWidget*)));
}

void DownloadDialog::ItemAdded(int row, DownloadWidget* item)
{
	//int row = downloadManager->GetDownloads().indexOf(downloadItem);
	downloadsView->setIndexWidget(downloadManager->GetDownloadModel()->index(row, 0), item);
	downloadsView->setRowHeight(row, item->sizeHint().height());
}

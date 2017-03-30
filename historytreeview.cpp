#include "historytreeview.h"

#include <QKeyEvent>

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "history.h"

HistoryTreeView::HistoryTreeView(QWidget* parent)
	: QTreeView(parent)
{
}

void HistoryTreeView::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Delete
		 || event->key() == Qt::Key_Backspace) && model())
	{
		RemoveOne();
	}
	else
		QAbstractItemView::keyPressEvent(event);
}

void HistoryTreeView::RemoveOne()
{
	if (!model())
		return;

	QModelIndex index = currentIndex();
	BrowserApplication::GetHistoryManager()->RemoveHistoryEntry(model()->data(index, HistoryModel::UrlStringRole).toString());
}

void HistoryTreeView::RemoveAll()
{
	if (!model())
		return;

	BrowserApplication::GetHistoryManager()->RemoveAllHistory();
}

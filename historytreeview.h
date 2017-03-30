#ifndef HISTORYTREEVIEW_H
#define HISTORYTREEVIEW_H

#include <QTreeView>

class HistoryTreeView : public QTreeView
{
	Q_OBJECT

public:
	HistoryTreeView(QWidget* parent = 0);
	void keyPressEvent(QKeyEvent* event);

public slots:
	void RemoveOne();
	void RemoveAll();
};

#endif // HISTORYTREEVIEW_H

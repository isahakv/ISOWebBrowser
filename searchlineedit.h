#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "urllineedit.h"

#include <QtWidgets/QLineEdit>

class BrowserMainWindow;
class QStringListModel;

class SearchLineEdit : public BrowserLineEdit
{
	Q_OBJECT
public:
	SearchLineEdit(QWidget* parent, bool _haveHistory = false);

	QMenu* GetMenu() const;
	bool GetHaveHistory() const { return haveHistory; }

signals:
	void Search(const QUrl& url);

protected:
	void paintEvent(QPaintEvent* event);

public slots:
	void ClearRecentSearches();
	void SlotSearch();

private slots:
	void AboutToShowMenu();
	void TriggeredMenuAction(QAction* action);
	void SaveSearchHistory();

private:
	void LoadSearchHistory();

	SearchButton* searchButton;
	bool haveHistory;
	int maxSavedSearches;
	QStringListModel* stringListModel;
};

#endif // SEARCHLINEEDIT_H

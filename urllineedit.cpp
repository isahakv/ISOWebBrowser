#include "urllineedit.h"

#include "webview.h"
#include "browsermainwindow.h"

#include <QApplication>
#include <QtCore/QEvent>

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QFocusEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOptionFrame>
#include <QStringListModel>
#include <QSettings>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

SearchButton::SearchButton(QWidget *parent)
	: QAbstractButton(parent)
	, menu(0)
{
#ifndef QT_NO_CURSOR
	setCursor(Qt::ArrowCursor);
#endif // QT_NO_CURSOR
	setToolTip(tr("Recent Searches"));
	setFocusPolicy(Qt::NoFocus);
}

void SearchButton::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)

	QPainter painter(this);
	QPixmap pixmap(":Images/Vector/search.svg");

	painter.drawPixmap(0, 0, width(), height(), pixmap);
}

void SearchButton::mousePressEvent(QMouseEvent* event)
{
	if (menu && event->button() == Qt::LeftButton)
	{
		QWidget* parent = parentWidget();
		if (parent)
		{
			QPoint r = parent->mapToGlobal(QPoint(0, parent->height()));
			menu->exec(QPoint(r.x(), r.y()));
		}

		event->accept();
	}

	QAbstractButton::mousePressEvent(event);
}

// ClearButton
ClearButton::ClearButton(QWidget *parent)
	: QAbstractButton(parent)
{
#ifndef QT_NO_CURSOR
	setCursor(Qt::ArrowCursor);
#endif // QT_NO_CURSOR
	setToolTip(tr("Clear"));
	setVisible(false);
	setFocusPolicy(Qt::NoFocus);
}

void ClearButton::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)

	QPainter painter(this);

	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
	painter.drawPixmap(0, 0, 16, 16, QPixmap(":Images/Vector/cancel-button.svg"));
	int width = this->width();
	int height = this->height();

	/*painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setBrush(isDown() ? palette().color(QPalette::Dark)
							  : palette().color(QPalette::Mid));
	painter.setPen(painter.brush().color());
	int size = this->width();
	int offset = size / 5;
	int radius = size/2 - offset;
	painter.drawEllipse(0, 0, width, height);

	painter.setPen(palette().color(QPalette::Base));
	int border = offset*2;
	painter.drawLine(border, border, (width-border), (height-border));
	painter.drawLine((width-border), border, border, (height-border));*/
}

void ClearButton::TextChanged(const QString& text)
{
	setVisible(!text.isEmpty());
}

// BrowserLineEdit
BrowserLineEdit::BrowserLineEdit(QWidget *parent, BrowserMainWindow* ownerMainWindow)
	: QWidget(parent)
	, ownerBrowserMainWindow(ownerMainWindow)
	, lineEdit(new QLineEdit(this))
	, leftWidget(0)
	, clearButton(0)
{
	setFocusPolicy(lineEdit->focusPolicy());
	setAttribute(Qt::WA_InputMethodEnabled);
	setSizePolicy(lineEdit->sizePolicy());
	setMouseTracking(true);
	setAcceptDrops(true);
	setAttribute(Qt::WA_MacShowFocusRect, true);
	setPalette(lineEdit->palette());

	// line edit
	lineEdit->setFrame(false);
	lineEdit->setFocusProxy(this);
	//lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

	// clearButton
	clearButton = new ClearButton(this);
	/*clearButton->setIcon(QIcon(":Images/Vector/cancel-button.svg"));
	clearButton->setAttribute(Qt::WA_TranslucentBackground);
	clearButton->setAttribute(Qt::WA_MacShowFocusRect, false);
	QPalette p( clearButton->palette() );
	p.setBrush( QPalette::Button, Qt::NoBrush );
	clearButton->setPalette( p );
	clearButton->setBackgroundRole(QPalette::NoRole);*/

	connect(clearButton, SIGNAL(clicked(bool)),
			lineEdit, SLOT(clear()));
	connect(lineEdit, SIGNAL(textChanged(QString)),
			clearButton, SLOT(TextChanged(QString)));
}

QSize BrowserLineEdit::SizeHint() const
{
	lineEdit->setFrame(true);
	QSize size = lineEdit->sizeHint();
	lineEdit->setFrame(false);
	return size;
}

QVariant BrowserLineEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
	return lineEdit->inputMethodQuery(property);
}

void BrowserLineEdit::focusInEvent(QFocusEvent* event)
{
	// qWarning("focusInEvent");
	lineEdit->event(event);
	//if (event->)
	{
		lineEdit->selectAll();
	}
	QWidget::focusInEvent(event);
}

void BrowserLineEdit::focusOutEvent(QFocusEvent* event)
{
	lineEdit->event(event);
	QWidget::focusOutEvent(event);
}

void BrowserLineEdit::keyPressEvent(QKeyEvent* event)
{
	lineEdit->event(event);
}

void BrowserLineEdit::resizeEvent(QResizeEvent* event)
{
	qWarning("resizeEvent");
	UpdateGeometries();
	QWidget::resizeEvent(event);
}

void BrowserLineEdit::inputMethodEvent(QInputMethodEvent* event)
{
	lineEdit->event(event);
}

void BrowserLineEdit::UpdateGeometries()
{
	QStyleOptionFrame panel;
	InitStyleOption(&panel);
	QRect rect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

	int height = rect.height();
	int width = rect.width();

	lineEdit->setGeometry(0, 0, width, height);

	int clearButtonSize = this->height() / 3;
	int x = this->width() - clearButtonSize;
	int y = (this->height() - clearButtonSize)/2;
	clearButton->setGeometry(x, y, clearButtonSize, clearButtonSize);
}

void BrowserLineEdit::InitStyleOption(QStyleOptionFrame *option) const
{
	option->initFrom(this);
	option->rect = contentsRect();
	option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
	option->midLineWidth = 0;
	option->state |= QStyle::State_Sunken;
	if (lineEdit->isReadOnly())
		option->state |= QStyle::State_ReadOnly;
#ifdef QT_KEYPAD_NAVIGATION
	if (hasEditFocus())
		option->state |= QStyle::State_HasEditFocus;
#endif
	option->features = QStyleOptionFrame::None;
}

// UrlLineEdit
UrlLineEdit::UrlLineEdit(QWidget *parent, BrowserMainWindow* ownerMainWindow)
	: BrowserLineEdit(parent, ownerMainWindow)
	, webView(0)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(lineEdit);
}

void UrlLineEdit::SetWebView(WebView* _webView)
{
	Q_ASSERT(_webView);
	webView = _webView;
	connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(WebViewUrlChanged(QUrl)));
	connect(webView, SIGNAL(loadProgress(int)), this, SLOT(update()));
}

void UrlLineEdit::focusOutEvent(QFocusEvent* event)
{
	BrowserLineEdit::focusOutEvent(event);

	if (lineEdit->text().isEmpty())
		lineEdit->setText(QString::fromUtf8(webView->url().toEncoded()));
}

void UrlLineEdit::WebViewUrlChanged(const QUrl& url)
{
	lineEdit->setText(QString::fromUtf8(url.toEncoded()));
}


// SearchLineEdit
SearchLineEdit::SearchLineEdit(QWidget* parent, BrowserMainWindow* ownerMainWindow)
	: BrowserLineEdit(parent, ownerMainWindow)
	, searchButton(new SearchButton(this))
	, maxSavedSearches(10)
	, stringListModel(new QStringListModel(this))
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(lineEdit);

	SetLeftWidget(searchButton);
	inactiveText = tr("Search");

	QMenu* m = GetMenu();
	connect(m, SIGNAL(aboutToShow()), this, SLOT(AboutToShowMenu()));
	connect(m, SIGNAL(triggered(QAction*)), this, SLOT(TriggeredMenuAction(QAction*)));

	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(SlotSearch()));
	LoadSearchHistory();
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

void SearchLineEdit::UpdateGeometries()
{
	BrowserLineEdit::UpdateGeometries();

	int clearButtonSize = this->height() / 3;
	int x = clearButtonSize;//this->width() - clearButtonSize;
	int y = (this->height() - clearButtonSize)/2;
	searchButton->setGeometry(x, y, clearButtonSize, clearButtonSize);
}

void SearchLineEdit::ClearRecentSearches()
{
	stringListModel->setStringList(QStringList());
	SaveSearchHistory();
}

void SearchLineEdit::SlotSearch()
{
	QString searchText = lineEdit->text();
	QStringList newList = stringListModel->stringList();
	if (newList.contains(searchText))
		newList.removeAt(newList.indexOf(searchText));
	newList.prepend(searchText);

	if (newList.count() >= maxSavedSearches)
		newList.removeLast();

	if (ownerBrowserMainWindow && !(ownerBrowserMainWindow->IsPrivateBrowsing()))
	{
		stringListModel->setStringList(newList);
		SaveSearchHistory();
	}

	QSettings settings(QString("ISOBrowser"));
	settings.beginGroup(QString("General"));
	QUrl url(settings.value(QString("DefaultSearchEngine"),
							BrowserMainWindow::defaultSearchEngine).toString());
	settings.endGroup();

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
	QVariant v = action->data();
	if (v.canConvert<QString>())
	{
		QString text = v.toString();
		lineEdit->setText(text);
		SlotSearch();
	}
}

void SearchLineEdit::SaveSearchHistory()
{
	QSettings settings(QString("ISOBrowser"));
	settings.beginGroup(QLatin1String("ToolbarSearch"));
	settings.setValue(QString("RecentSearches"), stringListModel->stringList());
	settings.setValue(QLatin1String("MaxSavedSearches"), 5472);
	settings.endGroup();

	settings.sync();
}

void SearchLineEdit::LoadSearchHistory()
{
	QSettings settings(QString("ISOBrowser"));
	settings.beginGroup(QLatin1String("ToolbarSearch"));
	QStringList list = settings.value(QLatin1String("RecentSearches")).toStringList();
	maxSavedSearches = settings.value(QLatin1String("MaxSavedSearches"), maxSavedSearches).toInt();
	settings.endGroup();
	stringListModel->setStringList(list);
}

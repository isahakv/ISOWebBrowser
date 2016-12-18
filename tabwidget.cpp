#include "tabwidget.h"

#include "browsertypes.h"
#include "webview.h"
#include "urllineedit.h"

#include <QApplication>
#include <QClipboard>
#include <QAction>
#include <QSettings>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLabel>
#include <QPixmap>
#include <QMovie>

#include <QIODevice>

TabBar::TabBar(QWidget *parent)
	: QTabBar(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setAcceptDrops(true);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(ContextMenuRequested(QPoint)));

	setTabsClosable(true);
	connect(this, SIGNAL(tabCloseRequested(int)), this, SIGNAL(CloseTab(int)));
	setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
	setMovable(true);
}

void TabBar::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		dragStartPos = event->pos();

	QTabBar::mousePressEvent(event);

	// Middle click on tab should close it.
	if (event->button() == Qt::MiddleButton)
	{
		const QPoint pos = event->pos();
		int index = tabAt(pos);
		if (index != -1)
			emit CloseTab(index);
	}
}

void TabBar::mouseMoveEvent(QMouseEvent *event)
{
	QTabBar::mouseMoveEvent(event);
}

void TabBar::CloseTab()
{
	if(QAction* action = qobject_cast<QAction*>(sender()))
	{
		int index = action->data().toInt();
		emit CloseTab(index);
	}
}

void TabBar::ContextMenuRequested(const QPoint& position)
{
	QMenu menu;
}

TabWidget::TabWidget(QWidget *parent)
	: QTabWidget(parent),
	  tabBar(new TabBar(this)),
	  lineEdits(0)
{
	setElideMode(Qt::ElideRight);

	connect(tabBar, SIGNAL(NewTab()), this, SLOT(NewTab()));
	connect(tabBar, SIGNAL(CloseTab(int)), this, SLOT(RequestCloseTab(int)));
	setTabBar(tabBar);

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(SlotCurrentTabChanged(int)));

	lineEdits = new QStackedWidget(this);
}

TabWidget::~TabWidget()
{

}

void TabWidget::AddWebAction(QAction* action, QWebEnginePage::WebAction webAction)
{
	webActionMappers.append(new WebActionMapper(action, webAction, this));
}

QWidget* TabWidget::GetLineEditStack() const
{
	return lineEdits;
}

QLineEdit* TabWidget::GetCurrentLineEdit() const
{
	return GetLineEdit(lineEdits->currentIndex());
}

QLineEdit* TabWidget::GetLineEdit(int index) const
{
	UrlLineEdit* urlLineEdit = qobject_cast<UrlLineEdit*>(lineEdits->widget(index));
	if (urlLineEdit)
		return urlLineEdit->GetLineEdit();
	return NULL;
}

WebView* TabWidget::GetCurrentWebView() const
{
	return GetWebView(currentIndex());
}

WebView* TabWidget::GetWebView(int index) const
{
	QWidget* widget = this->widget(index);
	if (WebView* webview = qobject_cast<WebView*>(widget))
		return webview;
	return NULL;
}

int TabWidget::GetWebViewIndex(WebView* webView) const
{
	return indexOf(webView);
}

void TabWidget::LoadHomePage(WebView* tab)
{
	QSettings settings;
	settings.beginGroup(QLatin1String("MainWindow"));
	HomePageType homeType = qvariant_cast<HomePageType>(settings.value(QLatin1String("HomePageType"), (unsigned short)HomePageType::NewTabPage));
	settings.endGroup();
	switch (homeType)
	{
	case HomePageType::NewTabPage:
		LoadNewTabPage(tab);
		break;
	case HomePageType::SpecificPage:
		//LoadPage(home);
		break;
	}
}

void TabWidget::LoadNewTabPage(WebView* tab)
{
	QString htmlTxt;
	QFile html(":html/NewTab.html");
	html.open(QIODevice::OpenModeFlag::ReadOnly);
	/*QTextStream in(&html);
	while (!in.atEnd())
	{
		qWarning("fffff");
		in >> htmlTxt;
	}*/
	htmlTxt = html.readAll();
	//qWarning(htmlTxt.toStdString().c_str());

	// tab->load(QUrl("qrc:///html/NewTab.html"));
	tab->setHtml(htmlTxt);
	//tab->load(QUrl(":html/NewTab.html"));
/*
	// This code may move to another function
	int index = GetWebViewIndex(tab);
	QLabel* label = qobject_cast<QLabel*>(tabBar->tabButton(index, QTabBar::ButtonPosition::LeftSide));
	if (label)
	{
		QPixmap pixmap(QString(":Images/16x16/new-tab.png"));
		label->setPixmap(pixmap);
	}
	else
	{
		label = new QLabel(this);
		QPixmap pixmap(QString(":Images/16x16/new-tab.png"));
		label->setPixmap(pixmap);
		tabBar->setTabButton(index, QTabBar::ButtonPosition::LeftSide, label);
	}
	// This code may move to another function

	tab->iconChanged(QIcon(":Images/16x16/new-tab.png"));
	tab->loadFinished(true);*/
}

void TabWidget::LoadUrlInCurrentTab(const QUrl& url)
{
	WebView* webView = GetCurrentWebView();
	if (webView)
	{
		webView->LoadUrl(url);
		webView->setFocus();
	}
}

WebView* TabWidget::NewTab(bool makeCurrent, bool loadHomePage)
{
	// line edit
	UrlLineEdit* urlLineEdit = new UrlLineEdit;
	QLineEdit* lineEdit = urlLineEdit->GetLineEdit();

	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
	lineEdits->addWidget(urlLineEdit);
	lineEdits->setSizePolicy(lineEdit->sizePolicy());
	// lineEdits->setCurrentWidget(urlLineEdit);

	// webview
	WebView* webView = new WebView;
	urlLineEdit->SetWebView(webView);
	connect(webView, SIGNAL(loadStarted()), this, SLOT(SlotWebViewLoadStarted()));
	connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(SlotWebViewLoadFinished(bool)));
	connect(webView, SIGNAL(iconChanged(QIcon)), this, SLOT(WebViewIconChanged(QIcon)));
	connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(WebViewTitleChanged(QString)));
	connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(WebViewUrlChanged(QUrl)));

	addTab(webView, tr("Untitled"));
	if (makeCurrent)
		setCurrentWidget(webView);

	SetupPage(webView->page());

	if (loadHomePage)
		LoadHomePage(webView);
	else
		LoadNewTabPage(webView);

	//qWarning(QString("LineEditStack Count = %1").arg(lineEdits->count()).toStdString().c_str());
	return webView;
}

// When index is -1 index chooses the current tab
void TabWidget::RequestCloseTab(int index)
{
	if (index < 0)
		index = currentIndex();
	if (index < 0 || index >= count())
		return;

	WebView* tab = GetWebView(index);
	if (!tab)
		return;

	tab->page()->triggerAction(QWebEnginePage::RequestClose);
}

void TabWidget::CloseTab(int index)
{
	if (index < 0 || index >= count())
		return;

	bool hasFocus = false;
	if (WebView* tab = GetWebView(index))
	{
		hasFocus = tab->hasFocus();
	}

	// delete lineEdit
	QWidget* lineEdit = lineEdits->widget(index);
	lineEdits->removeWidget(lineEdit);
	lineEdit->deleteLater();

	// delete webView
	QWidget* webView = widget(index);
	removeTab(index);
	webView->deleteLater();
	if (hasFocus && count() > 0)
		GetCurrentWebView()->setFocus();
	if (count() == 0)
		emit LastTabClosed();

	//qWarning(QString("LineEditStack Count = %1").arg(lineEdits->count()).toStdString().c_str());
}

void TabWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && !childAt(event->pos())
			&& event->pos().y() < (tabBar->y() + tabBar->height()))
	{
		NewTab();
		return;
	}

	QTabWidget::mouseDoubleClickEvent(event);
}

void TabWidget::contextMenuEvent(QContextMenuEvent* event)
{
	if (!childAt(event->pos()) && event->pos().y() < (tabBar->y() + tabBar->height()))
	{
		tabBar->ContextMenuRequested(event->pos());
		return;
	}
	QTabWidget::contextMenuEvent(event);
}

void TabWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MidButton && !childAt(event->pos())
			&& event->pos().y() < (tabBar->y() + tabBar->height()))
	{
		QUrl url(QApplication::clipboard()->text());
		qWarning(url.toString().toStdString().c_str());
		if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty())
		{
			WebView* webView = NewTab();
			webView->setUrl(url);
		}
	}

	QTabWidget::mouseReleaseEvent(event);
}

void TabWidget::SlotCurrentTabChanged(int index)
{
	WebView* webView = GetWebView(index);
	if (!webView)
		return;

	Q_ASSERT(lineEdits->count() == count());

	WebView* oldWebView = GetWebView(lineEdits->currentIndex());
	if (oldWebView)
	{
		disconnect(oldWebView->page(), SIGNAL(linkHovered(QString)),
				   this, SIGNAL(WebPageLinkHovered(QString)));
		//disconnect(oldWebView, SIGNAL(loadProgress(int)),
		//		   this, SIGNAL(WebPageLoadProgress(int)));
	}

	connect(webView->page(), SIGNAL(linkHovered(QString)),
					   this, SIGNAL(WebPageLinkHovered(QString)));
	//connect(webView, SIGNAL(loadProgress(int)),
	//				   this, SIGNAL(WebPageLoadProgress(int)));

	for (int i = 0; i < webActionMappers.count(); i++)
	{
		webActionMappers[i]->UpdateCurrentPage(webView->page());
	}

	lineEdits->setCurrentIndex(index);
	if (webView->GetUrl().isEmpty())
		lineEdits->currentWidget()->setFocus();
	else
		webView->setFocus();

	emit CurrentTabChanged(index);
}

void TabWidget::SlotWebViewLoadStarted()
{
	WebView* webView = qobject_cast<WebView*>(sender());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		SetTabIconToGif(index, ":Images/16x16/loading.gif");

		emit WebViewLoadStarted(webView);
	}
}

// Deal with this function
void TabWidget::SlotWebViewLoadFinished(bool b)
{
	WebView* webView = qobject_cast<WebView*>(sender());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		SetTabIconToImage(index, webView->icon());

		emit WebViewLoadFinished(webView);
	}
}

void TabWidget::WebViewIconChanged(const QIcon& icon)
{
	qWarning("WebViewIconChanged Called");

	WebView* webView = qobject_cast<WebView*>(sender());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		SetTabIconToImage(index, icon);
	}
}

void TabWidget::WebViewTitleChanged(const QString& title)
{
	WebView* webView = qobject_cast<WebView*>(sender());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		setTabText(index, title);
	}
}

void TabWidget::WebViewUrlChanged(const QUrl& url)
{
	qWarning("WebViewUrlChanged Function Called!");

	WebView* webView = qobject_cast<WebView*>(sender());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		tabBar->setTabData(index, url);
	}
}

void TabWidget::LineEditReturnPressed()
{
	if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender()))
	{
		if (lineEdits->currentWidget() == lineEdit->parent())
		{
			emit LoadPage(lineEdit->text());
			GetCurrentWebView()->setFocus();
		}
	}
}

void TabWidget::WindowCloseRequested()
{
	QWebEnginePage* webPage = qobject_cast<QWebEnginePage*>(sender());
	WebView* webView = qobject_cast<WebView*>(webPage->view());
	int index = GetWebViewIndex(webView);
	if (index >= 0)
		CloseTab(index);
}

void TabWidget::SetupPage(QWebEnginePage* page)
{
	connect(page, SIGNAL(windowCloseRequested()),
			this, SLOT(WindowCloseRequested()));
	connect(page, SIGNAL(geometryChangeRequested(QRect)),
			this, SIGNAL(geometryChangeRequested(QRect)));

	// WebView Actions

}

void TabWidget::SetTabIconToImage(int tabIndex, const QIcon& icon)
{
	QLabel* label = qobject_cast<QLabel*>(tabBar->tabButton(tabIndex, QTabBar::ButtonPosition::LeftSide));
	if (label)
	{
		QPixmap pixmap = icon.pixmap(QSize(16, 16));
		label->setPixmap(pixmap);
	}
	else
	{
		label = new QLabel(this);
		QPixmap pixmap = icon.pixmap(QSize(16, 16));
		label->setPixmap(pixmap);
		tabBar->setTabButton(tabIndex, QTabBar::ButtonPosition::LeftSide, label);
	}
}

void TabWidget::SetTabIconToGif(int tabIndex, const QString& path)
{
	QLabel* label = qobject_cast<QLabel*>(tabBar->tabButton(tabIndex, QTabBar::ButtonPosition::LeftSide));
	if (label)
	{
		QMovie* movie = label->movie();
		if (!movie)
		{
			movie = new QMovie(path, QByteArray(), label);
			label->setMovie(movie);
		}
		movie->start();
	}
	else
	{
		label = new QLabel(this);
		QMovie* movie = new QMovie(path, QByteArray(), label);
		label->setMovie(movie);
		movie->start();
		tabBar->setTabButton(tabIndex, QTabBar::ButtonPosition::LeftSide, label);
	}
}

WebActionMapper::WebActionMapper(QAction *_rootAction, QWebEnginePage::WebAction _webAction, QObject *parent)
	: QObject(parent)
	, currentPage(0)
	, rootAction(_rootAction)
	, webAction(_webAction)
{
	if (!rootAction)
		return;

	connect(rootAction, SIGNAL(triggered(bool)), this, SLOT(RootTriggered()));
	connect(rootAction, SIGNAL(destroyed(QObject*)), this, SLOT(RootDistroyed()));
}

void WebActionMapper::UpdateCurrentPage(QWebEnginePage* newCurrentPage)
{
	if (currentPage)
	{
		disconnect(currentPage->action(webAction), SIGNAL(changed()),
			 this, SLOT(CurrentPageActionChanged()));
		disconnect(currentPage, SIGNAL(destroyed(QObject*)),
				   this, SLOT(CurrentPageDestroyed()));
	}

	currentPage = newCurrentPage;
	if (!rootAction)
		return;

	if (!currentPage)
	{
		rootAction->setEnabled(false);
		rootAction->setCheckable(false);
		return;
	}

	QAction* source = currentPage->action(webAction);
	rootAction->setEnabled(source->isEnabled());
	rootAction->setCheckable(source->isCheckable());
	connect(currentPage->action(webAction), SIGNAL(changed()),
			this, SLOT(CurrentPageActionChanged()));
	connect(currentPage, SIGNAL(destroyed(QObject*)),
					   this, SLOT(CurrentPageDestroyed()));
}

void WebActionMapper::RootTriggered()
{
	if (!currentPage)
		return;

	QAction* action = currentPage->action(webAction);
	action->trigger();
}

void WebActionMapper::RootDistroyed()
{
	rootAction = 0;
}

void WebActionMapper::CurrentPageActionChanged()
{
	QAction* source = qobject_cast<QAction*>(sender());
	if (rootAction && source)
	{
		rootAction->setEnabled(source->isEnabled());
		rootAction->setChecked(source->isChecked());
	}
}

void WebActionMapper::CurrentPageDestroyed()
{
	currentPage = 0;
}

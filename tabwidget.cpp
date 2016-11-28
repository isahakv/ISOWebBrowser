#include "tabwidget.h"

#include "webview.h"

#include <QAction>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>

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
	qWarning("Fuck You");
}

TabWidget::TabWidget(QWidget *parent)
	: QTabWidget(parent),
	  tabBar(new TabBar(this))
{
	setElideMode(Qt::ElideRight);

	connect(tabBar, SIGNAL(NewTab()), this, SLOT(newTab()));
	connect(tabBar, SIGNAL(CloseTab(int)), this, SLOT(RequestCloseTab(int)));
	setTabBar(tabBar);
}

TabWidget::~TabWidget()
{

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

WebView* TabWidget::newTab(bool makeCurrent)
{
	WebView* webView = new WebView;

	addTab(webView, tr("Untitled"));
	if (makeCurrent)
		setCurrentWidget(webView);

	SetupPage(webView->page());

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

	QWidget* webView = widget(index);
	removeTab(index);
	webView->deleteLater();
	if (hasFocus && count() > 0)
		GetCurrentWebView()->setFocus();
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
}

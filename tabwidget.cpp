#include "tabwidget.h"

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "webviewwrapper.h"
#include "urllineedit.h"
#include "history.h"
#include "downloadmanager.h"

#include <QWebEngineProfile>
#include <QWebEngineDownloadItem>

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
#include <QtWidgets/QToolButton>
#include <QPixmap>
#include <QMovie>

#include <QIODevice>

TabBar::TabBar(QWidget *parent)
	: QTabBar(parent)
{
	setStyleSheet("QTabBar::tab {"
				  "border: 2px solid #C4C4C3;"
				  "border-bottom-color: #C2C7CB;"
				  "border-top-left-radius: 4px;"
				  "border-top-right-radius: 4px;"
				  "min-width: 8ex;"
				  "padding: 2px; }"
				  "QTabBar::tab:selected {"
				  "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #b7b7b7, stop: 0.4 #919191, stop: 0.5 #878787, stop: 1.0 #707070);"
				  "border-color: #9B9B9B;"
				  "border-bottom-color: #C2C7CB; }"
				  "QTabBar::tab:!selected {"
				  "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
				  "margin-top: 2px; }");

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

void TabBar::CloneTab()
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
	{
		int index = action->data().toInt();
		emit CloneTab(index);
	}
}

void TabBar::CloseTab()
{
	if(QAction* action = qobject_cast<QAction*>(sender()))
	{
		int index = action->data().toInt();
		emit CloseTab(index);
	}
}

void TabBar::CloseOtherTabs()
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
	{
		int index = action->data().toInt();
		emit CloseOtherTabs(index);
	}
}

void TabBar::ReloadTab()
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
	{
		int index = action->data().toInt();
		emit ReloadTab(index);
	}
}

void TabBar::MuteTab()
{
	QAction* action = qobject_cast<QAction*>(sender());
	int index = action->data().toInt();
	emit MuteTab(index, true);
}

void TabBar::UnmuteTab()
{
	QAction* action = qobject_cast<QAction*>(sender());
	int index = action->data().toInt();
	emit MuteTab(index, false);
}

void TabBar::ContextMenuRequested(const QPoint& position)
{
	QMenu menu;
	menu.addAction(tr("New &Tab"), this, SIGNAL(NewTab()), QKeySequence::AddTab);
	int index = tabAt(position);
	if (index != -1)
	{
		QAction* action = menu.addAction(tr("Clone Tab"), this, SLOT(CloneTab()));
		action->setData(index);

		action = menu.addAction(tr("&Close Tab"), this, SLOT(CloseTab()), QKeySequence::Close);
		action->setData(index);

		action = menu.addAction(tr("&Close Other Tabs"), this, SLOT(CloseOtherTabs()));
		action->setData(index);

		menu.addSeparator();

		action = menu.addAction(tr("&Reload Tab"), this, SLOT(ReloadTab()), QKeySequence::Refresh);
		action->setData(index);

		// Audio Mute / Unmute
		action = menu.addAction(tr("Mute Tab"), this, SLOT(MuteTab()));
		action->setData(index);

		action = menu.addAction(tr("Unmute Tab"), this, SLOT(UnmuteTab()));
		action->setData(index);
	}
	else
		menu.addSeparator();

	menu.addAction(tr("Reload All Tabs"), this, SIGNAL(ReloadAllTabs()));
	menu.exec(QCursor::pos());
}

int TabWidget::MaxSymbolsInTabTitle = 20; // move this to another file...

TabWidget::TabWidget(QWidget *parent, BrowserMainWindow* ownerMainWindow)
	: QTabWidget(parent)
	, ownerBrowserMainWindow(ownerMainWindow)
	, tabBar(new TabBar(this))
	, lineEdits(0)
	, profile(QWebEngineProfile::defaultProfile())
{
	setElideMode(Qt::ElideRight);
	setStyleSheet(QString("QTabWidget::pane { border-top: 2px solid #C2C7CB; }"
						  "QTabWidget::tab-bar { left: 5px }"));

	QToolButton* newTabButton = new QToolButton(this);
	setCornerWidget(newTabButton, Qt::TopRightCorner);
	newTabButton->setAutoRaise(true);
	newTabButton->setIcon(QIcon(":/Images/Vector/addTab.svg"));
	newTabButton->setToolTip(tr("Add Tab"));
	connect(newTabButton, SIGNAL(clicked()), this, SLOT(NewTab()));

	connect(tabBar, SIGNAL(NewTab()), this, SLOT(NewTab()));
	connect(tabBar, SIGNAL(CloneTab(int)), this, SLOT(CloneTab(int)));
	connect(tabBar, SIGNAL(CloseTab(int)), this, SLOT(RequestCloseTab(int)));
	connect(tabBar, SIGNAL(CloseOtherTabs(int)), this, SLOT(CloseOtherTabs(int)));
	connect(tabBar, SIGNAL(ReloadTab(int)), this, SLOT(ReloadTab(int)));
	connect(tabBar, SIGNAL(MuteTab(int,bool)), this, SLOT(SetAudioMutedForTab(int,bool)));
	connect(tabBar, SIGNAL(ReloadAllTabs()), this, SLOT(ReloadAllTabs()));
	connect(tabBar, SIGNAL(tabMoved(int,int)), this, SLOT(MoveTab(int,int)));
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
	if (WebViewWrapper* webViewWrapper = qobject_cast<WebViewWrapper*>(widget))
		return webViewWrapper->GetWebView();
	return NULL;
}

int TabWidget::GetWebViewIndex(WebView* webView) const
{
	return indexOf(webView->GetOwnerWebViewWrapper());
}

void TabWidget::SetProfile(QWebEngineProfile* newProfile)
{
	profile = newProfile;
	for (int i = 0; i < count(); i++)
	{
		if (WebView* webView = GetWebView(i))
		{
			WebPage* webPage = new WebPage(profile, webView);
			SetupPage(webPage);
			webPage->load(webView->page()->url());
			webView->setPage(webPage);
		}
	}
}

void TabWidget::LoadHomePage(WebView* tab, HomePageType homePageType)
{
	if (!tab)
		tab = GetCurrentWebView();
	if (!tab)
		return;

	if (homePageType == HomePageType::None)
	{
		QSettings settings("ISOBrowser");
		settings.beginGroup(QLatin1String("General"));
		homePageType = qvariant_cast<HomePageType>(settings.value(QLatin1String("HomePageType"),
															(unsigned short)HomePageType::NewTabPage));
		settings.endGroup();
	}

	switch (homePageType)
	{
	case HomePageType::NewTabPage:
		LoadNewTabPage(tab);
		break;
	case HomePageType::SpecificPage:
		LoadPage(tab, BrowserMainWindow::GetHomePage());
		break;
	}
}

void TabWidget::LoadNewTabPage(WebView* tab)
{
	QString htmlTxt;
	QFile html;

	if (ownerBrowserMainWindow && ownerBrowserMainWindow->IsPrivateBrowsing())
		html.setFileName(":html/NewIncognitoTab.html");
	else
		html.setFileName(":html/NewTab.html");

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
}

// If tab == NULL, Then Load on Current Tab, Note: maybe this will be deleted later...
void TabWidget::LoadUrl(WebView* tab, const QUrl& url)
{
	if (!tab)
		tab = GetCurrentWebView();
	if (!tab)
		return;

	tab->LoadUrl(url);
	tab->setFocus();
}

WebViewWrapper* TabWidget::NewTab(bool makeCurrent, HomePageType homePageType)
{
	// line edit
	UrlLineEdit* urlLineEdit = new UrlLineEdit(this);
	urlLineEdit->SetOwnerBrowserMainWindow(ownerBrowserMainWindow);
	QLineEdit* lineEdit = urlLineEdit->GetLineEdit();

	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
	lineEdits->addWidget(urlLineEdit);
	//lineEdits->setSizePolicy(lineEdit->sizePolicy());
	// lineEdits->setCurrentWidget(urlLineEdit);

	// webview
	WebViewWrapper* webViewWrapper = new WebViewWrapper;
	WebView* webView = webViewWrapper->GetWebView();
	webView->setPage(new WebPage(profile, webView));
	urlLineEdit->SetWebView(webView);
	connect(webView, SIGNAL(loadStarted()), this, SLOT(SlotWebViewLoadStarted()));
	connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(SlotWebViewLoadFinished(bool)));
	connect(webView, SIGNAL(iconChanged(QIcon)), this, SLOT(WebViewIconChanged(QIcon)));
	connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(WebViewTitleChanged(QString)));
	connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(WebViewUrlChanged(QUrl)));
	connect(webView->page(), SIGNAL(audioMutedChanged(bool)),
			this, SLOT(WebPageMutedOrAudibleChanged()));
	connect(webView->page(), SIGNAL(recentlyAudibleChanged(bool)),
			this, SLOT(WebPageMutedOrAudibleChanged()));

	addTab(webViewWrapper, tr("Untitled"));
	if (makeCurrent)
		setCurrentWidget(webViewWrapper);

	SetupPage(webView->page());
	LoadHomePage(webView, homePageType);

	//qWarning(QString("LineEditStack Count = %1").arg(lineEdits->count()).toStdString().c_str());
	return webViewWrapper;
}

// When index is -1 index chooses the current tab
void TabWidget::CloneTab(int index)
{
	if (index < 0)
		index = currentIndex();
	if (index < 0 || index >= count())
		return;

	WebView* webView = NewTab(false)->GetWebView();
	webView->setUrl(GetWebView(index)->url());
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

void TabWidget::CloseOtherTabs(int index)
{
	if (index < 0)
		index = currentIndex();
	if (index < 0 || index >= count())
		return;

	for (int i = count() - 1; i >= 0;  i--)
	{
		if (index == i)
			continue;

		CloseTab(i);
	}
}

// When index is -1 index chooses the current tab
void TabWidget::ReloadTab(int index)
{
	if (index < 0)
		index = currentIndex();
	if (index < 0 || index >= count())
		return;

	WebView* webView = GetWebView(index);
	if (webView)
		webView->reload();
}

void TabWidget::ReloadAllTabs()
{
	for (int i = 0; i < count(); i++)
	{
		WebView* webView = GetWebView(i);
		if (webView)
			webView->reload();
	}
}

void TabWidget::SetAudioMutedForTab(int index, bool mute)
{
	if (index < 0)
		index = currentIndex();
	if (index < 0 || index >= count())
		return;

	WebView* webView = GetWebView(index);
	if (webView)
		webView->page()->setAudioMuted(mute);
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
			WebViewWrapper* webViewWrapper = NewTab();
			webViewWrapper->GetWebView()->setUrl(url);
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
		disconnect(oldWebView->page()->profile(), SIGNAL(downloadRequested(QWebEngineDownloadItem*)),
				   this, SLOT(WebPageDownloadRequested(QWebEngineDownloadItem*)));
	}

	connect(webView->page(), SIGNAL(linkHovered(QString)),
					   this, SIGNAL(WebPageLinkHovered(QString)));
	//connect(webView, SIGNAL(loadProgress(int)),
	//				   this, SIGNAL(WebPageLoadProgress(int)));
	connect(webView->page()->profile(), SIGNAL(downloadRequested(QWebEngineDownloadItem*)),
					   this, SLOT(WebPageDownloadRequested(QWebEngineDownloadItem*)));

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
	Q_UNUSED(b)

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
	WebView* webView = qobject_cast<WebView*>(sender());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		SetTabIconToImage(index, icon);
		HistoryManager* historyManager = BrowserApplication::GetHistoryManager();
		historyManager->SetHistoryEntryIcon(webView->url().toString(), icon);
	}
}

void TabWidget::WebViewTitleChanged(const QString& title)
{
	WebView* webView = qobject_cast<WebView*>(sender());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		QString t = NormalizeTabTitle(title);
		setTabText(index, t);
		setTabToolTip(index, title);

		HistoryManager* historyManager = BrowserApplication::GetHistoryManager();
		historyManager->SetHistoryEntryTitle(webView->url().toString(), title);
	}
}

void TabWidget::WebViewUrlChanged(const QUrl& url)
{
	WebView* webView = qobject_cast<WebView*>(sender());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		tabBar->setTabData(index, url);
		HistoryManager* historyManager = BrowserApplication::GetHistoryManager();
		if (url.isValid() && url.toString() != "about:blank")
			historyManager->AddHistoryEntry(url.toString(), webView->title(), webView->icon());
	}
	//emit TabsChanged();
}

void TabWidget::WebPageMutedOrAudibleChanged()
{
	WebPage* webPage = qobject_cast<WebPage*>(sender());
	WebView* webView = qobject_cast<WebView*>(webPage->view());
	int index = GetWebViewIndex(webView);
	if (index != -1)
	{
		QString title = NormalizeTabTitle(webView->title());

		bool isMuted = webPage->isAudioMuted();
		bool isAudible = webPage->recentlyAudible();
		if (isMuted)
			title += tr(" (muted)");
		else if (isAudible)
			title += tr(" (audible)");

		setTabText(index, title);
	}
}

void TabWidget::WebPageDownloadRequested(QWebEngineDownloadItem *downloadItem)
{
	BrowserApplication::GetDownloadManager()->download(downloadItem);
	downloadItem->accept();
}

void TabWidget::LineEditReturnPressed()
{
	if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender()))
	{
		if (lineEdits->currentWidget() == lineEdit->parent())
		{
			emit LoadPage(GetCurrentWebView(), lineEdit->text());
			GetCurrentWebView()->setFocus();
		}
	}
}

void TabWidget::WindowCloseRequested()
{
	WebPage* webPage = qobject_cast<WebPage*>(sender());
	WebView* webView = qobject_cast<WebView*>(webPage->view());
	int index = GetWebViewIndex(webView);
	if (index >= 0)
		CloseTab(index);
}

void TabWidget::MoveTab(int fromIndex, int toIndex)
{
	QWidget* urlLineEdit = lineEdits->widget(fromIndex);
	lineEdits->removeWidget(urlLineEdit);
	lineEdits->insertWidget(toIndex, urlLineEdit);
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

QString TabWidget::NormalizeTabTitle(const QString& title)
{
	QString t = title;
	if (t.size() > MaxSymbolsInTabTitle)
	{
		t.resize(MaxSymbolsInTabTitle);
		t += QString("...");
	}
	return t;
}

WebActionMapper::WebActionMapper(QAction* _rootAction, QWebEnginePage::WebAction _webAction, QObject* parent)
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

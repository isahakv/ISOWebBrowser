#include "browsermainwindow.h"
#include "ui_mainwindow.h"

#include "defines.h"
#include "browserapplication.h"
#include "tabwidget.h"
#include "webview.h"
#include "inspectelement.h"
#include "history.h"
#include "urllineedit.h"
#include "searchlineedit.h"
#include "settings.h"
#include "browserhelpers.h"

#include <QWebEngineProfile>
#include <QWebEngineHistory>
#include <QWebEngineHistoryItem>

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QStyle>
#include <QPixmap>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QSettings>

BrowserMainWindow::BrowserMainWindow(QWidget *parent, bool isPrivateWindow)
	: QMainWindow(parent)
	, tabWidget(new TabWidget(this, this))
	, toolbarSearch(0)
	, isPrivateBrowsing(false)
	, privateProfile(0)
	, historyBack(0)
	, historyForward(0)
	, historyDialog(0)
	, stop(0)
	, reload(0)
{
	setToolButtonStyle(Qt::ToolButtonFollowStyle);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setMinimumSize(QSize(724, 124));
	setWindowTitle(isPrivateWindow ? tr("Incognito - ")+BrowserApplication::applicationName()
								   : BrowserApplication::applicationName());
	SetupMenu();
	SetupToolBar();
	statusBar()->setSizeGripEnabled(true);

	QWidget* centralWidget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setMargin(0);

	layout->addWidget(tabWidget);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	connect(tabWidget, SIGNAL(LoadPage(WebView*,QString)), this, SLOT(LoadPage(WebView*,QString)));
	connect(tabWidget, SIGNAL(WebPageLinkHovered(QString)),
			statusBar(), SLOT(showMessage(QString)));
	connect(tabWidget, SIGNAL(CurrentTabChanged(int)),
			this, SLOT(SlotCurrentWebPageChanged()));
	connect(tabWidget, SIGNAL(WebViewLoadStarted(WebView*)),
			this, SLOT(SlotWebPageLoadStarted(WebView*)));
	connect(tabWidget, SIGNAL(WebViewLoadFinished(WebView*)),
			this, SLOT(SlotWebPageLoadFinished(WebView*)));
	connect(tabWidget, SIGNAL(LastTabClosed()),
			tabWidget, SLOT(NewTab()));

	SetPrivateBrowsing(isPrivateWindow);
	LoadDefaultState();
	tabWidget->NewTab();
}

BrowserMainWindow::~BrowserMainWindow()
{

}

WebView* BrowserMainWindow::GetCurrentTab() const
{
	return tabWidget->GetCurrentWebView();
}

QByteArray BrowserMainWindow::SaveState(int withTabs) const
{
	Q_UNUSED(withTabs)

	QString version = BrowserApplication::GetInstance()->applicationVersion();
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	stream << version;
	stream << pos();
	stream << size();

	return data;
}

bool BrowserMainWindow::RestoreState(const QByteArray& state)
{
	QString version = BrowserApplication::GetInstance()->applicationVersion();
	QByteArray sd = state;
	QDataStream stream(&sd, QIODevice::ReadOnly);
	if (stream.atEnd())
		return false;

	QString v;
	stream >> v;
	if (v != version)
		return false;

	QPoint pos;
	QSize size;
	stream >> pos;
	stream >> size;

	move(pos);
	resize(size);

	return true;
}

QString BrowserMainWindow::GetHomePage()
{
	QSettings settings(APP_NAME);
	settings.beginGroup(BROWSER_GENERAL_REGISTRY_GROUP_KEY);
	QString homePage = settings.value(BROWSER_HOME_PAGE_REGISTRY_KEY, defaultHomePage).toString();
	settings.endGroup();

	return homePage;
}

QString BrowserMainWindow::GetDefaultSearchEngine()
{
	QSettings settings(APP_NAME);
	settings.beginGroup(BROWSER_GENERAL_REGISTRY_GROUP_KEY);
	QString searchEngine = settings.value(BROWSER_SEARCH_ENGINE_REGISTRY_KEY, defaultSearchEngine).toString();
	settings.endGroup();

	return searchEngine;
}

void BrowserMainWindow::LoadPage(WebView* tab, const QString& url)
{
	QUrl _url = QUrl::fromUserInput(url);
	LoadUrl(tab, _url);
}

void BrowserMainWindow::SetPrivateBrowsing(bool newPrivateBrowsing)
{
	isPrivateBrowsing = newPrivateBrowsing;

	if (newPrivateBrowsing)
	{
		if (!privateProfile)
		{
			privateProfile = new QWebEngineProfile(this);
			privateProfile->setPersistentCookiesPolicy(QWebEngineProfile::PersistentCookiesPolicy::NoPersistentCookies);
		}

		if (privateProfile == tabWidget->GetProfile())
			return;

		tabWidget->SetProfile(privateProfile);
	}
	else
	{
		if (QWebEngineProfile::defaultProfile() == tabWidget->GetProfile())
			return;

		tabWidget->SetProfile(QWebEngineProfile::defaultProfile());
	}
}

void BrowserMainWindow::closeEvent(QCloseEvent* event)
{
	if (tabWidget->count() > 1)
	{
		int ret = QMessageBox::warning(this, QString(),
									   tr("Are you sure you want to close the window?"
										  "\nThere are %1 tabs open!").arg(tabWidget->count()),
										QMessageBox::Yes | QMessageBox::No,
										QMessageBox::No);
		if (ret == QMessageBox::No)
		{
			event->ignore();
			return;
		}
	}

	SaveDefaultState();
	event->accept();
	deleteLater();
}

void BrowserMainWindow::SlotWebPageLoadStarted(WebView* webView)
{
	if (!webView || webView != GetCurrentTab())
		return;

	ActivatePageStopAction();
}

void BrowserMainWindow::SlotWebPageLoadFinished(WebView* webView)
{
	if (!webView || webView != GetCurrentTab())
		return;

	ActivatePageReloadAction();
}

void BrowserMainWindow::SlotCurrentWebPageChanged()
{
	if (!GetCurrentTab())
		return;

	bool isWebPageLoading = GetCurrentTab()->IsWebPageLoading();
	if (isWebPageLoading)
		ActivatePageStopAction();
	else
		ActivatePageReloadAction();
}

void BrowserMainWindow::SlotUpadateStatusBarText(const QString& text)
{
	statusBar()->showMessage(text, 2000);
}

// If tab == NULL, Then Load on Current Tab
void BrowserMainWindow::LoadUrl(WebView* tab, const QUrl& url)
{
	if (!tab)
		tab = GetCurrentTab();

	if (!tab || !url.isValid())
		return;

	tab->LoadUrl(url);
	tab->setFocus();
}

void BrowserMainWindow::SlotFileNew()
{
	BrowserApplication::GetInstance()->newMainWindow();
}

void BrowserMainWindow::SlotFileOpen()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
				tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));

	if (file.isEmpty())
		return;

	LoadPage(GetCurrentTab(), file);
}

void BrowserMainWindow::SlotFileSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Web Resource"), QString(),
			tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));

	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
		return;

	QString text("<html><head></head><body></body></html>");

	QDataStream out(&file);
	//while (!out.atEnd())
	{
		out << text;
	}

	file.flush();
	file.close();
}

// Change Name
void BrowserMainWindow::SlotNewPrivateWindow()
{
	BrowserApplication::GetInstance()->newMainWindow(true);
}

void BrowserMainWindow::SlotEditFind()
{
	if (!GetCurrentTab())
		return;

	bool ok;
	QString search = QInputDialog::getText(this, tr("Find"), tr("Text:"),
										   QLineEdit::Normal, lastSearch, &ok);
	if (ok && !search.isEmpty())
	{
		lastSearch = search;
		GetCurrentTab()->findText(lastSearch, 0, BrowserHelpers::Invoke<bool,BrowserMainWindow>(this, &BrowserMainWindow::HandleFindTextResult));
	}
}

void BrowserMainWindow::SlotEditFindNext()
{
	if (!GetCurrentTab() || lastSearch.isEmpty())
		return;

	GetCurrentTab()->findText(lastSearch);
}

void BrowserMainWindow::SlotEditFindPrevious()
{
	if (!GetCurrentTab() || lastSearch.isEmpty())
		return;

	GetCurrentTab()->findText(lastSearch, QWebEnginePage::FindBackward);
}

void BrowserMainWindow::SlotPreferences()
{
	SettingsDialog* settingsDialog = new SettingsDialog(this);
	settingsDialog->show();
}

void BrowserMainWindow::SlotViewZoomIn()
{
	if (!GetCurrentTab())
		return;

	GetCurrentTab()->setZoomFactor(GetCurrentTab()->zoomFactor() + 0.1f);
}

void BrowserMainWindow::SlotViewZoomOut()
{
	if (!GetCurrentTab())
		return;

	GetCurrentTab()->setZoomFactor(GetCurrentTab()->zoomFactor() - 0.1f);
}

void BrowserMainWindow::SlotViewResetZoom()
{
	if (!GetCurrentTab())
		return;

	GetCurrentTab()->setZoomFactor(1.0f);
}

void BrowserMainWindow::SlotViewPageSource()
{
	if (!GetCurrentTab())
		return;

	QPlainTextEdit* textEdit = new QPlainTextEdit;
	textEdit->setWindowTitle(tr("Page Source of %1").arg(GetCurrentTab()->title()));
	textEdit->setMinimumWidth(640);
	textEdit->setAttribute(Qt::WA_DeleteOnClose);
	textEdit->show();

	GetCurrentTab()->page()->toHtml(BrowserHelpers::Invoke(textEdit, &QPlainTextEdit::setPlainText));
}

void BrowserMainWindow::SlotViewToggleInspectElement(bool show)
{
	GetCurrentTab()->GetInspectElement()->setHidden(!show);
}

void BrowserMainWindow::SlotToggleHistoryDialog(bool show)
{
	if (show && !historyDialog)
	{
		historyDialog = new HistoryDialog(BrowserApplication::GetHistoryManager(), this);
		historyDialog->show();
		connect(historyDialog, SIGNAL(destroyed(QObject*)), this, SLOT(SlotHistoryDialogDestroyed()));
	}
	else if (!show && historyDialog)
	{
		historyDialog->deleteLater();
		historyDialog = 0;
	}
}

void BrowserMainWindow::SlotHistoryDialogDestroyed()
{
	toggleHistory->setChecked(false);
}

void BrowserMainWindow::SlotAboutApplication()
{
	QMessageBox about;
	QPixmap pic(":/Images/Creators/isahak.jpg");
	pic = pic.scaled(QSize(200, 200), Qt::KeepAspectRatio);
	about.setIconPixmap(pic);
	about.setText(tr("Version %1"
					 "<p>I'm Isahak Vardanyan, I'm born in 14.01.1999"
					 "<p>My profession is Software Enginneer and Game Developer"
					 "<p>This is my Personal \"Yet\" Not Commercial Browser!"
//					 "<p>Qt WebEngine is based on the Chromium open source project "
					 "If you have any questions and suggestion feel free to Contact me."
					 "<p>Contact: isahakv@gmail.com"
					 ).arg(QCoreApplication::applicationVersion()));

	about.show();
	about.exec();
}

void BrowserMainWindow::SlotHome()
{
	if (!GetCurrentTab())
		return;

	tabWidget->LoadHomePage(GetCurrentTab());
}

void BrowserMainWindow::LoadUrlInCurrentTab(const QUrl& url)
{
	LoadUrl(GetCurrentTab(), url);
}

// Fix this
void BrowserMainWindow::SlotAboutToShowBackMenu()
{
	historyBackMenu->clear();
	if (!GetCurrentTab())
		return;

	QWebEngineHistory* history = GetCurrentTab()->history();
	int historyCount = history->count();
	QList<QWebEngineHistoryItem> backItems = history->backItems(historyCount);
	for (int i = backItems.count() - 1; i >= 0; i--)
	{
		QWebEngineHistoryItem backItem = backItems[i];
		QAction* action = new QAction(this);
		action->setData(-1*(historyCount-i-1));

		//QIcon icon = BrowserApplication::GetInstance()->GetIconByUrl(backItem.url());
		//action->setIcon(icon);
		action->setText(backItem.title());
		historyBackMenu->addAction(action);
	}
}
// Fix this
void BrowserMainWindow::SlotAboutToShowForwardMenu()
{
	historyForwardMenu->clear();
	if (!GetCurrentTab())
		return;

	QWebEngineHistory* history = GetCurrentTab()->history();
	int historyCount = history->count();
	QList<QWebEngineHistoryItem> forwardItems = history->forwardItems(historyCount);
	for (int i = 0; i < forwardItems.count(); i++)
	{
		QWebEngineHistoryItem forwardItem = forwardItems[i];
		QAction* action = new QAction(this);
		action->setData(historyCount-i);

		//QIcon icon = BrowserApplication::GetInstance()->GetIconByUrl(forwardItem.url());
		//action->setIcon(icon);
		action->setText(forwardItem.title());
		historyForwardMenu->addAction(action);
	}
}
// Fix this
void BrowserMainWindow::SlotOpenActionUrl(QAction* action)
{
	int offset = action->data().toInt();
	QWebEngineHistory* history = GetCurrentTab()->history();
	if (offset < 0)
		history->goToItem(history->backItems(-1*offset).first());
	else if (offset > 0)
		history->goToItem(history->forwardItems(history->count() - offset + 1).back());
}

void BrowserMainWindow::UpdateToggleInspectElementState()
{
	if (!GetCurrentTab())
		return;

	toggleInspectElement->setChecked(!(GetCurrentTab()->GetInspectElement()->isHidden()));
}

void BrowserMainWindow::LoadDefaultState()
{
	QSettings settings(APP_NAME);
	settings.beginGroup(BROWSER_MAIN_WINDOW_REGISTRY_GROUP_KEY);
	QByteArray data = settings.value(BROWSER_DEFAULT_STATE_REGISTRY_KEY).toByteArray();
	RestoreState(data);
	settings.endGroup();
}

void BrowserMainWindow::SaveDefaultState()
{
	QSettings settings(APP_NAME);
	settings.beginGroup(BROWSER_MAIN_WINDOW_REGISTRY_GROUP_KEY);
	QByteArray data = SaveState();
	settings.setValue(BROWSER_DEFAULT_STATE_REGISTRY_KEY, data);
	settings.endGroup();
}

void BrowserMainWindow::SetupMenu()
{
	// File
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(tr("&New Window"), this, SLOT(SlotFileNew()), QKeySequence::New);
	fileMenu->addAction(tr("&Open File..."), this, SLOT(SlotFileOpen()), QKeySequence::Open);
	fileMenu->addAction(tr("&Save As..."), this, SLOT(SlotFileSaveAs()), QKeySequence::SaveAs);
	fileMenu->addAction(tr("New Private &Browsing..."), this, SLOT(SlotNewPrivateWindow()),
						QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N));

	fileMenu->addSeparator();
	fileMenu->addAction(tr("&Quit"), this, SLOT(close()), QKeySequence::Close);

	// Edit
	QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
	QAction* undo = editMenu->addAction(tr("&Undo"));
	undo->setShortcuts(QKeySequence::Undo);
	tabWidget->AddWebAction(undo, QWebEnginePage::Undo);

	QAction* redo = editMenu->addAction(tr("&Redo"));
	redo->setShortcuts(QKeySequence::Redo);
	tabWidget->AddWebAction(redo, QWebEnginePage::Redo);

	editMenu->addSeparator();

	QAction* cut = editMenu->addAction(tr("Cu&t"));
	cut->setShortcuts(QKeySequence::Cut);
	tabWidget->AddWebAction(cut, QWebEnginePage::Cut);

	QAction* copy = editMenu->addAction(tr("&Copy"));
	copy->setShortcuts(QKeySequence::Copy);
	tabWidget->AddWebAction(copy, QWebEnginePage::Copy);

	QAction* paste = editMenu->addAction(tr("&Paste"));
	paste->setShortcuts(QKeySequence::Paste);
	tabWidget->AddWebAction(paste, QWebEnginePage::Paste);

	editMenu->addSeparator();

	QAction* find = editMenu->addAction(tr("&Find"));
	find->setShortcuts(QKeySequence::Find);
	connect(find, SIGNAL(triggered(bool)), this, SLOT(SlotEditFind()));

	QAction* findNext = editMenu->addAction(tr("&Find Next"));
	findNext->setShortcuts(QKeySequence::FindNext);
	connect(findNext, SIGNAL(triggered(bool)), this, SLOT(SlotEditFindNext()));

	QAction* findPrevious = editMenu->addAction(tr("&Find Previous"));
	findPrevious->setShortcuts(QKeySequence::FindPrevious);
	connect(findPrevious, SIGNAL(triggered(bool)), this, SLOT(SlotEditFindPrevious()));

	editMenu->addSeparator();
	editMenu->addAction(tr("&Preferences"), this, SLOT(SlotPreferences()), tr("Ctrl+,"));

	// View
	QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

	stop = viewMenu->addAction(tr("&Stop"));
	tabWidget->AddWebAction(stop, QWebEnginePage::Stop);

	reload = viewMenu->addAction(tr("&Reload"));
	tabWidget->AddWebAction(reload, QWebEnginePage::Reload);

	viewMenu->addSeparator();

	viewMenu->addAction(tr("Zoom &In"), this, SLOT(SlotViewZoomIn()), QKeySequence(Qt::CTRL | Qt::Key_Plus));
	viewMenu->addAction(tr("Zoom &Out"), this, SLOT(SlotViewZoomOut()), QKeySequence(Qt::CTRL | Qt::Key_Minus));
	viewMenu->addAction(tr("Reset &Zoom"), this, SLOT(SlotViewResetZoom()), QKeySequence(Qt::CTRL | Qt::Key_0));

	viewMenu->addSeparator();

	viewMenu->addAction(tr("Page Source"), this, SLOT(SlotViewPageSource()), tr("Ctrl+Alt+U"));
	toggleInspectElement = new QAction(tr("Inspect Element"), this);
	toggleInspectElement->setCheckable(true);
	toggleInspectElement->setChecked(false);
	toggleInspectElement->setShortcut(QKeySequence(Qt::Key_F12));
	connect(toggleInspectElement, SIGNAL(toggled(bool)), this, SLOT(SlotViewToggleInspectElement(bool)));
	connect(tabWidget, SIGNAL(CurrentTabChanged(int)), this, SLOT(UpdateToggleInspectElementState()));
	viewMenu->addAction(toggleInspectElement);

	// History
	QMenu* historyMenu = menuBar()->addMenu(tr("Hi&story"));
	QList<QAction*> historyActions;

	historyBack = new QAction(tr("Back"), this);
	tabWidget->AddWebAction(historyBack, QWebEnginePage::Back);
	historyBack->setShortcut(QKeySequence::Back);
	historyBack->setIconVisibleInMenu(false);
	historyActions.append(historyBack);

	historyForward = new QAction(tr("Forward"), this);
	tabWidget->AddWebAction(historyForward, QWebEnginePage::Forward);
	historyForward->setShortcut(QKeySequence::Forward);
	historyForward->setIconVisibleInMenu(false);
	historyActions.append(historyForward);

	historyMenu->addActions(historyActions);
	historyMenu->addSeparator();
	toggleHistory = new QAction(tr("History"), this);
	toggleHistory->setCheckable(true);
	toggleHistory->setChecked(false);
	toggleHistory->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_H));
	connect(toggleHistory, SIGNAL(toggled(bool)), this, SLOT(SlotToggleHistoryDialog(bool)));
	historyMenu->addAction(toggleHistory);

	// Help
	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(tr("About Browser"), this, SLOT(SlotAboutApplication()));
}

void BrowserMainWindow::SetupToolBar()
{
	navigationBar = addToolBar(tr("Navigation"));

	historyBack->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0, this));
	historyBackMenu = new QMenu(this);
	historyBack->setMenu(historyBackMenu);
	connect(historyBackMenu, SIGNAL(aboutToShow()),
			this, SLOT(SlotAboutToShowBackMenu()));
	connect(historyBackMenu, SIGNAL(triggered(QAction*)),
			this, SLOT(SlotOpenActionUrl(QAction*)));
	navigationBar->addAction(historyBack);

	historyForward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, this));
	historyForwardMenu = new QMenu(this);
	historyForward->setMenu(historyForwardMenu);
	connect(historyForwardMenu, SIGNAL(aboutToShow()),
			this, SLOT(SlotAboutToShowForwardMenu()));
	connect(historyForwardMenu, SIGNAL(triggered(QAction*)),
			this, SLOT(SlotOpenActionUrl(QAction*)));
	navigationBar->addAction(historyForward);

	stopReload = new QAction(this);
	reloadIcon = style()->standardIcon(QStyle::SP_BrowserReload);
	stopIcon = style()->standardIcon(QStyle::SP_BrowserStop);
	stopReload->setIcon(reloadIcon);
	navigationBar->addAction(stopReload);

	QAction* home = new QAction(this);
	home->setIcon(QIcon(":Images/Vector/home-button.svg"));
	navigationBar->addAction(home);
	connect(home, SIGNAL(triggered(bool)), this, SLOT(SlotHome()));


	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(4);

	navigationBar->addWidget(tabWidget->GetLineEditStack());
	tabWidget->GetLineEditStack()->setSizePolicy(sizePolicy);

	toolbarSearch = new SearchLineEdit(navigationBar, true);
	toolbarSearch->SetOwnerBrowserMainWindow(this);
	navigationBar->addWidget(toolbarSearch);

	sizePolicy.setHorizontalStretch(1);
	toolbarSearch->setSizePolicy(sizePolicy);
	connect(toolbarSearch, SIGNAL(Search(QUrl)), this, SLOT(LoadUrlInCurrentTab(QUrl)));
}

void BrowserMainWindow::HandleFindTextResult(bool isFound)
{
	if (!isFound)
		SlotUpadateStatusBarText(tr("\"%1\" not Found").arg(lastSearch));
}

void BrowserMainWindow::ActivatePageStopAction()
{
	disconnect(stopReload, SIGNAL(triggered(bool)), reload, SLOT(trigger()));
	stopReload->setIcon(stopIcon);
	connect(stopReload, SIGNAL(triggered(bool)), stop, SLOT(trigger()));
	stopReload->setToolTip("Stop loading the current page");
}

void BrowserMainWindow::ActivatePageReloadAction()
{
	disconnect(stopReload, SIGNAL(triggered(bool)), stop, SLOT(trigger()));
	stopReload->setIcon(reloadIcon);
	connect(stopReload, SIGNAL(triggered(bool)), reload, SLOT(trigger()));
	stopReload->setToolTip("Reload the current page");
}

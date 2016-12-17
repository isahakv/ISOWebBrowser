#include "browsermainwindow.h"

#include "browserapplication.h"
#include "tabwidget.h"
#include "webview.h"

#include <QWebEngineHistory>
#include <QWebEngineHistoryItem>

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QStyle>
#include <QInputDialog>

template<typename Arg, typename R>
struct InvokeWrapper
{
	R* receiver;
	void (R::*memberFunc)(Arg);
	void operator()(Arg result)
	{
		(receiver->*memberFunc)(result);
	}
};

template<typename Arg, typename R>
InvokeWrapper<Arg, R> Invoke(R* receiver, void (R::*memberFunc)(Arg))
{
	InvokeWrapper<Arg, R> wrapper = { receiver, memberFunc };
	return wrapper;
}

const char* BrowserMainWindow::defaultHomePage = "http://google.com/";

BrowserMainWindow::BrowserMainWindow(QWidget *parent)
	: QMainWindow(parent)
	, tabWidget(new TabWidget(this))
	, historyBack(0)
	, historyForward(0)
	, stop(0)
	, reload(0)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	//
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

	connect(tabWidget, SIGNAL(LoadPage(QString)), this, SLOT(LoadPage(QString)));
	connect(tabWidget, SIGNAL(WebPageLinkHovered(QString)),
			statusBar(), SLOT(showMessage(QString)));
	connect(tabWidget, SIGNAL(WebViewLoadStarted()),
			this, SLOT(SlotWebPageLoadStarted()));
	connect(tabWidget, SIGNAL(WebViewLoadFinished(bool)),
			this, SLOT(SlotWebPageLoadFinished(bool)));

	tabWidget->NewTab(true, true);
}

BrowserMainWindow::~BrowserMainWindow()
{

}

WebView* BrowserMainWindow::GetCurrentTab() const
{
	return tabWidget->GetCurrentWebView();
}

void BrowserMainWindow::LoadPage(const QString& url)
{
	QUrl _url = QUrl::fromUserInput(url);
	LoadUrl(_url);
}

void BrowserMainWindow::SlotWebPageLoadStarted()
{
	disconnect(stopReload, SIGNAL(triggered(bool)), reload, SLOT(trigger()));
	stopReload->setIcon(stopIcon);
	connect(stopReload, SIGNAL(triggered(bool)), stop, SLOT(trigger()));
	stopReload->setToolTip("Stop loading the current page");
}

void BrowserMainWindow::SlotWebPageLoadFinished(bool b)
{
	disconnect(stopReload, SIGNAL(triggered(bool)), stop, SLOT(trigger()));
	stopReload->setIcon(reloadIcon);
	connect(stopReload, SIGNAL(triggered(bool)), reload, SLOT(trigger()));
	stopReload->setToolTip("Reload the current page");
}

void BrowserMainWindow::SlotUpadateStatusBarText(const QString& text)
{
	statusBar()->showMessage(text, 2000);
}

void BrowserMainWindow::LoadUrl(const QUrl& url)
{
	if (!GetCurrentTab() || !url.isValid())
		return;

	// tabWidget->GetCurrentLineEdit()->setText(QString::fromUtf8(url.toEncoded())); // maybe move this in tabwidget...
	tabWidget->LoadUrlInCurrentTab(url);
}

void BrowserMainWindow::SlotFileNew()
{
	BrowserApplication::GetInstance()->newMainWindow();
	//if (mw)
	//	mw->SlotLoadHomePage();
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
		GetCurrentTab()->findText(lastSearch, 0, Invoke(this, &BrowserMainWindow::HandleFindTextResult));
	}
}

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

		QIcon icon = BrowserApplication::GetInstance()->GetIconByUrl(backItem.url());
		action->setIcon(icon);
		action->setText(backItem.title());
		historyBackMenu->addAction(action);
	}
}

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

		QIcon icon = BrowserApplication::GetInstance()->GetIconByUrl(forwardItem.url());
		action->setIcon(icon);
		action->setText(forwardItem.title());
		historyForwardMenu->addAction(action);
	}
}

void BrowserMainWindow::SlotOpenActionUrl(QAction* action)
{
	int offset = action->data().toInt();
	QWebEngineHistory* history = GetCurrentTab()->history();
	if (offset < 0)
		history->goToItem(history->backItems(-1*offset).first());
	else if (offset > 0)
		history->goToItem(history->forwardItems(history->count() - offset + 1).back());
}

void BrowserMainWindow::SetupMenu()
{
	// File
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(tr("&New Window"), this, SLOT(SlotFileNew()), QKeySequence::New);
	fileMenu->addAction(tr("&Quit"), this, SLOT(close()), QKeySequence::Close);

	// Edit
	QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
	QAction* undo = editMenu->addAction(tr("&Undo"));
	undo->setShortcuts(QKeySequence::Undo);
	QAction* redo = editMenu->addAction(tr("&Redo"));
	redo->setShortcuts(QKeySequence::Redo);

	QAction* find = editMenu->addAction(tr("&Find"));
	find->setShortcuts(QKeySequence::Find);
	connect(find, SIGNAL(triggered(bool)), this, SLOT(SlotEditFind()));

	// View
	QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

	stop = viewMenu->addAction(tr("&Stop"));

	reload = viewMenu->addAction(tr("&Reload"));

	// History
	QMenu* historyMenu = menuBar()->addMenu(tr("Hi&story"));
	QList<QAction*> historyActions;

	historyBack = new QAction(tr("Back"), this);
	historyBack->setShortcut(QKeySequence::Back);
	historyBack->setIconVisibleInMenu(false);
	historyActions.append(historyBack);

	historyForward = new QAction(tr("Forward"), this);
	historyForward->setShortcut(QKeySequence::Forward);
	historyForward->setIconVisibleInMenu(false);
	historyActions.append(historyForward);

	historyMenu->addActions(historyActions);
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

	navigationBar->addWidget(tabWidget->GetLineEditStack());
}

void BrowserMainWindow::HandleFindTextResult(bool isFound)
{
	if (!isFound)
		SlotUpadateStatusBarText(tr("\"%1\" not Found").arg(lastSearch));
}

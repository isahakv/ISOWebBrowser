#include "browsermainwindow.h"

#include "browserapplication.h"
#include "tabwidget.h"
#include "webview.h"

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QStyle>

const char* BrowserMainWindow::defaultHomePage = "http://google.com/";

BrowserMainWindow::BrowserMainWindow(QWidget *parent)
	: QMainWindow(parent),
	  tabWidget(new TabWidget(this))
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

void BrowserMainWindow::LoadUrl(const QUrl& url)
{
	if (!GetCurrentTab() || !url.isValid())
		return;

	// tabWidget->GetCurrentLineEdit()->setText(QString::fromUtf8(url.toEncoded())); // maybe move this in tabwidget...
	tabWidget->LoadUrlInCurrentTab(url);
}

void BrowserMainWindow::SlotFileNew()
{
	BrowserMainWindow* mw = BrowserApplication::GetInstance()->newMainWindow();
	//if (mw)
	//	mw->SlotLoadHomePage();
}

void BrowserMainWindow::SetupMenu()
{
	// File
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(tr("&New Window"), this, SLOT(SlotFileNew()), QKeySequence::New);
	fileMenu->addAction(tr("&Quit"), this, SLOT(close()), QKeySequence::Close);

	// Edit
	QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));

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
	navigationBar->addAction(historyBack);

	historyForward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, this));
	historyForwardMenu = new QMenu(this);
	historyForward->setMenu(historyForwardMenu);
	navigationBar->addAction(historyForward);

	navigationBar->addWidget(tabWidget->GetLineEditStack());
}

#include "browsermainwindow.h"

#include "tabwidget.h"

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QStyle>


BrowserMainWindow::BrowserMainWindow(QWidget *parent)
	: QMainWindow(parent),
	  tabWidget(new TabWidget(this))
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	//
	SetupMenu();
	SetupToolBar();

	QWidget* centralWidget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setMargin(0);

	layout->addWidget(tabWidget);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	connect(tabWidget, SIGNAL(LoadPage(QString)), this, SLOT(LoadPage(QString)));

	tabWidget->NewTab();
	tabWidget->NewTab();
	tabWidget->NewTab();
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

	tabWidget->GetCurrentLineEdit()->setText(QString::fromUtf8(url.toEncoded()));
	tabWidget->LoadUrlInCurrentTab(url);
}

void BrowserMainWindow::SetupMenu()
{
	// File
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
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

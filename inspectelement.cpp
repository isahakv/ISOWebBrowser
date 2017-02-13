#include "inspectelement.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPlainTextEdit>
#include <QPushButton>

#include "browserhelpers.h"
#include "webviewwrapper.h"
#include "webview.h"

WebPageSource::WebPageSource(QWidget *parent, WebView* _webView)
	: QWidget(parent)
	, pageSource(new QPlainTextEdit(this))
	, refreshButton(new QPushButton(this))
	, webView(_webView)
{
	refreshButton->setText(tr("Refresh Source"));
	connect(refreshButton, SIGNAL(clicked()), this, SLOT(SlotRefresh()));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(pageSource);
	layout->addWidget(refreshButton);

	setLayout(layout);
}

void WebPageSource::SlotRefresh()
{
	if (!webView->GetWebPage())
		return;

	webView->GetWebPage()->toHtml(BrowserHelpers::Invoke(pageSource, &QPlainTextEdit::setPlainText));
}

JavaScriptConsole::JavaScriptConsole(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	listView = new QListWidget(this);
	layout->addWidget(listView);
	setLayout(layout);

	errorMessageColor = QColor(255, 0, 0);
	warningMessageColor = QColor(255, 255, 0);
	infoMessageColor = QColor(135, 206, 250);
}

void JavaScriptConsole::AddMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message,
								int lineNumber, const QString &sourceID)
{
	QListWidgetItem* listWidgetItem = new QListWidgetItem(listView);
	listWidgetItem->setText(tr("Line ") + QString::number(lineNumber) + tr("| ") + message + tr(" | Script ") + sourceID.toUtf8());

	if (level == QWebEnginePage::JavaScriptConsoleMessageLevel::ErrorMessageLevel)
		listWidgetItem->setBackgroundColor(errorMessageColor);
	else if (level == QWebEnginePage::JavaScriptConsoleMessageLevel::WarningMessageLevel)
		listWidgetItem->setBackgroundColor(warningMessageColor);
	else if (level == QWebEnginePage::JavaScriptConsoleMessageLevel::InfoMessageLevel)
		listWidgetItem->setBackgroundColor(infoMessageColor);

	listView->addItem(listWidgetItem);
}


InspectElement::InspectElement(QWidget *parent, WebViewWrapper* _ownerWebViewWrapper)
	: QTabWidget(parent)
	, ownerWebViewWrapper(_ownerWebViewWrapper)
	, javaScriptConsole(new JavaScriptConsole(this))
	, pageSource(new WebPageSource(this, GetWebWiew()))
{
	QToolButton* closeButton = new QToolButton(this);
	setCornerWidget(closeButton, Qt::TopRightCorner);
	closeButton->setAutoRaise(true);
	closeButton->setIcon(QIcon(":/Images/Vector/close.svg"));
	closeButton->setToolTip(tr("Close"));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));

	addTab(javaScriptConsole, tr("Console"));
	addTab(pageSource, tr("Page Source"));

	QSizePolicy sp = sizePolicy();
	sp.setHorizontalPolicy(QSizePolicy::Preferred);
	//sp.setVerticalStretch(0);
	setSizePolicy(sp);
}

WebView* InspectElement::GetWebWiew() const
{
	return ownerWebViewWrapper->GetWebView();
}

void InspectElement::showEvent(QShowEvent* event)
{
	Q_UNUSED(event)

	pageSource->SlotRefresh();
}

void InspectElement::AddJavaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
								 const QString &message,int lineNumber, const QString &sourceID)
{
	javaScriptConsole->AddMessage(level, message, lineNumber, sourceID);
}

#include "webview.h"

#include <QWebEngineProfile>

#include <QListWidget>
#include <QListWidgetItem>

#include "browserapplication.h"
#include "browsermainwindow.h"

WebPage::WebPage(QWebEngineProfile* profile, QObject* parent)
	: QWebEnginePage(profile, parent)
{
	testWindow = new QMainWindow(0);
	QListWidget* listView = new QListWidget(testWindow);
	testWindow->setCentralWidget(listView);
	testWindow->show();
}

void WebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message,
							  int lineNumber, const QString &sourceID)
{
	QListWidget* listView = qobject_cast<QListWidget*>(testWindow->centralWidget());
	if (listView)
	{
		QListWidgetItem* listWidgetItem = new QListWidgetItem(listView);
		listWidgetItem->setText(tr("Line ") + QString::number(lineNumber) + tr("| ") + message + tr(" | Script ") + sourceID.toUtf8());

		if (level == JavaScriptConsoleMessageLevel::ErrorMessageLevel)
			listWidgetItem->setBackgroundColor(QColor(255, 0, 0));
		else if (level == JavaScriptConsoleMessageLevel::WarningMessageLevel)
			listWidgetItem->setBackgroundColor(QColor(255, 255, 0));
		else if (level == JavaScriptConsoleMessageLevel::InfoMessageLevel)
			listWidgetItem->setBackgroundColor(QColor(135, 206, 250));

		listView->addItem(listWidgetItem);
	}
}

WebView::WebView(QWidget* parent)
	: QWebEngineView(parent)
{
	isWebPageLoading = false;

	connect(this, SIGNAL(loadStarted()), this, SLOT(SlotLoadStarted()));
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(SlotLoadFinished()));
}

void WebView::LoadUrl(const QUrl& url)
{
	initialUrl = url;
	load(url);
}

QUrl WebView::GetUrl() const
{
	QUrl url = QWebEngineView::url();
	if (!url.isEmpty())
		return url;

	return initialUrl;
}

void WebView::SlotLoadStarted()
{
	isWebPageLoading = true;
}

void WebView::SlotLoadFinished()
{
	isWebPageLoading = false;
}

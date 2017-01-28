#include "inspectelement.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

#include "webviewwrapper.h"

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
	, javaScriptConsole(new JavaScriptConsole(this))
	, ownerWebViewWrapper(_ownerWebViewWrapper)
{
	addTab(javaScriptConsole, tr("Console"));

	QSizePolicy sp = sizePolicy();
	sp.setHorizontalPolicy(QSizePolicy::Preferred);
	sp.setHorizontalPolicy(QSizePolicy::Preferred);
	sp.setVerticalStretch(0);
	setSizePolicy(sp);
}

void InspectElement::AddJavaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
								 const QString &message,int lineNumber, const QString &sourceID)
{
	javaScriptConsole->AddMessage(level, message, lineNumber, sourceID);
}

/*QSize InspectElement::sizeHint() const
{
	return QSize(100, 100);
}*/

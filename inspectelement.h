#ifndef INSPECTELEMENT_H
#define INSPECTELEMENT_H

#include <QTabWidget>

#include "webview.h"

class QListWidget;
class WebViewWrapper;

class JavaScriptConsole : public QWidget
{
	Q_OBJECT
public:
	JavaScriptConsole(QWidget *parent = 0);

	inline QListWidget* GetListView() const { return listView; }

public slots:
	void AddMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message,
			   int lineNumber, const QString &sourceID);

protected:
	QListWidget* listView;

	QColor errorMessageColor;
	QColor warningMessageColor;
	QColor infoMessageColor;
};

class InspectElement : public QTabWidget
{
	Q_OBJECT	
public:
	explicit InspectElement(QWidget *parent = 0, WebViewWrapper* _ownerWebViewWrapper = 0);
	//QSize sizeHint() const;
	inline WebViewWrapper* GetOwnerWebViewWrapper() { return ownerWebViewWrapper; }
	inline JavaScriptConsole* GetJavaScriptConsole() const { return javaScriptConsole; }

	void AddJavaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
									 const QString &message,int lineNumber, const QString &sourceID);

protected:
	WebViewWrapper* ownerWebViewWrapper;
	JavaScriptConsole* javaScriptConsole;
};

#endif // INSPECTELEMENT_H

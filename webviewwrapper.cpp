#include "webviewwrapper.h"

#include <QSplitter>
#include <QVBoxLayout>

#include "webview.h"
#include "inspectelement.h"

WebViewWrapper::WebViewWrapper(QWidget *parent)
	: QWidget(parent)
	, webView(new WebView(this, this))
	, inspectElement(new InspectElement(this, this))
{
	inspectElement->setHidden(true);

	QVBoxLayout* layout = new QVBoxLayout(this);
	QSplitter* splitter = new QSplitter(this);
	layout->addWidget(splitter);
	setLayout(layout);
	splitter->setOrientation(Qt::Vertical);

	splitter->addWidget(webView);
	splitter->addWidget(inspectElement);
}

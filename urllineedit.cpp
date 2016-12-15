#include "urllineedit.h"

#include "webview.h"

#include <QtGui/QFocusEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>

UrlLineEdit::UrlLineEdit(QWidget *parent)
	: QWidget(parent)
	, lineEdit(new QLineEdit(this))
	, webView(0)
{
	setFocusPolicy(lineEdit->focusPolicy());
	setAttribute(Qt::WA_InputMethodEnabled);
	setSizePolicy(lineEdit->sizePolicy());

	// line edit
	lineEdit->setFocusProxy(this);
	//lineEdit->setFocusPolicy(Qt::FocusPolicy::Fo);
	//lineEdit->setFocus();

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(lineEdit);

	QPushButton* button = new QPushButton(this);
	layout->addWidget(button);
	setLayout(layout);
}

void UrlLineEdit::SetWebView(WebView* _webView)
{
	Q_ASSERT(_webView);
	webView = _webView;
	connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(WebViewUrlChanged(QUrl)));
	connect(webView, SIGNAL(iconChanged(QIcon)), this, SLOT(WebViewIconChanged(QIcon)));
	connect(webView, SIGNAL(loadProgress(int)), this, SLOT(update()));
}

QSize UrlLineEdit::SizeHint() const
{
	lineEdit->setFrame(true);
	QSize size = lineEdit->sizeHint();
	lineEdit->setFrame(false);
	return size;
}

void UrlLineEdit::focusInEvent(QFocusEvent* event)
{
	lineEdit->event(event);
	QWidget::focusInEvent(event);
}

void UrlLineEdit::focusOutEvent(QFocusEvent* event)
{
	lineEdit->event(event);

	if (lineEdit->completer())
	{
		qWarning("Fuck yea");
	}

	QWidget::focusOutEvent(event);
}

void UrlLineEdit::keyPressEvent(QKeyEvent* event)
{
	lineEdit->event(event);
}

void UrlLineEdit::WebViewUrlChanged(const QUrl& url)
{
	lineEdit->setText(QString::fromUtf8(url.toEncoded()));
}

void UrlLineEdit::WebViewIconChanged(const QIcon& icon)
{

}

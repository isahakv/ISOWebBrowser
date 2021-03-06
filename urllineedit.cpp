#include "urllineedit.h"

#include "webview.h"
#include "browsermainwindow.h"

#include <QApplication>
#include <QtCore/QEvent>

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QFocusEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOptionFrame>
#include <QLabel>
#include <QStringListModel>
#include <QSettings>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

#include <QtSvg>

SearchButton::SearchButton(QWidget *parent, int _size)
	: QAbstractButton(parent)
	, menu(0)
	, size(_size)
{
#ifndef QT_NO_CURSOR
	setCursor(Qt::ArrowCursor);
#endif // QT_NO_CURSOR
	setToolTip(tr("Recent Searches"));
	setFocusPolicy(Qt::NoFocus);
}

void SearchButton::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)

	/*
	QSvgRenderer svgRenderer(QString("D:/Qt5.7/Projects/ISOWebBrowser/data/search.svg"));
	//QPixmap pix(QSize(16, 16));
	//QPainter pixPainter( &pix );
	//svgRenderer.load(QString(":Images/Vector/search.svg"));
	//svgRenderer.render( &pixPainter );

	QImage image(16, 16, QImage::Format_ARGB32);
	QPainter painter(&image);
	svgRenderer.render(&painter);*/

	//generator.setFileName(":Images/Vector/search.svg");
	//generator.setSize(QSize(size, size));
	//generator.setViewBox(QRect(0, 0, size, size));

	QPainter painter(this);
	//painter.begin(&generator);
	//painter.end();

	QPixmap pixmap(":Images/Vector/search.svg");

	painter.drawPixmap(0, 0, size, size, pixmap);
}

QSize SearchButton::sizeHint() const
{
	return QSize(size, size);
}

void SearchButton::mousePressEvent(QMouseEvent* event)
{
	if (menu && event->button() == Qt::LeftButton)
	{
		QWidget* parent = parentWidget();
		if (parent)
		{
			QPoint r = parent->mapToGlobal(QPoint(0, parent->height()));
			menu->exec(QPoint(r.x(), r.y()));
		}

		event->accept();
	}

	QAbstractButton::mousePressEvent(event);
}

// ClearButton
ClearButton::ClearButton(QWidget *parent, int _size)
	: QAbstractButton(parent)
	, size(_size)
{
#ifndef QT_NO_CURSOR
	setCursor(Qt::ArrowCursor);
#endif // QT_NO_CURSOR
	setToolTip(tr("Clear"));
	setVisible(false);
	setFocusPolicy(Qt::NoFocus);
}

void ClearButton::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)

	QPainter painter(this);

	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
	painter.drawPixmap(0, 0, size, size, QPixmap(":Images/Vector/cancel-button.svg"));
}

QSize ClearButton::sizeHint() const
{
	return QSize(size, size);
}

void ClearButton::TextChanged(const QString& text)
{
	setVisible(!text.isEmpty());
}

// BrowserLineEdit
BrowserLineEdit::BrowserLineEdit(QWidget *parent)
	: QWidget(parent)
	, lineEdit(new QLineEdit(this))
	, leftWidget(0)
	, clearButton(0)
{
	heightOfLineEdit = 35;

	setFocusPolicy(lineEdit->focusPolicy());
	setAttribute(Qt::WA_InputMethodEnabled);

	// setup size policies
	QSizePolicy sizePolicy = lineEdit->sizePolicy();
	sizePolicy.setVerticalPolicy(QSizePolicy::Preferred);
	lineEdit->setSizePolicy(sizePolicy);
	setSizePolicy(sizePolicy);

	setBackgroundRole(lineEdit->backgroundRole());
	setMouseTracking(true);
	setAcceptDrops(true);
	setAttribute(Qt::WA_MacShowFocusRect, true);
	//setPalette(lineEdit->palette());

	// line edit
	lineEdit->setFrame(true);
	lineEdit->setFocusProxy(this);
	lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
	QFont font = lineEdit->font();
	font.setPointSize(10);
	lineEdit->setFont(font);
	//lineEdit->setEchoMode(QLineEdit::Normal);

	// clearButton
	clearButton = new ClearButton(this);
	connect(clearButton, SIGNAL(clicked(bool)),
			lineEdit, SLOT(clear()));
	connect(lineEdit, SIGNAL(textChanged(QString)),
			clearButton, SLOT(TextChanged(QString)));
}

QSize BrowserLineEdit::sizeHint() const
{
	//lineEdit->setFrame(true);
	QSize size = lineEdit->sizeHint();
	//lineEdit->setFrame(false);
	return QSize(size.width(), heightOfLineEdit);
}

QVariant BrowserLineEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
	return lineEdit->inputMethodQuery(property);
}

void BrowserLineEdit::focusInEvent(QFocusEvent* event)
{
	// qWarning("focusInEvent");
	lineEdit->event(event);
	//if (event->)
	{
		lineEdit->selectAll();
	}
	QWidget::focusInEvent(event);
}

void BrowserLineEdit::focusOutEvent(QFocusEvent* event)
{
	lineEdit->event(event);
	QWidget::focusOutEvent(event);
}

void BrowserLineEdit::keyPressEvent(QKeyEvent* event)
{
	lineEdit->event(event);
}

void BrowserLineEdit::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
}

void BrowserLineEdit::inputMethodEvent(QInputMethodEvent* event)
{
	lineEdit->event(event);
}

// UrlLineEdit
UrlLineEdit::UrlLineEdit(QWidget *parent)
	: BrowserLineEdit(parent)
	, webView(0)
{	
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(5);
	layout->addWidget(lineEdit);
	layout->addWidget(clearButton);

	lineEdit->setPlaceholderText(QString("Enter Url"));
}

void UrlLineEdit::SetWebView(WebView* _webView)
{
	Q_ASSERT(_webView);
	if (webView)
	{
		disconnect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(WebViewUrlChanged(QUrl)));
		disconnect(webView, SIGNAL(loadProgress(int)), this, SLOT(update()));
	}

	webView = _webView;
	connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(WebViewUrlChanged(QUrl)));
	connect(webView, SIGNAL(loadProgress(int)), this, SLOT(update()));
}

void UrlLineEdit::focusOutEvent(QFocusEvent* event)
{
	BrowserLineEdit::focusOutEvent(event);

	if (webView && lineEdit->text().isEmpty())
		lineEdit->setText(QString::fromUtf8(webView->url().toEncoded()));
}

void UrlLineEdit::WebViewUrlChanged(const QUrl& url)
{
	qWarning(url.toString().toStdString().c_str());
	lineEdit->setText(QString::fromUtf8(url.toEncoded()));
}

#include "settings.h"

#include "browsertypes.h"
#include "browsermainwindow.h"

#include <QSettings>

SettingsDialog::SettingsDialog(QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);

	LoadDefaults();
	LoadSettings();
}

void SettingsDialog::accept()
{
	QDialog::accept();
	SaveSettings();
}

void SettingsDialog::LoadDefaults()
{

}

void SettingsDialog::LoadSettings()
{
	QSettings settings("ISOBrowser");
	settings.beginGroup(QLatin1String("General"));
	homeLineEdit->setText(BrowserMainWindow::GetHomePage());
	homePageTypeComboBox->setCurrentIndex(settings.value(QLatin1String("HomePageType"), 0).toInt());
	defaultSearchEngineLineEdit->setText(BrowserMainWindow::GetDefaultSearchEngine());

	settings.endGroup();
}

void SettingsDialog::SaveSettings()
{
	QSettings settings("ISOBrowser");
	settings.beginGroup(QLatin1String("General"));
	settings.setValue(QLatin1String("HomePage"), homeLineEdit->text());
	settings.setValue(QLatin1String("HomePageType"), homePageTypeComboBox->currentIndex());
	settings.setValue(QLatin1String("SearchEngine"), defaultSearchEngineLineEdit->text());

	settings.endGroup();
}

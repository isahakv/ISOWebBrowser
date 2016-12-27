#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_settings.h"

class SettingsDialog : public QDialog, public Ui_Settings
{
	Q_OBJECT

public:
	SettingsDialog(QWidget* parent = 0);
	void accept();

private slots:
	void LoadDefaults();
	void LoadSettings();
	void SaveSettings();
};

#endif // SETTINGSDIALOG_H

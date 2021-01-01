
#include <QtWidgets>

#include "preferences.h"
#include "ui_preferences.h"
#include "clockit.h"

#if defined(_WIN32)
#include <windows.h>
#endif



Preferences::Preferences(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Preferences)
{
	ui->setupUi(this);

	connect( this, SIGNAL(accepted()), this, SLOT(accept()) );

    QSettings settings("WorkClock", "Config");
	ui->edtHostname->setText( settings.value("hostname").toString() );
}



Preferences::~Preferences()
{
	delete ui;
}



void Preferences::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}



QString Preferences::getProject()
{
    QSettings settings("WorkClock", "Config");

	QString project = settings.value("project").toString();

	if ( project.isEmpty() ) {
		project = QString(getenv("PROJECT")).toLower();
		settings.setValue("project", project );
	}

	return project;
}


QString Preferences::getUserName()
{
    QSettings settings("WorkClock", "Config");

	QString userName = settings.value("name").toString();

	if ( userName.isEmpty() ) {
#if defined(_WIN32)
		char winUserName[256];
		DWORD winUserNameSize = sizeof(winUserName);
		GetUserNameA( winUserName, &winUserNameSize );
		userName = QString::fromLocal8Bit( winUserName ).toLower();
#else
		userName = QString(getenv("USER")).toLower();
#endif
		settings.setValue("name", userName );
	}

	// qDebug() << "getenv(USER) =" << userName;
	return userName;
}


QString Preferences::getHostName()
{
    QSettings settings("WorkClock", "Config");

	return settings.value("hostname").toString();
}



void Preferences::accept()
{
	// qDebug() << "Preferences::accept()";

    QSettings settings("WorkClock", "Config");

	settings.setValue("hostname", ui->edtHostname->text() );

	glbHostname = Preferences::getHostName();
	glbHostUrl = (QString("http://") + glbHostname);

	close();
	return;
}




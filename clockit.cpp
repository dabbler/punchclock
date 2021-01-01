
#include <QtWidgets>
#include <QFile>
#include <QDebug>

#include <time.h>

#include "clockit.h"
#include "tmp/ui/ui_clockit.h"
#include "preferences.h"
#include "viewentries.h"
#include "version.h"

// #define DEBUG_OUTPUT



/**
 *  @brief This is constructor for the dialog for choosing drive letter and browsing for folder to mount
 *
 *  ClockIt appears when user click Configure in the taskbar icon menu
 */
ClockIt::ClockIt(QWidget *parent) :
        QDialog(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
        ui(new Ui::ClockIt)
{
    ui->setupUi(this);

    srand( time(0L) );

    // this->setStyleSheet(" #ClockIt { border-image: url(images/background.png); } ");
    pixmapBackground = new QPixmap;
    pixmapBackground->load(":images/background.png");

    http = new QNetworkAccessManager;
	connect( http, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));


    readDefaultSettings();

	connect( &clockedInElsewhereTimer, SIGNAL(timeout()), this, SLOT(updateClockStatus()) );

    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(okClicked()) );
    QShortcut *shortcut = new QShortcut(QKeySequence(tr("Ctrl+O")), this );
    connect( shortcut, SIGNAL(activated()), this, SLOT(okClicked()) );

    ui->dtClockInTime->setCalendarPopup(true);
    ui->dtClockOutTime->setCalendarPopup(true);
    ui->dtClockInTime->setDisplayFormat("yyyy-MM-dd hh:mm");
    ui->dtClockOutTime->setDisplayFormat("yyyy-MM-dd hh:mm");

    createActions();
    createTrayIcon();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    setIcon();
    trayIcon->show();
    trayIcon->setToolTip("Clock Status Unknown");

	queryUserName();
	queryProjects();			// get the initial Projects
    updateClockStatus();	// get the initial clock status of this user

    /** describe all supported output formats */
    QString fileFormats = "( ";
    for ( int i = 0; i < QImageWriter::supportedImageFormats().count(); i++ ) {
        fileFormats += "*."; /* Insert wildcard */
        fileFormats += QString( QImageWriter::supportedImageFormats().at(i) ).toLower(); /* Insert the format */
        fileFormats += " "; /* Insert a space */
    }
    fileFormats += ")";
#ifdef DEBUG_OUTPUT
	qDebug() << "Supported formats for writing include: " << fileFormats;
#endif
}




ClockIt::~ClockIt()
{
	delete http;
    delete ui;
}


void ClockIt::changeEvent(QEvent *e)
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


/**
  * @brief Unmount drive on quit and decrease file size
  */
void ClockIt::quitApp()
{
    qApp->quit();
}


/**
  * @brief Unmount drive on quit and decrease file size
  */
void ClockIt::about()
{
    QMessageBox msgbox;
    msgbox.setWindowTitle( "WorkClock" );
    msgbox.setText(
            QString("Version:  ") + glbRevision + "\n" +
            QString("Last Updated on:  ") + glbDateLastModified
     );
    msgbox.setInformativeText( "\n(C) Copyright 2017 Peregrine LLC\n" );
    msgbox.exec();
}



/* transparent bitmapped background functions */

void ClockIt::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    if ( pixmapBackground && ! pixmapBackground->isNull() ) {
        *pixmapBackground = pixmapBackground->scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, *pixmapBackground);
    }
}



void ClockIt::resizeEvent(QResizeEvent * /* event */ )
{
    if ( pixmapBackground && ! pixmapBackground->isNull() ) {
        *pixmapBackground = pixmapBackground->scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        setMask(pixmapBackground->mask());
    }
    setWindowOpacity(0.90);
}


void ClockIt::mousePressEvent(QMouseEvent * event)
{
    isDragging = false;
    if ( event->button() == Qt::LeftButton ) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        isDragging = true;
        event->accept();
    }
}



void ClockIt::mouseMoveEvent(QMouseEvent * event)
{
    if ( event->buttons() & Qt::LeftButton ) {
        if ( isDragging ) {
            move(event->globalPos() - dragPosition);
            event->accept();
        }
    }
}



/**
 *  @brief Set Icon
 *
 *  Sets icon in the tray
 *
*/
void ClockIt::setIcon()
{
    QIcon icon;
    icon.addFile(":/images/clock.png");
    trayIcon->setIcon(icon);
}


/**
 *  @brief TrayIcon activation reason
 *
*/
void ClockIt::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch ( reason ) {
		case QSystemTrayIcon::Trigger:
			// qDebug() << "iconActivated(Trigger)";
			punchClock();
			break;
		case QSystemTrayIcon::DoubleClick:
			// qDebug() << "iconActivated(DoubleClick)";
			viewEntries();
			break;
		case QSystemTrayIcon::MiddleClick:
			// qDebug() << "iconActivated(MiddleClick)";
			editPreferences();
			break;
		case QSystemTrayIcon::Context:
			// qDebug() << "iconActivated(Context)";
			break;
		default:
			break;
	}
}


/**
 *  @brief Create Actions
 *
*/
void ClockIt::createActions()
{
    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quitApp()));

    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    punchClockAction = new QAction (tr("&Punch Clock"), this);
    connect(punchClockAction, SIGNAL(triggered()), this, SLOT(punchClock()));

    viewEntriesAction = new QAction(tr("&View Entries"), this);
    connect(viewEntriesAction, SIGNAL(triggered()), this, SLOT(viewEntries()));

    configAction = new QAction (tr("&Configure"), this);
    connect(configAction, SIGNAL(triggered()), this, SLOT(editPreferences()));

}


/**
 *  @brief Create Tray Icon Menu
 *
*/
void ClockIt::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(punchClockAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(viewEntriesAction);
    trayIconMenu->addAction(configAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}


/**
 *  @brief Shows the configuration dialog when Configure is selected
 *
*/
void ClockIt::viewEntries()
{
	ViewEntries v(this);
    v.exec();
}


/**
 *  @brief Shows the configuration dialog when Configure is selected
 *
*/
void ClockIt::editPreferences()
{
    Preferences prefs(this);

	allUserNames.clear();
	allProjects.clear();
	ui->comboProject->clear();

    prefs.exec();

    readDefaultSettings();

	queryProjects();			// get the updated list of Projects
	updateClockStatus();
}


void ClockIt::readDefaultSettings()
{
    QSettings settings("WorkClock", "Config");
    project = Preferences::getProject();

	if ( ! project.isEmpty() ) {
		ui->comboProject->setCurrentText( project );
	}
}


void ClockIt::okClicked()
{

    float elapsedHours = (float) ui->dtClockInTime->dateTime().secsTo( ui->dtClockOutTime->dateTime() ) / (60.0 * 60.0);

    QString urlStr = "/punchclock.php";
    urlStr += "?login=" +  QUrl::toPercentEncoding( Preferences::getUserName() );
    urlStr += "&description=" + QUrl::toPercentEncoding(ui->efDescription->toPlainText());
    urlStr += "&project_id=" + QString::number( allProjects.key(project) );
    urlStr += "&project_name=" + QUrl::toPercentEncoding(ui->comboProject->currentText());
    urlStr += "&" + QString("hours=%1").arg(elapsedHours,0,'f',3);
    if ( ui->dtClockInTime->isEnabled() ) {
        urlStr += "&start_time=" + QUrl::toPercentEncoding(ui->dtClockInTime->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    if ( ui->dtClockOutTime->isEnabled() ) {
        urlStr += "&end_time=" + QUrl::toPercentEncoding(ui->dtClockOutTime->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
        if ( ui->efDescription->toPlainText().isEmpty() ) {
            QMessageBox msgbox;
            msgbox.setWindowTitle( "Clock Out Problem" );
            msgbox.setText( "Clock Out NOT successful" );
            msgbox.setInformativeText( "You must enter a description of all the tasks worked on during this period." );
            msgbox.exec();
            return;
        }
    }

#ifdef DEBUG_OUTPUT
	qDebug() << "okClicked() -> get(" << glbHostUrl + urlStr << ")";
#endif
	QNetworkReply *reply = http->get( QNetworkRequest(QUrl(glbHostUrl + urlStr )));
	reply->setProperty("whichone", QVariant("clockInsertion") );

}


void ClockIt::updateClockStatus()
{
	fieldLock();
	QNetworkReply *reply = http->get( QNetworkRequest(QUrl(QString(glbHostUrl) + "/clockread.php?login=" + QUrl::toPercentEncoding( Preferences::getUserName() ))) );
	reply->setProperty("whichone", QVariant("clockReading") );

    /* Check every 5 minutes if the user Clocked IN or OUT from the website instead of here. This was useful to ensure privacy for screen capturing whenever the user is not Clocked IN. */
    clockedInElsewhereTimer.start( TIME_CHECKING_FOR_CLOCKED_IN_ELSEWHERE );
}


int ClockIt::isClockedIn()
{
    return (trayIcon->toolTip() == "Clocked In");
}


void ClockIt::punchClock()
{
    if ( isVisible() ) {
        hide();
    } else {
        ui->dtClockInTime->setDateTime( QDateTime::fromString(clockFields["start_time"],"yyyy-MM-dd hh:mm:ss") );
#ifdef CONSTRAIN_CLOCK_IN_OUT_TIMES
        ui->dtClockOutTime->setMinimumDateTime( QDateTime::fromString(clockFields["start_time"],"yyyy-MM-dd hh:mm:ss") );
        ui->dtClockOutTime->setMaximumDateTime( QDateTime::currentDateTime() );
#endif
        ui->dtClockOutTime->setDateTime( QDateTime::currentDateTime() );

        ui->dtClockInTime->setDateTime( QDateTime::fromString(clockFields["start_time"],"yyyy-MM-dd hh:mm:ss") );
        ui->dtClockOutTime->setDateTime( QDateTime::currentDateTime() );

        updateClockStatus();
		ui->editLongAgo->setText("");
        show();	/* the punch clock dialog box */

        /* show that the UI is NOT ready to be used yet (not until the web returns the right values for the fields) */
        fieldLock();
    }
}


void ClockIt::fieldLock()
{
    ui->efDescription->setEnabled(false);
    ui->dtClockInTime->setEnabled(false);
    ui->dtClockOutTime->setEnabled(false);
    ui->clockout->setEnabled(false);
}


void ClockIt::fieldUnlock()
{
    ui->efDescription->setEnabled(true);
    ui->dtClockInTime->setEnabled(true);
    ui->dtClockOutTime->setEnabled(true);
    ui->clockout->setEnabled(true);
}


void ClockIt::on_comboProject_editTextChanged(const QString &txt )
{
	if ( ! txt.isEmpty() ) {
#ifdef DEBUG_OUTPUT
		qDebug() << "on_comboProject_editTextChanged() " << txt;
#endif
		on_comboProject_activated(txt);
	}
}

void ClockIt::on_comboProject_activated(const QString &theProject )
{
#ifdef DEBUG_OUTPUT
	qDebug() << "on_comboProject_activated(" << theProject << ")";
#endif
	if ( ! theProject.isEmpty() ) {
		project = theProject;
		QSettings settings("WorkClock", "Config");
		settings.setValue("project", theProject );
	}
}


void ClockIt::replyFinished( QNetworkReply *reply )
{
#ifdef DEBUG_OUTPUT
	if ( reply->error() ) {
		qDebug() << "ERROR!";
		qDebug() << reply->error();
		qDebug() << reply;
	} else {
		qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
		qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
		qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
		qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

		// qDebug() << reply->readAll();
	}
#endif

	QString strWhichOne = reply->property("whichone").toString();


#ifdef DEBUG_OUTPUT
	qDebug() << "ClockIt::httpResponse(" << reply->error() << ") for " << strWhichOne;
#endif

    if ( reply->error() ) {
#ifdef DEBUG_OUTPUT
		qDebug() << reply->error();
#endif
        showMessage( "TimeCard Status", QString("%1").arg(reply->error()), 0 );
        return;
    }

    if ( strWhichOne == "userId" ) {
        userId = QString( reply->readAll() );
#ifdef DEBUG_OUTPUT
		qDebug() << "userId = " << userId;
#endif
		QSettings settings("WorkClock", "Config");
		settings.setValue("userId", userId );
    }

    if ( strWhichOne == "Projects" ) {
        QStringList allProjIdNames = QString( reply->readAll() ).split("\n", QString::SkipEmptyParts);
		allProjIdNames.sort(Qt::CaseInsensitive);
#ifdef DEBUG_OUTPUT
		qDebug() << "comboProject->addItems(" << allProjIdNames << ")";
#endif

		foreach ( QString p, allProjIdNames ) {
			QStringList projIdName = p.split("\t", QString::SkipEmptyParts);
			allProjects[ projIdName.value(1).toInt() ] = projIdName.value(0);
		}

		QStringList projNames = allProjects.values();
		projNames.sort(Qt::CaseInsensitive);
		ui->comboProject->clear();
		ui->comboProject->addItems( projNames );
		ui->comboProject->setCurrentText( project );
    }

    if ( strWhichOne == "UserName" ) {
        allUserNames = QString( reply->readAll() ).split(":", QString::SkipEmptyParts);
		allUserNames.sort(Qt::CaseInsensitive);
    }

    if ( strWhichOne == "clockInsertion" ) {
#ifdef DEBUG_OUTPUT
		qDebug() << "Clock Insert:" << reply->readAll();
#endif
        /* retrieve the updated clock status */
        updateClockStatus();
    }

    if ( strWhichOne == "clockReading" ) {

        /* show that the UI is now ready to be used */
        fieldUnlock();

        QString webResp = reply->readAll();

        clockFields.clear();
        foreach ( QString kv, webResp.split("\03", QString::SkipEmptyParts) ) {
            clockFields[ kv.split("\02").value(0) ] = kv.split("\02").value(1);
        }

        /* if both start and end time are filled, then we want to clock-in with a brand new record */
        if ( ! clockFields["start_time"].isEmpty() && ! clockFields["end_time"].isEmpty() ) {
            clockFields.clear();
        }

#ifdef DEBUG_OUTPUT
		qDebug() << "Clock Reading:" << clockFields;
		qDebug() << "Current project:" << project;
		qDebug() << "All Projects:" << allProjects;
#endif

        /** initialize the state of the dialog box to be shown */
        ui->efDescription->setText( clockFields["description"] );
		if ( clockFields["project_id"] > 0 ) {
			ui->comboProject->setCurrentText( allProjects[ clockFields["project_id"].toInt() ] );
		} else {
			if ( ! project.isEmpty() ) {
				ui->comboProject->setCurrentText( project );
			}
		}
        ui->dtClockInTime->setEnabled(true);
        ui->dtClockOutTime->setEnabled(true);
        ui->dtClockInTime->setVisible(true);
        ui->dtClockOutTime->setVisible(true);
        ui->clockout->setVisible(true);


        /* if we are currently clocked OUT... */
        if ( clockFields["start_time"].isEmpty() ) {
			trayIcon->setToolTip("Clocked Out");

            ui->dtClockInTime->setDateTime( QDateTime::currentDateTime() );
            ui->dtClockOutTime->setEnabled(false);
            ui->dtClockOutTime->setVisible(false);
            ui->clockout->setVisible(false);

            trayIcon->setToolTip("Clocked Out");

        } else { /* otherwise, we are presently clocked IN... */
			float secsDiff = ui->dtClockInTime->dateTime().secsTo( QDateTime::currentDateTime() ) / 3600.0;
			QString s = QString("Clocked In\nHours worked: %1").arg(QString::number(secsDiff,'g',3));
			trayIcon->setToolTip(s);
		}
	}

	reply->deleteLater();
}


void ClockIt::queryProjects()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "ClockIt::queryProjects()";
#endif

    /* keep looking for all the Projects, in case the connection to the web site failed */
	QSettings settings("WorkClock", "Config");
	QNetworkReply *reply = http->get( QNetworkRequest(QUrl(QString(glbHostUrl) + QString("/projects.php"))));
	reply->setProperty("whichone", QVariant("Projects") );
}



void ClockIt::queryUserName()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "ClockIt::queryUserName()";
#endif

    /* keep looking for all the Projects, in case the connection to the web site failed */
    if ( allUserNames.isEmpty() ) {
		QSettings settings("WorkClock", "Config");
		QNetworkReply *reply = http->get( QNetworkRequest(QUrl(QString(glbHostUrl) + QString("/userid.php?employee=%1").arg(settings.value("employeeFilterNumber").toInt() ))));
		reply->setProperty("whichone", QVariant("UserName") );
    }
}


void ClockIt::showMessage(QString msgTitle, QString msgBody, int iconType)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "showMessage(" << msgTitle << "," << msgBody << ")";
#endif
    QSystemTrayIcon::MessageIcon mIcon = QSystemTrayIcon::MessageIcon(iconType);
    trayIcon->showMessage(msgTitle, msgBody, mIcon, 2000);
}


void ClockIt::on_editLongAgo_textChanged( const QString &txt )
{
	if ( isClockedIn() ) {
		if ( txt.toFloat() < 0.000001 ) {
			ui->dtClockOutTime->setDateTime( QDateTime::currentDateTime() );
		} else {
			ui->dtClockOutTime->setDateTime( ui->dtClockInTime->dateTime().addSecs( (qint64) ( 3600 * txt.toFloat()) ) );
		}
	} else {
        ui->dtClockInTime->setDateTime( QDateTime::currentDateTime().addSecs( (qint64) (-3600 * txt.toFloat()) ) );
	}
}


void ClockIt::showHours()
{
	if ( isClockedIn() ) {
		float secsDiff = ui->dtClockInTime->dateTime().secsTo( ui->dtClockOutTime->dateTime() ) / 3600.0;
QString s = QString("Hours worked: %1").arg(QString::number(secsDiff,'g',3));
qDebug() << "showHours()    " + s;
		ui->lblHoursWorked->setText( QString("Hours worked: %1").arg(QString::number(secsDiff,'g',3)) );
	} else {
		ui->lblHoursWorked->setText("");
	}
}


void ClockIt::on_dtClockOutTime_dateTimeChanged(const QDateTime &dateTime)
{
	showHours();
}


void ClockIt::on_dtClockInTime_dateTimeChanged(const QDateTime &dateTime)
{
	showHours();
}



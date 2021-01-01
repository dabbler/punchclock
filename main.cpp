
#include <QtWidgets>
#include <QTcpSocket>

#include "clockit.h"
#include "preferences.h"

extern void quickClockIn( QString userName );
extern void quickClockOut( QString userName );

QString glbHostname = "google.com";
QString glbHostUrl = (QString("http://") + glbHostname);



/** {{{ int main(int argc, char *argv[])
 */
int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(workclock);

	QApplication app(argc, argv);

	if ( ! QSystemTrayIcon::isSystemTrayAvailable() ) {
		QMessageBox::critical( 0, QObject::tr("WorkClock"), QObject::tr("I couldn't detect any system tray on this system.") );
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);

	glbHostname = Preferences::getHostName();
	glbHostUrl = (QString("http://") + glbHostname);

	ClockIt clock_in_a_tray;

	if ( argc > 1 ) {

		for ( int i = 0 ; i < argc ; i++ ) {
			if ( QString(argv[i]) == "login" ) {
				quickClockIn( Preferences::getUserName() );
			}
			if ( QString(argv[i]) == "logout" ) {
				quickClockOut( Preferences::getUserName() );
			}
		}
	} else {
		return app.exec();
	}

	return 0;
}
/* }}} */


/** {{{ void quickClockOut( QString userName )
 */
void quickClockOut( QString userName )
{
	QString urlStr = "/punchclock.php";
	urlStr += "?login=" +  QUrl::toPercentEncoding(userName);
	// urlStr += "&description=" + QUrl::toPercentEncoding("Automated Clock In: ");
	// urlStr += "&start_time=" + QUrl::toPercentEncoding(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	urlStr += "&end_time=" + QUrl::toPercentEncoding(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	// urlStr += QString("&hours=%1").arg(elapsedHours,0,'f',3);

	qDebug() << "quickClockOut() -> get(" << glbHostUrl + urlStr << ")";

	QTcpSocket *sockDirect = new QTcpSocket;
	sockDirect->connectToHost( glbHostname, 80 );

	if ( sockDirect->waitForConnected(15000) ) {

		sockDirect->write( QString("GET %1 HTTP/1.1\r\nHost: %2\r\n\r\n").arg(urlStr).arg(glbHostname).toLatin1().constData() );

		if (sockDirect->waitForBytesWritten(15000)) {

			if ( sockDirect->isReadable() ) {
				QString buf;
				sockDirect->waitForReadyRead(2000);
				buf = sockDirect->readAll();
				qDebug() << "ClockIt::quickClockOut()   buf = " << QString(buf);
			}

		} else {
			qDebug() << "Error : Can't write to " << glbHostUrl << ":80";
		}
	} else {
		qDebug() << "Error : Can't connect to " << glbHostUrl << ":80";
	}
}
/* }}} */

/** {{{ void quickClockIn( QString userName )
 */
void quickClockIn( QString userName )
{
	QString urlStr = "/punchclock.php";
	urlStr += "?login=" +  QUrl::toPercentEncoding(userName);
	urlStr += "&description=" + QUrl::toPercentEncoding("Automated Clock In: ");
	// urlStr += QString("&hours=%1").arg(elapsedHours,0,'f',3);
	urlStr += "&start_time=" + QUrl::toPercentEncoding(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	// urlStr += "&end_time=" + QUrl::toPercentEncoding(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

	qDebug() << "quickClockIn() -> get(" << glbHostUrl + urlStr << ")";

	QTcpSocket *sockDirect = new QTcpSocket;
	sockDirect->connectToHost( glbHostname, 80 );

	if ( sockDirect->waitForConnected(15000) ) {

		sockDirect->write( QString("GET %1 HTTP/1.1\r\nHost: %2\r\n\r\n").arg(urlStr).arg(glbHostname).toLatin1().constData() );

		if (sockDirect->waitForBytesWritten(15000)) {

			if ( sockDirect->isReadable() ) {
				QString buf;
				sockDirect->waitForReadyRead(2000);
				buf = sockDirect->readAll();
				qDebug() << "ClockIt::quickClockOut()   buf = " << QString(buf);
			}

		} else {
			qDebug() << "Error : Can't write to " << glbHostUrl << ":80";
		}
	} else {
		qDebug() << "Error : Can't connect to " << glbHostUrl << ":80";
	}
}
/* }}} */


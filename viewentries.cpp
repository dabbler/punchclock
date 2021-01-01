
#include <QtWidgets>

#include "viewentries.h"
#include "ui_viewentries.h"



/** {{{ ViewEntries::ViewEntries(QWidget *parent) : QDialog(parent), ui(new Ui::ViewEntries)
 */
ViewEntries::ViewEntries(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewEntries)
{
    ui->setupUi(this);

	QSettings settings("WorkClock", "Config");
	QStringList userNames = settings.value("userNames" ).toStringList();

	ui->comboUser->clear();
	ui->comboUser->addItems( userNames );

	ui->dateFrom->setDate( QDate::currentDate().addDays(-15) );
	ui->dateTo->setDate( QDate::currentDate() );

}
/* }}} */


/** {{{ ViewEntries::~ViewEntries()
 */
ViewEntries::~ViewEntries()
{
    delete ui;
}
/* }}} */



/** {{{ void ViewEntries::on_butOk_clicked()
 */
void ViewEntries::on_butOk_clicked()
{
	QString userName( QUrl::toPercentEncoding(ui->comboUser->currentText()) );

	QString link = QString("%1?from=%2&to=%3&name=%4")
			.arg("http://timecards.shinydata.com/entries/listhours")
			.arg( (ui->dateFrom->text()) )
			.arg( (ui->dateTo->text()) )
			.arg( userName );

	qDebug() << QString("openUrl(%1) => %2").arg(link).arg(
		QDesktopServices::openUrl( QUrl(link) )
		);

	accept();
}
/* }}} */


/** {{{ void ViewEntries::on_butCancel_clicked()
 */
void ViewEntries::on_butCancel_clicked()
{
	reject();
}
/* }}} */



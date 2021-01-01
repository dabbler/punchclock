/*
 *
 * Copyright (C) 2020 David Beccue
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see https://www.gnu.org/licenses/.
 *
 */

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



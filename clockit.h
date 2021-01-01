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
#ifndef CLOCKIT_H
#define CLOCKIT_H

#include <QTimer>
#include <QDialog>
#include <QBitmap>
#include <QProgressBar>
#include <QSystemTrayIcon>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#define TIME_CHECKING_FOR_CLOCKED_IN_ELSEWHERE	(1 * 60 * 1000)

extern QString glbHostname;
extern QString glbHostUrl;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE


namespace Ui {
  class ClockIt;
}

class ClockIt : public QDialog
{
    Q_OBJECT

public:
    ClockIt(QWidget *parent = 0);
    ~ClockIt();

    Ui::ClockIt *ui;

    QString userId;
    QString userName;
    QString project;

private:
    QPixmap *pixmapBackground;
    QPoint dragPosition;
    bool isDragging;

	QTimer transferProcessResetTimer;
	QTimer clockedInElsewhereTimer;

	QNetworkAccessManager *http;

    QHash<QString,QString> clockFields;
    QHash<int,QString> allProjects;
    QStringList allUserNames;

private slots:
    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage(QString msgTitle, QString msgBody, int iconType);
    void editPreferences();
	void showHours();

    void fieldLock();
    void fieldUnlock();

	void replyFinished( QNetworkReply *reply );

    void on_comboProject_activated(const QString &arg1);
    void on_comboProject_editTextChanged(const QString &arg1);

    void on_editLongAgo_textChanged(const QString &arg1);

    void on_dtClockOutTime_dateTimeChanged(const QDateTime &dateTime);

    void on_dtClockInTime_dateTimeChanged(const QDateTime &dateTime);

public slots:
    void punchClock();
    void updateClockStatus();
    void queryProjects();
	void queryUserName();

    void okClicked();

    void quitApp();
    void about();
    void viewEntries();

protected:
    void paintEvent(QPaintEvent * event);
    void resizeEvent(QResizeEvent * /* event */ );
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void changeEvent(QEvent *e);
    void readDefaultSettings();

    int isClockedIn();

private:

    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();

    QAction *quitAction;
    QAction *aboutAction;
    QAction *configAction;
    QAction *viewEntriesAction;
    QAction *punchClockAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // CLOCKIT_H

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
#ifndef VIEWENTRIES_H
#define VIEWENTRIES_H

#include <QDialog>

namespace Ui {
class ViewEntries;
}

class ViewEntries : public QDialog
{
    Q_OBJECT

public:
    explicit ViewEntries(QWidget *parent = 0);
    ~ViewEntries();

private:
    Ui::ViewEntries *ui;

private slots:
    void on_butOk_clicked();
    void on_butCancel_clicked();
};

#endif // VIEWENTRIES_H

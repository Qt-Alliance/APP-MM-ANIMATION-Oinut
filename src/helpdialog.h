/****************************************************************************
    Copyright (C) 2012-2015 Omer Bahri Gordebak <purplehuman at zoho.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include "ui_helpdialog.h"

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    HelpDialog(QWidget *parent = 0);

private slots:
    void escapeClicked();

private:
    Ui::HelpDialog ui;
};
#endif // HELPDIALOG_H

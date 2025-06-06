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

#ifndef BLURDIALOG_H
#define BLURDIALOG_H

#include <QDialog>
#include "ui_blurdialog.h"

class BlurDialog : public QDialog
{
    Q_OBJECT

public:
    BlurDialog(QWidget *parent = 0);
    int getRadius()
    { return ui.blurSpinBox->value(); }

private slots:
    void on_okPushButton_clicked();
    void on_cancelPushButton_clicked();

private:
    Ui::BlurDialog ui;
};
#endif // BLURDIALOG_H

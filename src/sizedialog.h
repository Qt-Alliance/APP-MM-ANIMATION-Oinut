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

#ifndef SIZEDIALOG_H
#define SIZEDIALOG_H

#include <QDialog>
#include "ui_sizedialog.h"
#include <QSettings>


class SizeDialog : public QDialog
{
    Q_OBJECT

public:
    SizeDialog(QWidget *parent = 0);
    void writeSettings();
    QSettings *settings;
    int w;
    int h;
    void setWidth(int width)
    { w = width;
      ui.widthSpinBox->setValue(w);
    }
    void setHeight(int height)
    { h = height;
      ui.heightSpinBox->setValue(h);
    }
    int getWidth()
    { return ui.widthSpinBox->value(); }
    int getHeight()
    { return ui.heightSpinBox->value(); }

private slots:
    void on_okPushButton_clicked();
    void on_cancelPushButton_clicked();

private:
    Ui::SizeDialog ui;
};
#endif // SIZEDIALOG_H

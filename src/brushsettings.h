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

#ifndef BRUSHSETTINGS_H
#define BRUSHSETTINGS_H

#include <QDialog>
#include "ui_brushsettings.h"
#include <QSettings>


class BrushSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    BrushSettingsDialog(QWidget *parent = 0);
    bool getSizeWithPressure();
    bool getAlphaWithPressure();
    bool getSaturationWithPressure();
    void setSizeWithPressure(bool isSize);
    void setAlphaWithPressure(bool isAlpha);
    void setSaturationWithPressure(bool isSaturation);

private slots:
    void on_okPushButton_clicked();
    void on_cancelPushButton_clicked();

private:
    Ui::BrushSettingsDialog ui;

};
#endif // BRUSHSETTINGS_H

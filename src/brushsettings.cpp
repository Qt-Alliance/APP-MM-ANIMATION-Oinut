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

#include <QtGui>
#include "brushsettings.h"

BrushSettingsDialog::BrushSettingsDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
}

void BrushSettingsDialog::on_okPushButton_clicked()
{
    accept();
}

void BrushSettingsDialog::on_cancelPushButton_clicked()
{
    reject();
}

bool BrushSettingsDialog::getSizeWithPressure()
{
    return ui.sizeCheckBox->isChecked();
}

bool BrushSettingsDialog::getAlphaWithPressure()
{
    return ui.alphaCheckBox->isChecked();
}

void BrushSettingsDialog::setSizeWithPressure(bool isSize)
{
    ui.sizeCheckBox->setChecked(isSize);
}

void BrushSettingsDialog::setAlphaWithPressure(bool isAlpha)
{
    ui.alphaCheckBox->setChecked(isAlpha);
}

bool BrushSettingsDialog::getSaturationWithPressure()
{
    return ui.saturationCheckBox->isChecked();
}

void BrushSettingsDialog::setSaturationWithPressure(bool isSaturation)
{
    ui.saturationCheckBox->setChecked(isSaturation);
}

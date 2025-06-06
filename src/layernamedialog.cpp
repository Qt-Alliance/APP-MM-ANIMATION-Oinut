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
#include "layernamedialog.h"

LayerNameDialog::LayerNameDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
}

void LayerNameDialog::on_okPushButton_clicked()
{
    accept();
}

void LayerNameDialog::on_cancelPushButton_clicked()
{
    reject();
}

QString LayerNameDialog::getName()
{
    return ui.lineEdit->text();
}

void LayerNameDialog::setName(QString layerName)
{
    ui.lineEdit->setText(layerName);
    ui.lineEdit->selectAll();
}

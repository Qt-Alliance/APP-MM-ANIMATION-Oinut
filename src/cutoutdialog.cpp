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

#include "cutoutdialog.h"
#include "ui_cutoutdialog.h"
#include "mainwindow.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <QtWidgets>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <stdlib.h>
#include <QSound>
#include <QSvgRenderer>
#include <QSvgGenerator>
#ifdef Q_OS_WIN
    #include <stdlib.h>
    #include <ctime>
#endif

using namespace std;

CutoutDialog::CutoutDialog(QWidget *) :
    QMainWindow(),
    ui(new Ui::CutoutDialog)
{
    ui->setupUi(this);
    mode = "move";

    setMinimumSize(820, 620);
    oldSelected.clear();
    isSetParent = false;
    isOnion = true;
    isSaved = true;
    fileName = "";
    parentId = 0;
    isGrid = false;
    childId = 0;
    childIdCount = 0;
    z = 0;
    username = QString(getenv("USER"));
    tmpDir = QDir::tempPath() + "/oinut-" + username + "/";
    isSceneEmpty = true;
    tmpname = "";
    path = "";
    zoomFactor = 1.0;

    firstRun();

    ui->actionAddImage->setDisabled(true);
    ui->actionChangeImage->setDisabled(true);
    ui->actionDeleteImage->setDisabled(true);
    ui->actionEditPivotPoint->setDisabled(true);
    ui->actionLock->setDisabled(true);
    ui->actionMove->setDisabled(true);
    ui->actionOnion3->setDisabled(true);
    ui->actionRestoreEdit->setDisabled(true);
    ui->actionRestoreRotate->setDisabled(true);
    ui->actionRestoreScale->setDisabled(true);
    ui->actionRotate->setDisabled(true);
    ui->actionRotate15->setDisabled(true);
    ui->actionSave->setDisabled(true);
    ui->actionSaveAs->setDisabled(true);
    ui->actionScale->setDisabled(true);
    ui->actionSendAboveParent->setDisabled(true);
    ui->actionSendBelowParent->setDisabled(true);
    ui->actionSendUp->setDisabled(true);
    ui->actionSendDown->setDisabled(true);
    ui->actionAddFrame->setDisabled(true);
    ui->actionInsertFrame->setDisabled(true);
    ui->actionReplaceFrame->setDisabled(true);
    ui->actionSetParent->setDisabled(true);
    ui->actionUnsetParent->setDisabled(true);
    ui->actionZoomIn->setDisabled(true);
    ui->actionZoomOut->setDisabled(true);
    ui->actionDragMode->setDisabled(true);
    ui->actionImportOinut->setDisabled(true);
    ui->actionGrid->setDisabled(true);
    ui->actionExportSelected->setDisabled(true);

    connect(ui->actionAddImage, SIGNAL(triggered()), this, SLOT(addImage()));
    connect(ui->actionChangeImage, SIGNAL(triggered()), this, SLOT(changeImage()));
    connect(ui->actionDeleteImage, SIGNAL(triggered()), this, SLOT(removeImage()));
    connect(ui->actionEditPivotPoint, SIGNAL(triggered()), this, SLOT(setModeEdit()));
    connect(ui->actionLock, SIGNAL(triggered()), this, SLOT(toggleLock()));
    connect(ui->actionMove, SIGNAL(triggered()), this, SLOT(setModeMove()));
    connect(ui->actionOnion3, SIGNAL(triggered()), this, SLOT(onion3()));
    connect(ui->actionRestoreEdit, SIGNAL(triggered()), this, SLOT(restoreItemEdit()));
    connect(ui->actionRestoreRotate, SIGNAL(triggered()), this, SLOT(restoreItemRotate()));
    connect(ui->actionRotate, SIGNAL(triggered()), this, SLOT(setModeRotate()));
    connect(ui->actionRotate15, SIGNAL(triggered()), this, SLOT(setModeRotate15()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionScale, SIGNAL(triggered()), this, SLOT(setModeScale()));
    connect(ui->actionSendUp, SIGNAL(triggered()), this, SLOT(sendUp()));
    connect(ui->actionSendDown, SIGNAL(triggered()), this, SLOT(sendDown()));
    connect(ui->actionSendAboveParent, SIGNAL(triggered()), this, SLOT(sendAboveParent()));
    connect(ui->actionSendBelowParent, SIGNAL(triggered()), this, SLOT(sendBelowParent()));
    connect(ui->actionAddFrame, SIGNAL(triggered()), this, SLOT(addFrame()));
    connect(ui->actionInsertFrame, SIGNAL(triggered()), this, SLOT(insertFrame()));
    connect(ui->actionReplaceFrame, SIGNAL(triggered()), this, SLOT(replaceFrame()));
    connect(ui->actionSetParent, SIGNAL(triggered()), this, SLOT(setParent()));
    connect(ui->actionUnsetParent, SIGNAL(triggered()), this, SLOT(unsetParent()));
    connect(ui->actionLoadLast, SIGNAL(triggered()), this, SLOT(loadLast()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quitApp()));
    connect(ui->actionRestoreScale, SIGNAL(triggered()), this, SLOT(restoreItemScale()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(ui->actionDragMode, SIGNAL(triggered()), this, SLOT(toggleDragMode()));
    connect(ui->actionImportOinut, SIGNAL(triggered()), this, SLOT(importOinut()));
    connect(ui->actionGrid, SIGNAL(triggered()), this, SLOT(toggleGrid()));
    connect(ui->actionExportSelected, SIGNAL(triggered()), this, SLOT(exportSelected()));

    new QShortcut(QKeySequence(Qt::Key_Up), this, SLOT(keyUp()));
    new QShortcut(QKeySequence(Qt::Key_Down), this, SLOT(keyDown()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(keyRight()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(keyLeft()));
    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapeClicked()));
    new QShortcut(QKeySequence(tr("Tab")), this, SLOT(selectNext()));
    new QShortcut(QKeySequence(tr("Shift+Tab")), this, SLOT(selectPrevious()));


    connect(scene, SIGNAL(selectionChanged()), this, SLOT(selectionChangedSlot()));
}

void CutoutDialog::keyUp()
{
    if(!isFirstRun) {
    QList<QGraphicsItem *> selected = scene->selectedItems();
    if(selected.count() > 0)
    {
        GraphicsPixmapItem *theItem = (GraphicsPixmapItem *)selected.first();
        if(!theItem->isLocked()) {
            qreal dx = 0;
            qreal dy = -1;
            qreal sc = selected.first()->scale();
            qreal iw = selected.first()->boundingRect().width();
            qreal ih = selected.first()->boundingRect().height();
            qreal dw = -((iw / 2) * 0.01);
            qreal dh = -((ih / 2) * 0.01);
            isSaved = false;
            if(mode == "move") {
                selected.first()->parentItem()->moveBy(dx, dy);
            } else if(mode == "edit") {
                selected.first()->moveBy(dx, dy);
            } else if(mode == "scale") {
                qreal ds;
                ds = sc + 0.01;
                selected.first()->moveBy(dw, dh);
                selected.first()->setScale(ds);
                if(selected.first()->childItems().count() > 0)  {
                    for(int c = 0; c < selected.first()->childItems().count(); c++) {
                        selected.first()->childItems().at(c)->setScale(1.0 / ds);
                    }
                }

            }
        }
    }
    }
}

void CutoutDialog::keyDown()
{
    if(!isFirstRun) {
    QList<QGraphicsItem *> selected = scene->selectedItems();
    if(selected.count() > 0)
    {
        GraphicsPixmapItem *theItem = (GraphicsPixmapItem *)selected.first();
        if(!theItem->isLocked()) {
            qreal dx = 0;
            qreal dy = 1;
            qreal sc = selected.first()->scale();
            qreal iw = selected.first()->boundingRect().width();
            qreal ih = selected.first()->boundingRect().height();
            qreal dw = ((iw / 2) * 0.01);
            qreal dh = ((ih / 2) * 0.01);
            isSaved = false;
            if(mode == "move") {
                selected.first()->parentItem()->moveBy(dx, dy);
            } else if(mode == "edit") {
                selected.first()->moveBy(dx, dy);
            } else if(mode == "scale") {
                qreal ds;
                ds = sc - 0.01;
                selected.first()->moveBy(dw, dh);
                selected.first()->setScale(ds);
                if(selected.first()->childItems().count() > 0)  {
                    for(int c = 0; c < selected.first()->childItems().count(); c++) {
                        selected.first()->childItems().at(c)->setScale(1.0 / ds);
                    }
                }
            }
        }
    }
    }
}

void CutoutDialog::keyRight()
{
    if(!isFirstRun) {
    QList<QGraphicsItem *> selected = scene->selectedItems();
    if(selected.count() > 0)
    {
        GraphicsPixmapItem *theItem = (GraphicsPixmapItem *)selected.first();
        if(!theItem->isLocked()) {
            qreal dx = 1;
            qreal dy = 0;
            isSaved = false;
            if(mode == "move") {
                selected.first()->parentItem()->moveBy(dx, dy);
            } else if(mode == "edit") {
                selected.first()->moveBy(dx, dy);
            } else if(mode == "rotate") {
                qreal rotation = selected.first()->parentItem()->rotation();
                ++rotation;
                qreal hc = selected.first()->parentItem()->boundingRect().center().x();
                qreal vc = selected.first()->parentItem()->boundingRect().center().y();
                selected.first()->parentItem()->setTransformOriginPoint(hc, vc);
                selected.first()->parentItem()->setRotation(rotation);
            } else if(mode == "rotate15") {
                qreal rotation = selected.first()->parentItem()->rotation();
                rotation += 15;
                qreal hc = selected.first()->parentItem()->boundingRect().center().x();
                qreal vc = selected.first()->parentItem()->boundingRect().center().y();
                selected.first()->parentItem()->setTransformOriginPoint(hc, vc);
                selected.first()->parentItem()->setRotation(rotation);
            }
        }
    }
    }
}

void CutoutDialog::keyLeft()
{
    if(!isFirstRun) {
    QList<QGraphicsItem *> selected = scene->selectedItems();
    if(selected.count() > 0)
    {
        GraphicsPixmapItem *theItem = (GraphicsPixmapItem *)selected.first();
        if(!theItem->isLocked()) {
            qreal dx = -1;
            qreal dy = 0;
            isSaved = false;
            if(mode == "move") {
                selected.first()->parentItem()->moveBy(dx, dy);
            } else if(mode == "edit") {
                selected.first()->moveBy(dx, dy);
            } else if(mode == "rotate") {
                qreal rotation = selected.first()->parentItem()->rotation();
                --rotation;
                qreal hc = selected.first()->parentItem()->boundingRect().center().x();
                qreal vc = selected.first()->parentItem()->boundingRect().center().y();
                selected.first()->parentItem()->setTransformOriginPoint(hc, vc);
                selected.first()->parentItem()->setRotation(rotation);
            } else if(mode == "rotate15") {
                qreal rotation = selected.first()->parentItem()->rotation();
                rotation -= 15;
                qreal hc = selected.first()->parentItem()->boundingRect().center().x();
                qreal vc = selected.first()->parentItem()->boundingRect().center().y();
                selected.first()->parentItem()->setTransformOriginPoint(hc, vc);
                selected.first()->parentItem()->setRotation(rotation);
            }

        }
    }
    }
}

void CutoutDialog::sendUp()
{
    if(scene->selectedItems().count() > 0) {
        QList<QGraphicsItem *> allItems = scene->items();
        QGraphicsItem *selected = scene->selectedItems().first();
        GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(!myItem->isLocked()) {
            QList<QGraphicsItem *> tmpList;
            int previousZValue = 0;
            tmpList.clear();
    
            if(selected->parentItem()->parentItem() == 0) {
                for(int k = 0; k < allItems.count(); k++)
                {
                    if(allItems.at(k)->type() == GraphicsPixmapItem::Type || allItems.at(k)->type() == GraphicsSvgItem::Type) {
                        if(allItems.at(k)->parentItem()->parentItem() == 0) {
                            tmpList.append(allItems.at(k));
                        }
                    }
                }
            } else {
                for(int k = 0; k < selected->parentItem()->parentItem()->childItems().count(); k++)
                {
                    tmpList.append(selected->parentItem()->parentItem()->childItems().at(k)->childItems().first());
                }
            }
            for(int i = 0; i < tmpList.count() - 1; i++)
            {
                for(int l = 0; l < tmpList.count() - i - 1; l++)
                {
                    if(tmpList.at(l)->zValue() > tmpList.at(l+1)->zValue())
                    {
                        QGraphicsItem *tmpPixmap;
                        tmpPixmap = tmpList.at(l);
                        tmpList.replace(l, tmpList.at(l+1));
                        tmpList.replace(l+1, tmpPixmap);
                    }
                }
            }
            if(selected->zValue() < tmpList.last()->zValue()) {
                for(int i = 0; i < tmpList.count() - 1; i++)
                {
                     if(tmpList.at(i)->zValue() == selected->zValue()) {
                         previousZValue = tmpList.at(i+1)->zValue();
                     }
                }
            } else {
                 previousZValue = selected->zValue();
            }
            for(int i = 0; i < allItems.count(); i++)
            {
                if(selected->parentItem()->parentItem() == 0) {
                    for(int k = 0; k < allItems.count(); k++)
                    {
                        if(allItems.at(k)->zValue() == previousZValue && (allItems.at(k)->type() == GraphicsPixmapItem::Type || allItems.at(k)->type() == GraphicsSvgItem::Type)) {
                            allItems.at(k)->setZValue(selected->zValue());
                            allItems.at(k)->parentItem()->setZValue(selected->zValue());
                            selected->setZValue(previousZValue);
                            selected->parentItem()->setZValue(previousZValue);
                        }
                    }
                } else {
                    QList<QGraphicsItem *> children = selected->parentItem()->parentItem()->childItems();
                    for(int k = 0; k < children.count(); k++)
                    {
                        if(children.at(k)->zValue() == previousZValue) {
                            children.at(k)->setZValue(selected->zValue());
                            children.at(k)->childItems().first()->setZValue(selected->zValue());
                            selected->setZValue(previousZValue);
                            selected->parentItem()->setZValue(previousZValue);
                        }
                    }
                }
            }
            isSaved = false;
        }
    }
}

void CutoutDialog::sendDown()
{
    if(scene->selectedItems().count() > 0) {
        QList<QGraphicsItem *> allItems = scene->items();
        QGraphicsItem *selected = scene->selectedItems().first();
        GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(!myItem->isLocked()) {
            QList<QGraphicsItem *> tmpList;
            int previousZValue = 0;
            tmpList.clear();
    
            if(selected->parentItem()->parentItem() == 0) {
                for(int k = 0; k < allItems.count(); k++)
                {
                    if(allItems.at(k)->type() == GraphicsPixmapItem::Type || allItems.at(k)->type() == GraphicsSvgItem::Type) {
                        if(allItems.at(k)->parentItem()->parentItem() == 0) {
                            tmpList.append(allItems.at(k));
                        }
                    }
                }
            } else {
                for(int k = 0; k < selected->parentItem()->parentItem()->childItems().count(); k++)
                {
                    tmpList.append(selected->parentItem()->parentItem()->childItems().at(k)->childItems().first());
                }
            }
            for(int i = 0; i < tmpList.count() - 1; i++)
            {
                for(int l = 0; l < tmpList.count() - i - 1; l++)
                {
                    if(tmpList.at(l)->zValue() > tmpList.at(l+1)->zValue())
                    {
                        QGraphicsItem *tmpPixmap;
                        tmpPixmap = tmpList.at(l);
                        tmpList.replace(l, tmpList.at(l+1));
                        tmpList.replace(l+1, tmpPixmap);
                    }
                }
            }
            if(selected->zValue() > tmpList.first()->zValue()) {
                for(int i = 1; i < tmpList.count(); i++)
                {
                     if(tmpList.at(i)->zValue() == selected->zValue()) {
                         previousZValue = tmpList.at(i-1)->zValue();
                     }
                }
            } else {
                 previousZValue = selected->zValue();
            }
            for(int i = 0; i < allItems.count(); i++)
            {
                if(selected->parentItem()->parentItem() == 0) {
                    for(int k = 0; k < allItems.count(); k++)
                    {
                        if(allItems.at(k)->zValue() == previousZValue && (allItems.at(k)->type() == GraphicsPixmapItem::Type || allItems.at(k)->type() == GraphicsSvgItem::Type)) {
                            allItems.at(k)->setZValue(selected->zValue());
                            allItems.at(k)->parentItem()->setZValue(selected->zValue());
                            selected->setZValue(previousZValue);
                            selected->parentItem()->setZValue(previousZValue);
                        }
                    }
                } else {
                    QList<QGraphicsItem *> children = selected->parentItem()->parentItem()->childItems();
                    for(int k = 0; k < children.count(); k++)
                    {
                        if(children.at(k)->zValue() == previousZValue) {
                            children.at(k)->setZValue(selected->zValue());
                            children.at(k)->childItems().first()->setZValue(selected->zValue());
                            selected->setZValue(previousZValue);
                            selected->parentItem()->setZValue(previousZValue);
                        }
                    }
                }
            }
            isSaved = false;
        }
    }
}

void CutoutDialog::setParent()
{
    if(scene->selectedItems().count() > 0) {
        GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(!myItem->isLocked()) {
            if(isSetParent) {
                isSetParent = false;
                ui->actionSetParent->setIcon(QIcon(":images/setparent.svg"));
            } else {
                isSetParent = true;
                ui->actionSetParent->setIcon(QIcon(":images/setparentSelected.svg"));
            }
        }
    }
}

void CutoutDialog::unsetParent()
{
    QList<QGraphicsItem *> selected = scene->selectedItems();
    if(selected.count() > 0)
    {
        if(selected.first()->type() == GraphicsPixmapItem::Type) {
            GraphicsPixmapItem * selectedPixmap = (GraphicsPixmapItem *)selected.first();
            if(!selectedPixmap->isLocked()) {
                selectedPixmap->ParentId = 0;
                selectedPixmap->parentItem()->setParentItem(0);
                selectedPixmap->parentItem()->setScale(1.0);
                selectedPixmap->parentItem()->setPos(background->pos().x() + (background->boundingRect().width() / 2) - 5, background->pos().y() + (background->boundingRect().height()  / 2) - 5);
                selectedPixmap->parentItem()->setRotation(0);
                isSaved = false;
            }
        } else {
            GraphicsSvgItem * selectedItem = (GraphicsSvgItem *)selected.first();
            if(!selectedItem->isLocked()) {
                selectedItem->ParentId = 0;
                selectedItem->parentItem()->setParentItem(0);
                selectedItem->parentItem()->setScale(1.0);
                selectedItem->parentItem()->setPos(background->pos().x() + (background->boundingRect().width() / 2) - 5, background->pos().y() + (background->boundingRect().height()  / 2) - 5);
                selectedItem->parentItem()->setRotation(0);
                isSaved = false;
            }
        }
    }
}

void CutoutDialog::setModeRotate()
{
    mode = "rotate";
    if(scene->selectedItems().count() > 0) {
        if(scene->selectedItems().first() -> type() == GraphicsPixmapItem::Type) {
            GraphicsPixmapItem *tmpItem = (GraphicsPixmapItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
        } else {
            GraphicsSvgItem *tmpItem = (GraphicsSvgItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
        }
        PivotItem *tmpParent = (PivotItem *)scene->selectedItems().first()->parentItem();
        tmpParent->setMode(mode);
    }
    ui->actionEditPivotPoint->setIcon(QIcon(":images/edit.svg"));
    ui->actionMove->setIcon(QIcon(":images/move.svg"));
    ui->actionRotate->setIcon(QIcon(":images/rotateSelected.svg"));
    ui->actionRotate15->setIcon(QIcon(":images/rotate15.svg"));
    ui->actionScale->setIcon(QIcon(":images/scale.svg"));
}

void CutoutDialog::setModeRotate15()
{
    mode = "rotate15";
    if(scene->selectedItems().count() > 0) {
        if(scene->selectedItems().first() -> type() == GraphicsPixmapItem::Type) {
            GraphicsPixmapItem *tmpItem = (GraphicsPixmapItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
            PivotItem *parentItem = (PivotItem *)tmpItem->parentItem();
            parentItem->setMode(mode);
        } else {
            GraphicsSvgItem *tmpItem = (GraphicsSvgItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
            PivotItem *parentItem = (PivotItem *)tmpItem->parentItem();
            parentItem->setMode(mode);
        }
        PivotItem *tmpParent = (PivotItem *)scene->selectedItems().first()->parentItem();
        tmpParent->setMode(mode);
    }
    ui->actionEditPivotPoint->setIcon(QIcon(":images/edit.svg"));
    ui->actionMove->setIcon(QIcon(":images/move.svg"));
    ui->actionRotate->setIcon(QIcon(":images/rotate.svg"));
    ui->actionRotate15->setIcon(QIcon(":images/rotate15Selected.svg"));
    ui->actionScale->setIcon(QIcon(":images/scale.svg"));
}

void CutoutDialog::setModeMove()
{
    mode = "move";
    if(scene->selectedItems().count() > 0) {
        if(scene->selectedItems().first() -> type() == GraphicsPixmapItem::Type) {
            GraphicsPixmapItem *tmpItem = (GraphicsPixmapItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
        } else {
            GraphicsSvgItem *tmpItem = (GraphicsSvgItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
        }
        PivotItem *tmpParent = (PivotItem *)scene->selectedItems().first()->parentItem();
        tmpParent->setMode(mode);
    }
    ui->actionEditPivotPoint->setIcon(QIcon(":images/edit.svg"));
    ui->actionMove->setIcon(QIcon(":images/moveSelected.svg"));
    ui->actionRotate->setIcon(QIcon(":images/rotate.svg"));
    ui->actionRotate15->setIcon(QIcon(":images/rotate15.svg"));
    ui->actionScale->setIcon(QIcon(":images/scale.svg"));
}

void CutoutDialog::setModeEdit()
{
    mode = "edit";
    if(scene->selectedItems().count() > 0) {
        if(scene->selectedItems().first() -> type() == GraphicsPixmapItem::Type) {
            GraphicsPixmapItem *tmpItem = (GraphicsPixmapItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
        } else {
            GraphicsSvgItem *tmpItem = (GraphicsSvgItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
        }
        PivotItem *tmpParent = (PivotItem *)scene->selectedItems().first()->parentItem();
        tmpParent->setMode(mode);
    }
    ui->actionEditPivotPoint->setIcon(QIcon(":images/editSelected.svg"));
    ui->actionMove->setIcon(QIcon(":images/move.svg"));
    ui->actionRotate->setIcon(QIcon(":images/rotate.svg"));
    ui->actionRotate15->setIcon(QIcon(":images/rotate15.svg"));
    ui->actionScale->setIcon(QIcon(":images/scale.svg"));
}

void CutoutDialog::setModeScale()
{
    mode = "scale";
    if(scene->selectedItems().count() > 0) {
        if(scene->selectedItems().first() -> type() == GraphicsPixmapItem::Type) {
            GraphicsPixmapItem *tmpItem = (GraphicsPixmapItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
        } else {
            GraphicsSvgItem *tmpItem = (GraphicsSvgItem *)scene->selectedItems().first();
            tmpItem->setMode(mode);
        }
        PivotItem *tmpParent = (PivotItem *)scene->selectedItems().first()->parentItem();
        tmpParent->setMode(mode);
    }
    ui->actionEditPivotPoint->setIcon(QIcon(":images/edit.svg"));
    ui->actionMove->setIcon(QIcon(":images/move.svg"));
    ui->actionRotate->setIcon(QIcon(":images/rotate.svg"));
    ui->actionRotate15->setIcon(QIcon(":images/rotate15.svg"));
    ui->actionScale->setIcon(QIcon(":images/scaleSelected.svg"));
}

void CutoutDialog::selectionChangedSlot()
{
    QList<QGraphicsItem *> allItems = scene->items();
    for(int l = 0; l < allItems.count(); l++)
    {
        if(allItems.at(l)->type() == PivotItem::Type)
        {
            if(!allItems.at(l)->childItems().first()->isSelected())
            {
                allItems.at(l)->setOpacity(0.0);
            } else {
                allItems.at(l)->setOpacity(1.0);
            }
        }
    }
    for(int i = 0; i < oldSelected.count(); i++)
    {
        oldSelected.at(i)->setSelected(false);
        if(scene->selectedItems().count() > 0) {
            if(isSetParent) {
                if(!oldSelected.at(i)->isAncestorOf(scene->selectedItems().first()) && oldSelected.at(i) != scene->selectedItems().first()) {
                    oldSelected.at(i)->parentItem()->setParentItem(scene->selectedItems().first());
                    qreal hc = scene->selectedItems().first()->boundingRect().center().x();
                    qreal vc = scene->selectedItems().first()->boundingRect().center().y();
                    oldSelected.at(i)->parentItem()->setPos(hc, vc);
                    oldSelected.at(i)->parentItem()->setScale(1.0 / scene->selectedItems().first()->scale());
                    GraphicsPixmapItem *oldPixmap = (GraphicsPixmapItem *)oldSelected.at(i);
                    PivotItem *tmpEllipse = (PivotItem *)scene->selectedItems().first()->parentItem();
                    GraphicsPixmapItem *tmpPixmap = (GraphicsPixmapItem *)scene->selectedItems().first();
                    tmpEllipse->ParentId = tmpPixmap->childId();
                    oldPixmap->ParentId = tmpPixmap->childId();
                    isSetParent = false;
                    ui->actionSetParent->setIcon(QIcon(":images/setparent.svg"));
                    isSaved = false;
                }
            }
        }
    }
    if(scene->selectedItems().count() > 0) {
        GraphicsPixmapItem *lockedItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(lockedItem->isLocked()) {
            ui->actionLock->setIcon(QIcon(":images/lockSelected.svg"));
        } else {
            ui->actionLock->setIcon(QIcon(":images/lock.svg"));
        }
    }
    if(scene->selectedItems().count() > 0) {
        GraphicsPixmapItem *tmpItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        tmpItem->setMode(mode);
        PivotItem *tmpParent = (PivotItem *)scene->selectedItems().first()->parentItem();
        tmpParent->setMode(mode);
	if(tmpItem->hasChanged())// || tmpParent->hasChanged())
		isSaved = false;
    }
    if(oldSelected.count() > 0) {
        GraphicsPixmapItem *oldItem = (GraphicsPixmapItem *)oldSelected.first();
        PivotItem *oldParent = (PivotItem *)oldItem->parentItem();
        if(oldItem->hasChanged() || oldParent->hasChanged())
	    isSaved = false;
    }
    oldSelected = scene->selectedItems();
}

void PivotItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    initialPos = mapToParent(event->pos());
    QGraphicsItem::mousePressEvent(event);
}
 
void PivotItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isLocked()) {
        QPointF pos;
        qreal sc = childItems().first()->scale();
        qreal iw = childItems().first()->boundingRect().width();
        qreal ih = childItems().first()->boundingRect().height();
        qreal dw = -((iw / 2) * 0.01);
        qreal dh = -((ih / 2) * 0.01);
        pos = mapToParent(event->pos());
        isSaved = false;
        if(mode == "rotate") {
            if (pos.y() > initialPos.y()) {
                ++rot;
            } else {
                --rot;
            }
            qreal hc = boundingRect().center().x();
            qreal vc = boundingRect().center().y();
            setTransformOriginPoint(hc, vc);
            setRotation(rot);
        } else if(mode == "move") {
            qreal dx = pos.x() - initialPos.x();
            qreal dy = pos.y() - initialPos.y();
            moveBy(dx, dy);
        } else if(mode == "scale") {
            qreal ds;
            if(pos.y() > initialPos.y()) {
                ds = sc - 0.01;
                childItems().first()->moveBy(-dw, -dh);
            } else {
                ds = sc + 0.01;
                childItems().first()->moveBy(dw, dh);
            }
            childItems().first()->setScale(ds);
            if(childItems().first()->childItems().count() > 0)  {
                for(int c = 0; c < childItems().first()->childItems().count(); c++) {
                    childItems().first()->childItems().at(c)->setScale(1.0 / ds);
                }
            }
        }
        initialPos = pos;
    }
}

void GraphicsPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setSelected(true);
    if(mode == "scale" || mode == "edit") 
        initialPos = mapToParent(event->pos());
    else
        initialPos = parentItem()->mapToParent(event->pos());
    QGraphicsItem::mousePressEvent(event);
}
 
void GraphicsPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isLocked()) {
        QPointF pos;
        qreal sc = scale();
        qreal iw = boundingRect().width();
        qreal ih = boundingRect().height();
        qreal dw = -((iw / 2) * 0.01);
        qreal dh = -((ih / 2) * 0.01);
        isSaved = false;
        if(mode == "scale" || mode == "edit") 
            pos = mapToParent(event->pos());
        else
            pos = parentItem()->mapToParent(event->pos());
        if(mode == "rotate") {
            if (pos.y() > initialPos.y()) {
                ++rot;
            } else {
                --rot;
            }
            qreal hc = parentItem()->boundingRect().center().x();
            qreal vc = parentItem()->boundingRect().center().y();
            parentItem()->setTransformOriginPoint(hc, vc);
            parentItem()->setRotation(rot);
        } else if(mode == "move"){
            qreal dx = pos.x() - initialPos.x();
            qreal dy = pos.y() - initialPos.y();
            parentItem()->moveBy(dx, dy);
        } else if(mode == "edit"){
            qreal dx = pos.x() - initialPos.x();
            qreal dy = pos.y() - initialPos.y();
            moveBy(dx, dy);
        } else if(mode == "scale") {
            qreal ds;
            if(pos.y() > initialPos.y()) {
                ds = sc - 0.01;
                moveBy(-dw, -dh);
            } else {
                ds = sc + 0.01;
                moveBy(dw, dh);
            }
            setScale(ds);
            if(childItems().count() > 0) {
                for(int c = 0; c < childItems().count(); c++) {
                    childItems().at(c)->setScale(1.0 / ds);
                }
            }
        }
        initialPos = pos;
    }
}

void GraphicsSvgItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setSelected(true);
    if(mode == "scale" || mode == "edit") 
        initialPos = mapToParent(event->pos());
    else
        initialPos = parentItem()->mapToParent(event->pos());
    QGraphicsItem::mousePressEvent(event);
}
 
void GraphicsSvgItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isLocked()) {
        QPointF pos;
        qreal sc = scale();
        qreal iw = boundingRect().width();
        qreal ih = boundingRect().height();
        qreal dw = -((iw / 2) * 0.01);
        qreal dh = -((ih / 2) * 0.01);
        isSaved = false;
        if(mode == "scale" || mode == "edit") 
            pos = mapToParent(event->pos());
        else
            pos = parentItem()->mapToParent(event->pos());
        if(mode == "rotate") {
            if (pos.y() > initialPos.y()) {
                ++rot;
            } else {
                --rot;
            }
            qreal hc = parentItem()->boundingRect().center().x();
            qreal vc = parentItem()->boundingRect().center().y();
            parentItem()->setTransformOriginPoint(hc, vc);
            parentItem()->setRotation(rot);
        } else if(mode == "move"){
            qreal dx = pos.x() - initialPos.x();
            qreal dy = pos.y() - initialPos.y();
            parentItem()->moveBy(dx, dy);
        } else if(mode == "edit"){
            qreal dx = pos.x() - initialPos.x();
            qreal dy = pos.y() - initialPos.y();
            moveBy(dx, dy);
        } else if(mode == "scale") {
            qreal ds;
            if(pos.y() > initialPos.y()) {
                ds = sc - 0.01;
                moveBy(-dw, -dh);
            } else {
                ds = sc + 0.01;
                moveBy(dw, dh);
            }
            setScale(ds);
            if(childItems().count() > 0) {
                for(int c = 0; c < childItems().count(); c++) {
                    childItems().at(c)->setScale(1.0 / ds);
                }
            }
        }
        initialPos = pos;
    }
}

void CutoutDialog::newAnim()
{
    if(maybeSave()) {
        makeTmpName();

            enableActions();

            if(!isFirstRun) {
                delete graphicsView;
            }
            isFirstRun = false;
    
            for(int i = 0; i < scene->items().count(); i++)
                if(scene->items().at(i)->scene() == scene && scene->items().at(i)->parentItem() == 0)
                    scene->removeItem(scene->items().at(i));
            scene->clear();
    
            scene->setSceneRect(0, 0, 3000, 3000);
            graphicsView = new QGraphicsView;
            graphicsView->setScene(scene);
            graphicsView->setSceneRect(0, 0, 3000, 3000);
            graphicsView->setRenderHint(QPainter::Antialiasing);
            graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            graphicsView->viewport()->setAutoFillBackground(false);
            graphicsView->setAlignment(Qt::AlignCenter);
            graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
            graphicsView->setAttribute(Qt::WA_TranslucentBackground, true);
            graphicsView->setAutoFillBackground(false);

            QPixmap backgroundPix(w, h);
            backgroundPix.fill(Qt::white);
            background = new QGraphicsPixmapItem;
            background->setPixmap(backgroundPix);
            scene->addItem(background);
            background->setZValue(-2000);
            background->setPos((scene->sceneRect().center().x()) - (w / 2), (scene->sceneRect().center().y()) - (h / 2));

            frameShow = new QGraphicsRectItem;
            QPen pen(Qt::red, 2);
            pen.setStyle(Qt::DotLine);
            QBrush brush(Qt::transparent);
            frameShow->setPen(pen);
            frameShow->setBrush(brush);
            frameShow->setZValue(3000);
            frameShow->setRect(0, 0, w, h);
            frameShow->setPos((scene->sceneRect().center().x()) - (w / 2), (scene->sceneRect().center().y()) - (h / 2));
            scene->addItem(frameShow);

            ui->viewLayout->addWidget(graphicsView);

            framePixmaps.clear();
            pixmapItems.clear();
            oldSelected.clear();
            isSetParent = false;
            isOnion = false;
            isGrid = false;
            ui->actionGrid->setIcon(QIcon(":images/grid.svg"));
            onion3();
            isSaved = true;
            isSceneEmpty = true;
            fileName = "";
            parentId = 0;
            childId = 0;
            childIdCount = 0;
            setModeMove();
        }
}

void CutoutDialog::firstRun()
{
    isFirstRun = true;
    isSaved = true;
    scene = new QGraphicsScene(this);

    pixmapItems.clear();
    framePixmaps.clear();
}

void CutoutDialog::replaceFrame()
{
    isSaved = false;
    QList<QGraphicsItem *> allItems = scene->items();
    restoreSelected = scene->selectedItems();
    if(background->scene() == scene)
        scene->removeItem(background);
    if(frameShow->scene() == scene)
        scene->removeItem(frameShow);
    for(int l = 0; l < pixmapItems.count(); l++)
        if(pixmapItems.at(l)->scene() == scene)
            scene->removeItem(pixmapItems.at(l));
    for(int i = 0; i < allItems.count(); i++)
            if(allItems.at(i)->scene() == scene && allItems.at(i)->parentItem() == 0)
                scene->removeItem(allItems.at(i));
    for(int i = 0; i < allItems.count(); i++)
        if(allItems.at(i)->parentItem() == 0 && allItems.at(i)->type() == PivotItem::Type) {
            scene->addItem(allItems.at(i));
            allItems.at(i)->setOpacity(0.0);
        }
    scene->clearSelection();
    QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem();
    QPixmap pixmap(w, h);
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    scene->render(&painter, QRectF(0, 0, w, h), background->mapRectToScene(background->boundingRect()));
    painter.end();

    pixmapItem->setPixmap(pixmap);
    pixmapItem->setPos(background->pos());
    pixmapItem->setZValue(-1000);
    pixmapItems.append(pixmapItem);
    win->replaceImage(pixmap);

    scene->addItem(background);
    QPixmap framePixmap(w, h);
    framePixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter2;
    painter2.begin(&framePixmap);
    painter2.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    scene->render(&painter2, QRectF(0, 0, w, h), background->mapRectToScene(background->boundingRect()));
    painter2.end();
    framePixmaps.append(framePixmap);

    if(isOnion) {
        for(int i = 0; i < pixmapItems.count(); i++)
        {
                if(i == pixmapItems.count() - 3) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.2);
                }
                if(i == pixmapItems.count() - 2) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.4);
                }
                if(i == pixmapItems.count() - 1) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.8);
                }
        }
    }
    scene->addItem(frameShow);
    if(restoreSelected.count() > 0)
        restoreSelected.first()->setSelected(true);
}

void CutoutDialog::insertFrame()
{
    isSaved = false;
    QList<QGraphicsItem *> allItems = scene->items();
    restoreSelected = scene->selectedItems();
    if(background->scene() == scene)
        scene->removeItem(background);
    if(frameShow->scene() == scene)
        scene->removeItem(frameShow);
    for(int i = 0; i < pixmapItems.count(); i++)
        if(pixmapItems.at(i)->scene() == scene)
            scene->removeItem(pixmapItems.at(i));
    for(int i = 0; i < allItems.count(); i++)
            if(allItems.at(i)->scene() == scene && allItems.at(i)->parentItem() == 0)
                scene->removeItem(allItems.at(i));
    for(int i = 0; i < allItems.count(); i++)
        if(allItems.at(i)->parentItem() == 0 && allItems.at(i)->type() == PivotItem::Type) {
            scene->addItem(allItems.at(i));
            allItems.at(i)->setOpacity(0.0);
        }
    scene->clearSelection();
    QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem();
    QPixmap pixmap(w, h);
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    scene->render(&painter, QRectF(0, 0, w, h), background->mapRectToScene(background->boundingRect()));
    painter.end();

    pixmapItem->setPixmap(pixmap);
    pixmapItem->setPos(background->pos());
    pixmapItem->setZValue(-1000);
    pixmapItems.append(pixmapItem);
    win->insertImage(pixmap);

    scene->addItem(background);
    QPixmap framePixmap(w, h);
    framePixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter2;
    painter2.begin(&framePixmap);
    painter2.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    scene->render(&painter2, QRectF(0, 0, w, h), background->mapRectToScene(background->boundingRect()));
    painter2.end();
    framePixmaps.append(framePixmap);

    if(isOnion) {
        for(int i = 0; i < pixmapItems.count(); i++)
        {
                if(i == pixmapItems.count() - 3) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.2);
                }
                if(i == pixmapItems.count() - 2) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.4);
                }
                if(i == pixmapItems.count() - 1) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.8);
                }
        }
    }
    scene->addItem(frameShow);
    if(restoreSelected.count() > 0)
        restoreSelected.first()->setSelected(true);
}

void CutoutDialog::addFrame()
{
    isSaved = false;
    QList<QGraphicsItem *> allItems = scene->items();
    restoreSelected = scene->selectedItems();
    if(background->scene() == scene)
        scene->removeItem(background);
    if(frameShow->scene() == scene)
        scene->removeItem(frameShow);
    for(int i = 0; i < pixmapItems.count(); i++)
        if(pixmapItems.at(i)->scene() == scene)
            scene->removeItem(pixmapItems.at(i));
    for(int i = 0; i < allItems.count(); i++)
            if(allItems.at(i)->scene() == scene && allItems.at(i)->parentItem() == 0)
                scene->removeItem(allItems.at(i));
    for(int i = 0; i < allItems.count(); i++)
        if(allItems.at(i)->parentItem() == 0 && allItems.at(i)->type() == PivotItem::Type) {
            scene->addItem(allItems.at(i));
            allItems.at(i)->setOpacity(0.0);
        }
    scene->clearSelection();
    QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem();
    QPixmap pixmap(w, h);
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    scene->render(&painter, QRectF(0, 0, w, h), background->mapRectToScene(background->boundingRect()));
    painter.end();

    pixmapItem->setPixmap(pixmap);
    pixmapItem->setPos(background->pos());
    pixmapItem->setZValue(-1000);
    pixmapItems.append(pixmapItem);
    win->addImage(pixmap);

    scene->addItem(background);
    QPixmap framePixmap(w, h);
    framePixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter2;
    painter2.begin(&framePixmap);
    painter2.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    scene->render(&painter2, QRectF(0, 0, w, h), background->mapRectToScene(background->boundingRect()));
    painter2.end();
    framePixmaps.append(framePixmap);

    if(isOnion) {
        for(int i = 0; i < pixmapItems.count(); i++)
        {
                if(i == pixmapItems.count() - 3) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.2);
                }
                if(i == pixmapItems.count() - 2) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.4);
                }
                if(i == pixmapItems.count() - 1) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.8);
                }
        }
    }
    scene->addItem(frameShow);
    if(restoreSelected.count() > 0)
        restoreSelected.first()->setSelected(true);
}

void CutoutDialog::restoreWorkspace()
{
    for(int i = 0; i < scene->items().count(); i++)
        if(scene->items().at(i)->scene() == scene && scene->items().at(i)->parentItem() == 0)
            scene->removeItem(scene->items().at(i));
    QPixmap bgPixmap(w, h);
    bgPixmap.fill(Qt::white);
    if(isGrid) {
        QPainter painter(&bgPixmap);
        painter.setPen(QPen(Qt::blue, 2));
        painter.setOpacity(0.1);
        for(int i = 0; i < w; i = i + 20)
            painter.drawLine(QPointF(i, 0), QPointF(i, h));
        for(int i = 0; i < h; i = i + 20)
            painter.drawLine(QPointF(0, i), QPointF(w, i));
        painter.end();
    }
    background->setPixmap(bgPixmap);
    scene->addItem(background);
    if(isOnion) {
        for(int i = 0; i < pixmapItems.count(); i++)
        {
                if(i == pixmapItems.count() - 3) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.2);
                }
                if(i == pixmapItems.count() - 2) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.4);
                }
                if(i == pixmapItems.count() - 1) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.8);
                }
        }
    }
    for(int i = 0; i < sceneItems.count(); i++) 
        if(sceneItems.at(i)->type() == PivotItem::Type && sceneItems.at(i)->parentItem() == NULL) {
            scene->addItem(sceneItems.at(i));
        }
    scene->addItem(frameShow);
    if(restoreSelected.count() > 0)
        restoreSelected.first()->setSelected(true);
}

void CutoutDialog::addImage()
{
    QString itemFileName = QFileDialog::getOpenFileName(
                this,
                "Select a file to open",
                QDir::homePath(),
                "Images (*.png *.jpg);;Scalable Vector Graphics (*.svg)");
    if (!itemFileName.isEmpty())
    {
        if(itemFileName.endsWith("png", Qt::CaseInsensitive) || itemFileName.endsWith("jpg", Qt::CaseInsensitive)) {
            GraphicsPixmapItem *item = new GraphicsPixmapItem();
            item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemSendsGeometryChanges);
            QPixmap itemPixmap;
            itemPixmap.load(itemFileName);
            item->setPixmap(itemPixmap);
            item->fileName = itemFileName;
            int hc = item->boundingRect().center().x();
            int vc = item->boundingRect().center().y();
            item->setPos(-(hc - 5), -(vc - 5));
            ++childId;
            item->ParentId = 0;
            item->ChildId = childId;
            item->IsLocked = false;
            ++z;
            item->setZValue(z);

            PivotItem *itempivot = new PivotItem();
            itempivot->setRect(0, 0, 10, 10);
            itempivot->setPos(background->pos().x() + (background->boundingRect().width() / 2) - 5, background->pos().y() + (background->boundingRect().height()  / 2) - 5);
            itempivot->setOpacity(0.0);
            itempivot->ParentId = item->parentId();
            itempivot->ChildId = item->childId();
            itempivot->IsLocked = false;
            scene->addItem(itempivot);
            itempivot->setZValue(z);

            item->setParentItem(itempivot);
            item->setSelected(true);
            isSaved = false;
            isSceneEmpty = false;
        } else if(itemFileName.endsWith("svg", Qt::CaseInsensitive)) {
            QSvgRenderer *renderer = new QSvgRenderer(itemFileName);
            GraphicsSvgItem *item = new GraphicsSvgItem;
            item->setSharedRenderer(renderer);
            item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemSendsGeometryChanges);
            item->fileName = itemFileName;
            int hc = item->boundingRect().center().x();
            int vc = item->boundingRect().center().y();
            item->setPos(-(hc - 5), -(vc - 5));
            ++childId;
            item->ParentId = 0;
            item->ChildId = childId;
            item->IsLocked = false;
            ++z;
            item->setZValue(z);

            PivotItem *itempivot = new PivotItem();
            itempivot->setRect(0, 0, 10, 10);
            itempivot->setPos(background->pos().x() + (background->boundingRect().width() / 2) - 5, background->pos().y() + (background->boundingRect().height()  / 2) - 5);
            itempivot->setOpacity(0.0);
            itempivot->ParentId = item->parentId();
            itempivot->ChildId = item->childId();
            itempivot->IsLocked = false;
            scene->addItem(itempivot);
            itempivot->setZValue(z);
            item->setParentItem(itempivot);
            item->setSelected(true);
            isSaved = false;
            isSceneEmpty = false;
        }
    }
}

void CutoutDialog::removeImage()
{
    QList<QGraphicsItem *> selected = scene->selectedItems();
    if(selected.count() > 0)
    {
        if(scene->selectedItems().first()->type() == GraphicsPixmapItem::Type) {
            GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
            if(!myItem->isLocked()) {
                scene->removeItem(selected.first()->parentItem());
                isSaved = false;
            }
        }
        if(scene->selectedItems().first()->type() == GraphicsSvgItem::Type) {
            GraphicsSvgItem *myItem = (GraphicsSvgItem *)scene->selectedItems().first();
            if(!myItem->isLocked()) {
                scene->removeItem(selected.first()->parentItem());
                isSaved = false;
            }
        }
        if(scene->items().count() > 0) {
            for(int i = 0; i < scene->items().count(); i++) {
                if(scene->items().at(i)->type() == GraphicsPixmapItem::Type || scene->items().at(i)->type() == GraphicsSvgItem::Type) {
                    isSceneEmpty = false;
                    break;
                } else {
                    isSceneEmpty = true;
                }
            }
        }
    }
}

void CutoutDialog::loadLast()
{
    if(maybeSave()) {
        settings = new QSettings;
        fileName = settings->value("LastCutoutFile", "").toString();
        load();     
    }
}

void CutoutDialog::saveAs()
{
    fileName = "";
    save();
}

void CutoutDialog::save()
{
    framePixmapFileNames.clear();
    pixmapItemFileNames.clear();
    if(fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(
            this,
            "Save File",
            QDir::homePath(),
            "OINUT Cutout Files (*.oic *.OIC)");
    }
    if(!fileName.isEmpty()) {
        if(!fileName.endsWith(".oic", Qt::CaseInsensitive)) {
            fileName += ".oic";
        }
        settings = new QSettings;
        settings->setValue("LastCutoutFile", fileName);
        isSaved = true;
        QDir dir(tmpDir);
        makePath();
        QDir fileDir(path);
        if(fileDir.exists()) {
            fileDir.removeRecursively();
        }
        dir.mkpath(path);
        QString sceneDataFileName = path + "/SceneData.xml";
        QFile sceneData(sceneDataFileName);
        if (!sceneData.open(QIODevice::WriteOnly))
        {
            return;
        }
        QXmlStreamWriter xmlWriter(&sceneData);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeDTD("<!DOCTYPE SceneData>");
        xmlWriter.writeStartElement("SceneData");
        xmlWriter.writeAttribute("version", "v1.0");
        xmlWriter.writeStartElement("FileType");
        xmlWriter.writeAttribute("extension", "oic");
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("DocumentSize");
        xmlWriter.writeAttribute("width", QString::number(w));
        xmlWriter.writeAttribute("height", QString::number(h));
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("ItemList");
        for(int i = 0; i < pixmapItems.count(); i++)
        {
            QPixmap pixmap(w, h);
            std::stringstream ss;
            ss << std::setw(4) << std::setfill('0') << (i + 1);
            QString frameNumberStr = QString(ss.str().c_str());
            QString pixmapFile = "pixmap" + frameNumberStr +".png";
            pixmapItemFileNames.append(pixmapFile);
            pixmap = pixmapItems.at(i)->pixmap();
            pixmap.save(path + "/" + pixmapFile);
        }
        for(int l = 0; l < framePixmaps.count(); l++)
        {
            QPixmap pixmap(w, h);
            std::stringstream ss;
            ss << std::setw(4) << std::setfill('0') << (l + 1);
            QString frameNumberStr = QString(ss.str().c_str());
            QString frameFile = "frame" + frameNumberStr +".png";
            framePixmapFileNames.append(frameFile);
            pixmap = framePixmaps.at(l);
            pixmap.save(path + "/" + frameFile);
        }
        int itemCount = 0;
        foreach( QGraphicsItem* item, scene->items())
        {
            if( item->type() == PivotItem::Type )
            {
                PivotItem* myItem = (PivotItem*)item;
                xmlWriter.writeStartElement("PivotItem");
                xmlWriter.writeAttribute("xCoord", QString::number(myItem->pos().x()));
                xmlWriter.writeAttribute("yCoord", QString::number(myItem->pos().y()));
                xmlWriter.writeAttribute("zvalue", QString::number(myItem->zValue()));
                xmlWriter.writeAttribute("rotation", QString::number(myItem->rotation()));
                xmlWriter.writeAttribute("parentId", QString::number(myItem->parentId()));
                xmlWriter.writeAttribute("childId", QString::number(myItem->childId()));
                xmlWriter.writeAttribute("isLocked", QString::number(myItem->isLocked()));
                if(item->flags().testFlag(QGraphicsItem::ItemStacksBehindParent)) {
                    xmlWriter.writeAttribute("stacksBehindParent", "true");
                } else {
                    xmlWriter.writeAttribute("stacksBehindParent", "false");
                }
                xmlWriter.writeEndElement();
                itemCount++;
                std::stringstream is;
                is << std::setw(4) << std::setfill('0') << itemCount;
                QString itemNumber = QString(is.str().c_str());
                QGraphicsItem *childItem = myItem->childItems().first();
                if(childItem->type() == GraphicsPixmapItem::Type) {
                    QString itemName = "item" + itemNumber + ".png";

                    GraphicsPixmapItem* mychildItem = (GraphicsPixmapItem*)childItem;
                    xmlWriter.writeStartElement("GraphicsPixmapItem");
                    xmlWriter.writeAttribute("xCoord", QString::number(mychildItem->pos().x()));
                    xmlWriter.writeAttribute("yCoord", QString::number(mychildItem->pos().y()));
                    xmlWriter.writeAttribute("zvalue", QString::number(mychildItem->zValue()));
                    xmlWriter.writeAttribute("parentId", QString::number(mychildItem->parentId()));
                    xmlWriter.writeAttribute("childId", QString::number(mychildItem->childId()));
                    xmlWriter.writeAttribute("isLocked", QString::number(mychildItem->isLocked()));
                    xmlWriter.writeAttribute("scale", QString::number(mychildItem->scale()));

                    xmlWriter.writeAttribute("fileName", itemName);
                    QPixmap pixmap(w, h);
                    pixmap = mychildItem->pixmap();
                    pixmap.save(path + "/" + itemName);

                    mychildItem->fileName = itemName;
                    xmlWriter.writeEndElement();
                }
                if(childItem->type() == GraphicsSvgItem::Type) {
                    QString itemName = "item" + itemNumber + ".svg";

                    GraphicsSvgItem* mychildItem = (GraphicsSvgItem*)childItem;
                    xmlWriter.writeStartElement("GraphicsSvgItem");
                    xmlWriter.writeAttribute("xCoord", QString::number(mychildItem->pos().x()));
                    xmlWriter.writeAttribute("yCoord", QString::number(mychildItem->pos().y()));
                    xmlWriter.writeAttribute("zvalue", QString::number(mychildItem->zValue()));
                    xmlWriter.writeAttribute("parentId", QString::number(mychildItem->parentId()));
                    xmlWriter.writeAttribute("childId", QString::number(mychildItem->childId()));
                    xmlWriter.writeAttribute("isLocked", QString::number(mychildItem->isLocked()));
                    xmlWriter.writeAttribute("scale", QString::number(mychildItem->scale()));

                    xmlWriter.writeAttribute("fileName", itemName);
                    QSvgGenerator svgGen;
                    svgGen.setFileName(path + "/" + itemName);
                    QPainter painter(&svgGen);
                    QStyleOptionGraphicsItem opt;
                    mychildItem->paint(&painter, &opt);

                    mychildItem->fileName = itemName;
                    xmlWriter.writeEndElement();
                }

            }
        }

        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        sceneData.close();
        QuaZip zip(fileName);
        zip.setFileNameCodec("IBM866");
        if(!zip.open(QuaZip::mdCreate)) {
            return;
        }
        QDir zipDir(path);
        QFile inFile;
        QuaZipFile outFile(&zip);
        QStringList sl;
        QFileInfoList files;
        char c;
        QStringList qsl = zipDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs| QDir::Files);
        foreach (QString file, qsl) {
            QFileInfo finfo(QString("%1/%2").arg(zipDir.path()).arg(file));
            if (finfo.isDir()) {
                QDir sd(finfo.filePath());
                QStringList qslBelow = sd.entryList(QDir::NoDotAndDotDot | QDir::Files);
                foreach (QString fileBelow, qslBelow) {
                    QFileInfo fbinfo(QString("%1/%2").arg(sd.path()).arg(fileBelow));
    
                    sl << QDir::toNativeSeparators(fbinfo.filePath());
                }
            } else {
                sl << QDir::toNativeSeparators(finfo.filePath());
            }
        }
        foreach(QString fn, sl) files << QFileInfo(fn);
        foreach(QFileInfo fileInfo, files) {
            if(!fileInfo.isFile())
                continue;
            QString fileNameWithRelativePath = fileInfo.filePath().remove(0, zipDir.absolutePath().length() + 1);
            inFile.setFileName(fileInfo.filePath());
            if(!inFile.open(QIODevice::ReadOnly)) {
                return;
            }
            if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileNameWithRelativePath, fileInfo.filePath()))) {
                return;
            }
    
            while(inFile.getChar(&c) && outFile.putChar(c));
    
            outFile.close();
            inFile.close();
        }
    }
}

void CutoutDialog::load()
{
    if (!fileName.isEmpty()) {
        if(fileName.endsWith("oic", Qt::CaseInsensitive)) {
            makeTmpName();
            framePixmaps.clear();
            pixmapItems.clear();
            QList<QGraphicsItem *> pixmapList;
            settings = new QSettings;
            isSaved = true;
            isSceneEmpty =  true;
            settings->setValue("LastCutoutFile", fileName);
            QDir extractDir(tmpDir);
            makePath();
            extractDir.mkpath(path);
            QuaZip zip(fileName);
            zip.open(QuaZip::mdUnzip);
            QuaZipFile file(&zip);
	    if(zip.getZipError() != UNZ_OK)
	    {
                QMessageBox::warning(this, tr("Oinut"),
                    tr("File seems to be corrupt."),
                    QMessageBox::Ok);

                return;
	    }
            for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
                file.open(QIODevice::ReadOnly);
                QFile writtenFile(path + "/" + zip.getCurrentFileName());
                writtenFile.open(QIODevice::WriteOnly);
                writtenFile.write(file.readAll());
                writtenFile.close();
                file.close();
            }
            zip.close();

            PivotItem *item = new PivotItem();
            GraphicsPixmapItem *itemPixmap = new GraphicsPixmapItem();
            GraphicsSvgItem *itemSvg = new GraphicsSvgItem();

            QFile sceneData(path + "/SceneData.xml");
            QTransform transform;
            if (!sceneData.open(QIODevice::ReadOnly))
            {
                QMessageBox::warning(this, tr("Oinut"),
                    tr("File seems to be corrupt."),
                    QMessageBox::Ok);
                return;
            }
            QXmlStreamReader xmlReader(&sceneData);
            while(!xmlReader.atEnd())
            {
                xmlReader.readNext();
                if(xmlReader.isStartElement()) {
                    if(xmlReader.name().toString() == "FileType") {
                        if(xmlReader.attributes().value("extension").toString() != "oic") {
                            QMessageBox::warning(this, tr("Oinut"), tr("File is not a Oinut Cutout file."), QMessageBox::Ok);
                            return;
                        }
                    }
                    if(xmlReader.name().toString() == "DocumentSize") {
                        enableActions();
                        ui->actionOnion3->setIcon(QIcon(":images/onion3Selected.svg"));
                        ui->actionMove->setIcon(QIcon(":images/moveSelected.svg"));


                        w = xmlReader.attributes().value("width").toString().toInt();
                        h = xmlReader.attributes().value("height").toString().toInt();
                        if(!isFirstRun) {
                            delete graphicsView;
                        }
                        scene->clear();
    
                        scene->setSceneRect(0, 0, 3000, 3000);
                        graphicsView = new QGraphicsView;
                        graphicsView->setScene(scene);
                        graphicsView->setSceneRect(0, 0, 3000, 3000);
                        graphicsView->setRenderHint(QPainter::Antialiasing);
                        graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                        graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                        graphicsView->viewport()->setAutoFillBackground(false);
                        graphicsView->setAlignment(Qt::AlignCenter);
                        graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
                        graphicsView->setAttribute(Qt::WA_TranslucentBackground, true);
                        graphicsView->setAutoFillBackground(false);

                        QPixmap backgroundPix(w, h);
                        backgroundPix.fill(Qt::white);
                        background = new QGraphicsPixmapItem;
                        background->setZValue(-2000);
                        background->setPixmap(backgroundPix);
                        background->setPos((scene->sceneRect().center().x()) - (w / 2), (scene->sceneRect().center().y()) - (h / 2));

                        ui->viewLayout->addWidget(graphicsView);
                     }
                    if(xmlReader.name().toString() == "PivotItem") {
                        item = new PivotItem();
                        qreal xCoord = xmlReader.attributes().value("xCoord").toString().toDouble();
                        qreal yCoord = xmlReader.attributes().value("yCoord").toString().toDouble();
                        int zvalue = xmlReader.attributes().value("zvalue").toString().toInt();
                        qreal rotate = xmlReader.attributes().value("rotation").toString().toDouble();
                        QString stacksBehindParent = xmlReader.attributes().value("stacksBehindParent").toString();
                        int itemParentId = xmlReader.attributes().value("parentId").toString().toInt();
                        int itemChildId = xmlReader.attributes().value("childId").toString().toInt();
                        QString itemIsLocked = xmlReader.attributes().value("isLocked").toString();
                        item->setRect(0, 0, 10, 10);
                        item->setTransformOriginPoint(5, 5);
                        item->setRotation(rotate);
                        item->setPos(xCoord, yCoord);
                        item->setOpacity(0.0);
                        item->setBrush(QBrush(Qt::white));
                        item->setZValue(zvalue);
                        if(itemIsLocked == "1") {
                            item->IsLocked = true;
                        } else {
                            item->IsLocked = false;
                        }
                        if(stacksBehindParent == "true") {
                            item->setFlags(QGraphicsItem::ItemIsMovable| QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemStacksBehindParent);
                        } else {
                            item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
                        }
                        item->ParentId = itemParentId;
                        item->ChildId = itemChildId;
                        scene->addItem(item);
                    }
                    if(xmlReader.name().toString() == "GraphicsPixmapItem") {
                        itemPixmap = new GraphicsPixmapItem();
                        qreal xPixmapCoord = xmlReader.attributes().value("xCoord").toString().toDouble();
                        qreal yPixmapCoord = xmlReader.attributes().value("yCoord").toString().toDouble();
                        int pixmapzvalue = xmlReader.attributes().value("zvalue").toString().toInt();
                        int itemParentId = xmlReader.attributes().value("parentId").toString().toInt();
                        int itemChildId = xmlReader.attributes().value("childId").toString().toInt();
                         QString childItemIsLocked = xmlReader.attributes().value("isLocked").toString();
                        qreal childItemScale = xmlReader.attributes().value("scale").toString().toDouble();
    
                        QString pixmapFileName = xmlReader.attributes().value("fileName").toString();
                        pixmapFileName = path + "/" + pixmapFileName;
                        QPixmap pixmap;
                        pixmap.load(pixmapFileName);
                        itemPixmap->setPixmap(pixmap);
                        itemPixmap->setPos(xPixmapCoord, yPixmapCoord);
                        itemPixmap->fileName = pixmapFileName;
                        itemPixmap->setZValue(pixmapzvalue);
                        itemPixmap->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemSendsGeometryChanges);
                        itemPixmap->ParentId = itemParentId;
                        itemPixmap->ChildId = itemChildId;
                        if(childItemIsLocked == "1") {
                            itemPixmap->IsLocked = true;
                        } else {
                            itemPixmap->IsLocked = false;
                        }
                        itemPixmap->setScale(childItemScale);
                        scene->addItem(itemPixmap);
                        isSceneEmpty = false;
                        itemPixmap->setParentItem(item);
                        pixmapList.append(itemPixmap);
                    }
                    if(xmlReader.name().toString() == "GraphicsSvgItem") {
                        itemSvg = new GraphicsSvgItem();
                        qreal xPixmapCoord = xmlReader.attributes().value("xCoord").toString().toDouble();
                        qreal yPixmapCoord = xmlReader.attributes().value("yCoord").toString().toDouble();
                        int pixmapzvalue = xmlReader.attributes().value("zvalue").toString().toInt();
                        int itemParentId = xmlReader.attributes().value("parentId").toString().toInt();
                        int itemChildId = xmlReader.attributes().value("childId").toString().toInt();
                         QString childItemIsLocked = xmlReader.attributes().value("isLocked").toString();
                        qreal childItemScale = xmlReader.attributes().value("scale").toString().toDouble();
    
                        QString pixmapFileName = xmlReader.attributes().value("fileName").toString();
                        pixmapFileName = path + "/" + pixmapFileName;
                        QSvgRenderer *renderer = new QSvgRenderer(pixmapFileName);
                        itemSvg->setSharedRenderer(renderer);
                        itemSvg->setPos(xPixmapCoord, yPixmapCoord);
                        itemSvg->fileName = pixmapFileName;
                        itemSvg->setZValue(pixmapzvalue);
                        itemSvg->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemSendsGeometryChanges);
                        itemSvg->ParentId = itemParentId;
                        itemSvg->ChildId = itemChildId;
                        if(childItemIsLocked == "1") {
                            itemSvg->IsLocked = true;
                        } else {
                            itemSvg->IsLocked = false;
                        }
                        itemSvg->setScale(childItemScale);
                        scene->addItem(itemSvg);
                        isSceneEmpty = false;
                        itemSvg->setParentItem(item);
                        pixmapList.append(itemSvg);
                    }
                }    
            }
            sceneData.close();
            QDir fileDir(path);
            QStringList fileList = fileDir.entryList();
            for(int i = 0; i < fileList.count(); i++) {
                if(fileList.at(i).startsWith("pixmap")) {
                    QPixmap itemPixmap = QPixmap(w, h);;
                    itemPixmap.load(path + "/" + fileList.at(i));
                    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem;
                    pixmapItem->setPixmap(itemPixmap);
                    pixmapItem->setZValue(-1000);
                    pixmapItem->setPos(background->pos());
                    pixmapItems.append(pixmapItem);
                }
                if(fileList.at(i).startsWith("frame")) {
                    QPixmap framePixmap = QPixmap(w, h);
                    framePixmap.load(path + "/" + fileList.at(i));
                    framePixmaps.append(framePixmap);
                }
            }
            for(int i = 0; i < scene->items().count(); i++)
            {
                if(z < scene->items().at(i)->zValue()) {
                    z = scene->items().at(i)->zValue();
                }
            }
            ++z;
            for(int i = 0; i < pixmapList.count(); i++)
            {
                GraphicsPixmapItem *tmpPix = (GraphicsPixmapItem *)pixmapList.at(i);
                if(childId < tmpPix->childId()) {
                    childId = tmpPix->childId();
                }
            }
            ++childId;
            for(int i = 0; i < pixmapList.count() - 1; i++)
            {
                for(int l = 0; l < pixmapList.count() - i - 1; l++)
                {
                    GraphicsPixmapItem *tmpPix = (GraphicsPixmapItem *)pixmapList.at(l);
                    GraphicsPixmapItem *nextPix = (GraphicsPixmapItem *)pixmapList.at(l+1);
                    if(tmpPix->childId() < nextPix->childId())
                    {
                        GraphicsPixmapItem *tmpPixmap;
                        tmpPixmap = tmpPix;
                        pixmapList.replace(l, nextPix);
                        pixmapList.replace(l+1, tmpPixmap);
                    }
                 }
            }
            for(int i = 0; i < pixmapList.count(); i++)
            {
                for(int l = 0; l < pixmapList.count(); l++)
                {
                    GraphicsPixmapItem *childPix = (GraphicsPixmapItem *)pixmapList.at(i);
                    GraphicsPixmapItem *parentPix = (GraphicsPixmapItem *)pixmapList.at(l);
                    if(childPix->parentId() == parentPix->childId())
                    {
                        pixmapList.at(i)->parentItem()->setParentItem(pixmapList.at(l));
                    }
                }
            }
            if(isOnion) {
                for(int i = 0; i < pixmapItems.count(); i++)
                {
                        if(i == pixmapItems.count() - 3) {
                            scene->addItem(pixmapItems.at(i));
                            pixmapItems.at(i)->setOpacity(0.2);
                        }
                        if(i == pixmapItems.count() - 2) {
                            scene->addItem(pixmapItems.at(i));
                            pixmapItems.at(i)->setOpacity(0.4);
                        }
                        if(i == pixmapItems.count() - 1) {
                            scene->addItem(pixmapItems.at(i));
                            pixmapItems.at(i)->setOpacity(0.8);
                        }
                }
            }
            for(int c = 0; c < scene->items().count(); c++)
            {
                if(scene->items().at(c)->type() == PivotItem::Type && scene->items().at(c)->parentItem() != 0) {
                    scene->items().at(c)->setScale(1.0 / scene->items().at(c)->parentItem()->scale());
                }
            }
            isFirstRun = false;
            isOnion = false;
            setModeMove();
            onion3();
        } else {
            QMessageBox::warning(this, tr("Oinut"),
                    tr("File is not a Oinut Cutout file."),
                    QMessageBox::Ok);
        }

    }
}

void CutoutDialog::getFileName()
{
    fileName = QFileDialog::getOpenFileName(
                this,
                "Select a file to open",
                QDir::homePath(),
                "OINUT Cutout Files (*.oic *.OIC)");
}

void CutoutDialog::open()
{
    if(maybeSave()) {
        if(!fileName.isEmpty()) {
            makePath();
            QDir dir(path);
            dir.removeRecursively();
        }
        getFileName();
        if(!fileName.isEmpty()) {
            load();
        }
    }
}

void CutoutDialog::restoreItemEdit()
{
    if(scene->selectedItems().count() > 0)
    {
        GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(!myItem->isLocked()) {
            qreal hr = (scene->selectedItems().first()->boundingRect().width() / 2) - 5;
            qreal vr = (scene->selectedItems().first()->boundingRect().height() / 2) - 5;
            scene->selectedItems().first()->setPos(-hr, -vr);
            isSaved = false;
        }
    }
}

void CutoutDialog::restoreItemRotate()
{
    if(scene->selectedItems().count() > 0)
    {
        GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(!myItem->isLocked()) {
            qreal hc = scene->selectedItems().first()->parentItem()->boundingRect().center().x();
            qreal vc = scene->selectedItems().first()->parentItem()->boundingRect().center().y();
            scene->selectedItems().first()->parentItem()->setTransformOriginPoint(hc, vc);
            scene->selectedItems().first()->parentItem()->setRotation(0);
            isSaved = false;
        }
    }
}

void CutoutDialog::restoreItemScale()
{
    if(scene->selectedItems().count() > 0)
    {
        GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(!myItem->isLocked()) {
            if(scene->selectedItems().first()->parentItem()->parentItem() == 0 ) {
                qreal ps =scene->selectedItems().first()->scale();
                qreal iw = scene->selectedItems().first()->boundingRect().width();
                qreal ih = scene->selectedItems().first()->boundingRect().height();
                qreal dw;
                qreal dh;
                qreal ds;
                if(ps >= 1.0) {
                    ds = ps - 1.0;
                    dw = (iw / 2) * (ds);
                    dh = (ih / 2) * (ds);
                } else {
                    ds = 1.0 - ps;
                    dw = -((iw / 2) * (ds));
                    dh = -((ih / 2) * (ds));
                }
                scene->selectedItems().first()->moveBy(dw, dh);
                scene->selectedItems().first()->setScale(1.0);
                if(scene->selectedItems().first()->childItems().count() > 0) {
                    for(int i = 0; i < scene->selectedItems().first()->childItems().count(); i++) {
                        scene->selectedItems().first()->childItems().at(i)->setScale(1.0);
                    }
                }
            } else {
                qreal ps = scene->selectedItems().first()->parentItem()->parentItem()->scale();
                qreal es = scene->selectedItems().first()->parentItem()->scale();
                qreal cs = scene->selectedItems().first()->scale();
                qreal iw = scene->selectedItems().first()->boundingRect().width();
                qreal ih = scene->selectedItems().first()->boundingRect().height();
                qreal ds;
                qreal dw;
                qreal dh;
                if(cs >= 1.0) {
                    ds = cs - 1.0;
                    dw = (iw / 2) * ds;
                    dh = (ih / 2) * ds;
                } else {
                    ds = 1.0 - cs;
                    dw = -((iw / 2) * ds);
                    dh = -((ih / 2) * ds);
                }
                scene->selectedItems().first()->moveBy(dw, dh);
                scene->selectedItems().first()->setScale(ps * es);
                if(scene->selectedItems().first()->childItems().count() > 0) {
                    for(int i = 0; i < scene->selectedItems().first()->childItems().count(); i++) {
                        scene->selectedItems().first()->childItems().at(i)->setScale(1.0);
                    }
                }
            }
            isSaved = false;
        }
    }
}

void CutoutDialog::sendBelowParent()
{
    if(scene->selectedItems().count() > 0) {
        GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(!myItem->isLocked()) {
            scene->selectedItems().first()->parentItem()->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemStacksBehindParent);
            isSaved = false;
        }
    }
}

void CutoutDialog::sendAboveParent()
{
    if(scene->selectedItems().count() > 0) {
        GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
        if(!myItem->isLocked()) {
            scene->selectedItems().first()->parentItem()->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
            isSaved = false;
        }
    }
}

void CutoutDialog::closeEvent(QCloseEvent *event)
{
    if(maybeSave()) {
        makePath();
        QDir oldDir(path);
        if(oldDir.exists()) {
            oldDir.removeRecursively();
        }
        win->enableActions();
        event->accept();
    } else {
        event->ignore();
    }
}

bool CutoutDialog::maybeSave()
{
    if (!isSaved) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Oinut"),
                    tr("The document hasn't been saved.\n"
                    "Do you want to save your changes?"),
                    QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard);
        if (ret == QMessageBox::Save)
            save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void CutoutDialog::quitApp()
{
    if(maybeSave()) {
        makePath();
        QDir oldDir(path);
        if(oldDir.exists()) {
            oldDir.removeRecursively();
        }
        win->enableActions();
        close();
    }
}

void CutoutDialog::toggleLock()
{
    if(scene->selectedItems().count() > 0)
    {
        GraphicsPixmapItem *theItem = (GraphicsPixmapItem *) scene->selectedItems().first();
        PivotItem *theParentItem = (PivotItem *)theItem->parentItem();
        if(theItem->isLocked())
        {
            theItem->IsLocked = false;
            theParentItem->IsLocked = false;
            ui->actionLock->setIcon(QIcon(":images/lock.svg"));
        } else {
            theItem->IsLocked = true;
            theParentItem->IsLocked = true;
            ui->actionLock->setIcon(QIcon(":images/lockSelected.svg"));
        }
    isSaved = false;
    }
}

void CutoutDialog::changeImage()
{
    if(scene->selectedItems().count() > 0) {
        if(scene->selectedItems().first()->type() == GraphicsPixmapItem::Type) {
            GraphicsPixmapItem *myItem = (GraphicsPixmapItem *)scene->selectedItems().first();
            if(!myItem->isLocked()) {
                QString fileName = QFileDialog::getOpenFileName(
                    this,
                    "Select a file to open",
                    QDir::homePath(),
                    "Images (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG)");
                if (!fileName.isEmpty())
                {
                    QPixmap itemPixmap;
                    itemPixmap.load(fileName);
                    myItem->setPixmap(itemPixmap);
                    myItem->fileName = fileName;
                    int hc = myItem->boundingRect().center().x();
                    int vc = myItem->boundingRect().center().y();
                    myItem->setPos(-(hc - 5), -(vc - 5));
                }
                isSaved = false;
            }
        } else {
            GraphicsSvgItem *myItem = (GraphicsSvgItem *)scene->selectedItems().first();
            if(!myItem->isLocked()) {
                QString fileName = QFileDialog::getOpenFileName(
                    this,
                    "Select a file to open",
                    QDir::homePath(),
                    "Scalable Vector Graphics (*.svg *.SVG)");
                if (!fileName.isEmpty())
                {
                    QSvgRenderer *renderer = new QSvgRenderer(fileName);
                    myItem->setSharedRenderer(renderer);
                    myItem->fileName = fileName;
                    int hc = myItem->boundingRect().center().x();
                    int vc = myItem->boundingRect().center().y();
                    myItem->setPos(-(hc - 5), -(vc - 5));
                }
                isSaved = false;
            }
        }
    }
}

void CutoutDialog::escapeClicked()
{
    if(!isFirstRun) {
    if(isSetParent) {
        setParent();
    } else if(mode == "rotate" || mode == "edit" || mode == "rotate15" || mode == "scale") {
        setModeMove();
    } else if(scene->selectedItems().count() > 0) {
        foreach(QGraphicsItem *item, scene->selectedItems()) {
            item->setSelected(false);
        }
    }
    }
}

void CutoutDialog::onion3()
{
    sceneItems = scene->items();
    restoreSelected = scene->selectedItems();
        if(background->scene() == scene)
            scene->removeItem(background);
        if(frameShow->scene() == scene)
            scene->removeItem(frameShow);
        for(int i = 0; i < pixmapItems.count(); i++)
            if(pixmapItems.at(i)->scene() == scene)
                scene->removeItem(pixmapItems.at(i));
        for(int i = 0; i < scene->items().count(); i++)
            if(scene->items().at(i)->scene() == scene && scene->items().at(i)->parentItem() == 0)
                scene->removeItem(scene->items().at(i));
    if(!isOnion) {
        ui->actionOnion3->setIcon(QIcon(":images/onion3Selected.svg"));
        isOnion = true;
    } else {
        isOnion = false;
        ui->actionOnion3->setIcon(QIcon(":images/onion3.svg"));
    }
    restoreWorkspace();
}

void CutoutDialog::enableActions()
{
    ui->actionAddImage->setDisabled(false);
    ui->actionChangeImage->setDisabled(false);
    ui->actionDeleteImage->setDisabled(false);
    ui->actionEditPivotPoint->setDisabled(false);
    ui->actionLock->setDisabled(false);
    ui->actionMove->setDisabled(false);
    ui->actionOnion3->setDisabled(false);
    ui->actionRestoreEdit->setDisabled(false);
    ui->actionRestoreRotate->setDisabled(false);
    ui->actionRestoreScale->setDisabled(false);
    ui->actionRotate->setDisabled(false);
    ui->actionRotate15->setDisabled(false);
    ui->actionSave->setDisabled(false);
    ui->actionSaveAs->setDisabled(false);
    ui->actionScale->setDisabled(false);
    ui->actionSendAboveParent->setDisabled(false);
    ui->actionSendBelowParent->setDisabled(false);
    ui->actionSendUp->setDisabled(false);
    ui->actionSendDown->setDisabled(false);
    ui->actionAddFrame->setDisabled(false);
    ui->actionInsertFrame->setDisabled(false);
    ui->actionReplaceFrame->setDisabled(false);
    ui->actionSetParent->setDisabled(false);
    ui->actionUnsetParent->setDisabled(false);
    ui->actionZoomIn->setDisabled(false);
    ui->actionZoomOut->setDisabled(false);
    ui->actionDragMode->setDisabled(false);
    ui->actionImportOinut->setDisabled(false);
    ui->actionGrid->setDisabled(false);
    ui->actionExportSelected->setDisabled(false);
}

void CutoutDialog::makeTmpName()
{
    if(!tmpname.isEmpty()) {
        makePath();
        QDir oldDir(path);
        if(oldDir.exists())
            oldDir.removeRecursively();
    }
    std::stringstream rns;
    rns << "tmp." << time(NULL) << "." << rand();
    tmpname = QString(rns.str().c_str());
}

void CutoutDialog::makePath()
{
    path = tmpDir + tmpname;
}

void CutoutDialog::zoomIn()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor += 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void CutoutDialog::zoomOut()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor -= 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void CutoutDialog::toggleDragMode()
{
        if(graphicsView->dragMode() == QGraphicsView::ScrollHandDrag) {
                graphicsView->setDragMode(QGraphicsView::NoDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragMode.svg"));
                for(int i = 0; i < scene->items().count(); i++) {
                    if(scene->items().at(i)->type() == GraphicsPixmapItem::Type || scene->items().at(i)->type() == GraphicsSvgItem::Type) {
                        scene->items().at(i)->setFlag(QGraphicsItem::ItemIsMovable, true);
                        scene->items().at(i)->setFlag(QGraphicsItem::ItemIsSelectable, true);
                    }
                }
        } else {
                graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragModeSelected.svg"));
                for(int i = 0; i < scene->items().count(); i++) {
                    if(scene->items().at(i)->type() == GraphicsPixmapItem::Type || scene->items().at(i)->type() == GraphicsSvgItem::Type) {
                        scene->items().at(i)->setFlag(QGraphicsItem::ItemIsMovable, false);
                        scene->items().at(i)->setFlag(QGraphicsItem::ItemIsSelectable, false);
                    }
                }
        }
}

void CutoutDialog::importOinut()
{
    getFileName();
    if(!fileName.isEmpty()) {
        if(fileName.endsWith("oic", Qt::CaseInsensitive)) {
            isSaved = false;
            isSceneEmpty =  false;
            childIdCount = childIdCount + 100;
            QList<QGraphicsItem *> pixmapList;
            pixmapList.clear();
            std::stringstream rns;
            rns << "tmp." << time(NULL) << "." << rand();
            QString tmpImportName = QString(rns.str().c_str());
            QString importPath = tmpDir + tmpImportName;
            QDir extractDir(tmpDir);
            extractDir.mkpath(importPath);
            QuaZip zip(fileName);
            zip.open(QuaZip::mdUnzip);
            QuaZipFile file(&zip);
	    if(zip.getZipError() != UNZ_OK)
	    {
                QMessageBox::warning(this, tr("Oinut"),
                    tr("File seems to be corrupt."),
                    QMessageBox::Ok);

                return;
	    }
            for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
                file.open(QIODevice::ReadOnly);
                QFile writtenFile(importPath + "/" + zip.getCurrentFileName());
                writtenFile.open(QIODevice::WriteOnly);
                writtenFile.write(file.readAll());
                writtenFile.close();
                file.close();
            }
            zip.close();

            PivotItem *item = new PivotItem();
            GraphicsPixmapItem *itemPixmap = new GraphicsPixmapItem();
            GraphicsSvgItem *itemSvg = new GraphicsSvgItem();

            QFile sceneData(importPath + "/SceneData.xml");
            QTransform transform;
            if (!sceneData.open(QIODevice::ReadOnly))
            {
                QMessageBox::warning(this, tr("Oinut"),
                    tr("File seems to be corrupt."),
                    QMessageBox::Ok);
                return;
            }
            QXmlStreamReader xmlReader(&sceneData);
            while(!xmlReader.atEnd())
            {
                xmlReader.readNext();
                if(xmlReader.isStartElement()) {
                    if(xmlReader.name().toString() == "FileType") {
                        if(xmlReader.attributes().value("extension").toString() != "oic") {
                            QMessageBox::warning(this, tr("Oinut"), tr("File is not a Oinut Cutout file."), QMessageBox::Ok);
                            return;
                        }
                    }
                    if(xmlReader.name().toString() == "PivotItem") {
                        item = new PivotItem();
                        qreal xCoord = xmlReader.attributes().value("xCoord").toString().toDouble();
                        qreal yCoord = xmlReader.attributes().value("yCoord").toString().toDouble();
                        int zvalue = xmlReader.attributes().value("zvalue").toString().toInt();
                        qreal rotate = xmlReader.attributes().value("rotation").toString().toDouble();
                        QString stacksBehindParent = xmlReader.attributes().value("stacksBehindParent").toString();
                        int itemParentId = xmlReader.attributes().value("parentId").toString().toInt();
                        int itemChildId = xmlReader.attributes().value("childId").toString().toInt();
                        QString itemIsLocked = xmlReader.attributes().value("isLocked").toString();
                        item->setRect(0, 0, 10, 10);
                        item->setTransformOriginPoint(5, 5);
                        item->setRotation(rotate);
                        item->setPos(xCoord, yCoord);
                        item->setOpacity(0.0);
                        item->setBrush(QBrush(Qt::white));
                        item->setZValue(zvalue + childIdCount);
                        if(itemIsLocked == "1") {
                            item->IsLocked = true;
                        } else {
                            item->IsLocked = false;
                        }
                        if(stacksBehindParent == "true") {
                            item->setFlags(QGraphicsItem::ItemIsMovable| QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemStacksBehindParent);
                        } else {
                            item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
                        }
                        item->ParentId = itemParentId + childIdCount;
                        item->ChildId = itemChildId + childIdCount;
                        scene->addItem(item);
                    }
                    if(xmlReader.name().toString() == "GraphicsPixmapItem") {
                        itemPixmap = new GraphicsPixmapItem();
                        qreal xPixmapCoord = xmlReader.attributes().value("xCoord").toString().toDouble();
                        qreal yPixmapCoord = xmlReader.attributes().value("yCoord").toString().toDouble();
                        int pixmapzvalue = xmlReader.attributes().value("zvalue").toString().toInt();
                        int itemParentId = xmlReader.attributes().value("parentId").toString().toInt();
                        int itemChildId = xmlReader.attributes().value("childId").toString().toInt();
                         QString childItemIsLocked = xmlReader.attributes().value("isLocked").toString();
                        qreal childItemScale = xmlReader.attributes().value("scale").toString().toDouble();
    
                        QString pixmapFileName = xmlReader.attributes().value("fileName").toString();
                        pixmapFileName = importPath + "/" + pixmapFileName;
                        QPixmap pixmap;
                        pixmap.load(pixmapFileName);
                        itemPixmap->setPixmap(pixmap);
                        itemPixmap->setPos(xPixmapCoord, yPixmapCoord);
                        itemPixmap->fileName = pixmapFileName;
                        itemPixmap->setZValue(pixmapzvalue + childIdCount);
                        itemPixmap->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemSendsGeometryChanges);
                        itemPixmap->ParentId = itemParentId + childIdCount;
                        itemPixmap->ChildId = itemChildId + childIdCount;
                        if(childItemIsLocked == "1") {
                            itemPixmap->IsLocked = true;
                        } else {
                            itemPixmap->IsLocked = false;
                        }
                        itemPixmap->setScale(childItemScale);
                        scene->addItem(itemPixmap);
                        isSceneEmpty = false;
                        itemPixmap->setParentItem(item);
                        pixmapList.append(itemPixmap);
                    }
                    if(xmlReader.name().toString() == "GraphicsSvgItem") {
                        itemSvg = new GraphicsSvgItem();
                        qreal xPixmapCoord = xmlReader.attributes().value("xCoord").toString().toDouble();
                        qreal yPixmapCoord = xmlReader.attributes().value("yCoord").toString().toDouble();
                        int pixmapzvalue = xmlReader.attributes().value("zvalue").toString().toInt();
                        int itemParentId = xmlReader.attributes().value("parentId").toString().toInt();
                        int itemChildId = xmlReader.attributes().value("childId").toString().toInt();
                         QString childItemIsLocked = xmlReader.attributes().value("isLocked").toString();
                        qreal childItemScale = xmlReader.attributes().value("scale").toString().toDouble();
    
                        QString pixmapFileName = xmlReader.attributes().value("fileName").toString();
                        pixmapFileName = importPath + "/" + pixmapFileName;
                        QSvgRenderer *renderer = new QSvgRenderer(pixmapFileName);
                        itemSvg->setSharedRenderer(renderer);
                        itemSvg->setPos(xPixmapCoord, yPixmapCoord);
                        itemSvg->fileName = pixmapFileName;
                        itemSvg->setZValue(pixmapzvalue + childIdCount);
                        itemSvg->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemSendsGeometryChanges);
                        itemSvg->ParentId = itemParentId + childIdCount;
                        itemSvg->ChildId = itemChildId + childIdCount;
                        if(childItemIsLocked == "1") {
                            itemSvg->IsLocked = true;
                        } else {
                            itemSvg->IsLocked = false;
                        }
                        itemSvg->setScale(childItemScale);
                        scene->addItem(itemSvg);
                        isSceneEmpty = false;
                        itemSvg->setParentItem(item);
                        pixmapList.append(itemSvg);
                    }
                }    
            }
            sceneData.close();
            for(int i = 0; i < pixmapList.count() - 1; i++)
            {
                for(int l = 0; l < pixmapList.count() - i - 1; l++)
                {
                    GraphicsPixmapItem *tmpPix = (GraphicsPixmapItem *)pixmapList.at(l);
                    GraphicsPixmapItem *nextPix = (GraphicsPixmapItem *)pixmapList.at(l+1);
                    if(tmpPix->childId() < nextPix->childId())
                    {
                        GraphicsPixmapItem *tmpPixmap;
                        tmpPixmap = tmpPix;
                        pixmapList.replace(l, nextPix);
                        pixmapList.replace(l+1, tmpPixmap);
                    }
                 }
            }
            for(int i = 0; i < pixmapList.count(); i++)
            {
                for(int l = 0; l < pixmapList.count(); l++)
                {
                    GraphicsPixmapItem *childPix = (GraphicsPixmapItem *)pixmapList.at(i);
                    GraphicsPixmapItem *parentPix = (GraphicsPixmapItem *)pixmapList.at(l);
                    if(childPix->parentId() == parentPix->childId())
                    {
                        pixmapList.at(i)->parentItem()->setParentItem(pixmapList.at(l));
                    }
                }
            }
            for(int c = 0; c < scene->items().count(); c++)
            {
                if(scene->items().at(c)->type() == PivotItem::Type && scene->items().at(c)->parentItem() != 0) {
                    scene->items().at(c)->setScale(1.0 / scene->items().at(c)->parentItem()->scale());
                }
            }
            isFirstRun = false;
            QDir oldDir(importPath);
            if(oldDir.exists())
                oldDir.removeRecursively();
        }
    }
}

void CutoutDialog::deleteFrame()
{
    for(int i = 0; i < pixmapItems.count(); i++)
        if(pixmapItems.at(i)->scene() == scene)
            scene->removeItem(pixmapItems.at(i));
    isSaved = false;
    if(framePixmaps.count() > 0) {
        framePixmaps.removeLast();
        pixmapItems.removeLast();
    }
    if(isOnion) {
        for(int i = 0; i < pixmapItems.count(); i++)
        {
                if(i == pixmapItems.count() - 3) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.2);
                }
                if(i == pixmapItems.count() - 2) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.4);
                }
                if(i == pixmapItems.count() - 1) {
                    scene->addItem(pixmapItems.at(i));
                    pixmapItems.at(i)->setOpacity(0.8);
                }
        }
    }
    win->removeLast();
}

void PivotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    setRect(0, 0, 10, 10);
    qreal hc = boundingRect().center().x();
    qreal vc = boundingRect().center().y();
    painter->setBrush(QBrush(QColor(255, 255, 255, 128)));
    painter->drawEllipse(QPoint(hc, vc), 5, 5);
    painter->drawLine(QPointF(5, 0), QPointF(5, 10));
    painter->drawLine(QPointF(0, 5), QPointF(10, 5));
}

void CutoutDialog::selectNext()
{
    if(scene->items().count() > 0)
    {
        QList<GraphicsPixmapItem *> items;
        for(int i = 0; i < scene->items().count(); i++) {
            if(scene->items().at(i)->type() == GraphicsPixmapItem::Type || scene->items().at(i)->type() == GraphicsSvgItem::Type) {
                GraphicsPixmapItem *item = (GraphicsPixmapItem *)scene->items().at(i);
                items.append(item);
            }
        }
        bool swapped = true;
        int j = 0;
        while(swapped) {
            swapped = false;
            j++;
            for(int i = 0; i < items.count() - j; i++) {
                if(items.at(i)->childId() > items.at(i+1)->childId()) {
                    GraphicsPixmapItem *tmpItem = items.at(i);
                    items.replace(i, items.at(i + 1));
                    items.replace(i + 1, tmpItem);
                    swapped = true;
                }
            }
        }
        int currentItem = -1;
        for(int i = 0; i < items.count(); i++) {
            if(items.at(i)->isSelected())
                currentItem = i;
        }
        if(currentItem == items.count() - 1) {
            items.at(0)->setSelected(true);
        } else {
            items.at(currentItem + 1)->setSelected(true);
        }
    }
}

void CutoutDialog::selectPrevious()
{
    if(scene->items().count() > 0)
    {
        QList<GraphicsPixmapItem *> items;
        for(int i = 0; i < scene->items().count(); i++) {
            if(scene->items().at(i)->type() == GraphicsPixmapItem::Type || scene->items().at(i)->type() == GraphicsSvgItem::Type) {
                GraphicsPixmapItem *item = (GraphicsPixmapItem *)scene->items().at(i);
                items.append(item);
            }
        }
        bool swapped = true;
        int j = 0;
        while(swapped) {
            swapped = false;
            j++;
            for(int i = 0; i < items.count() - j; i++) {
                if(items.at(i)->childId() > items.at(i+1)->childId()) {
                    GraphicsPixmapItem *tmpItem = items.at(i);
                    items.replace(i, items.at(i + 1));
                    items.replace(i + 1, tmpItem);
                    swapped = true;
                }
            }
        }
        int currentItem = items.count();
        for(int i = 0; i < items.count(); i++) {
            if(items.at(i)->isSelected())
                currentItem = i;
        }
        if(currentItem == 0) {
            items.at(items.count() - 1)->setSelected(true);
        } else {
            items.at(currentItem - 1)->setSelected(true);
        }
    }

}

void CutoutDialog::toggleGrid()
{
    sceneItems = scene->items();
    restoreSelected = scene->selectedItems();
        if(background->scene() == scene)
            scene->removeItem(background);
        if(frameShow->scene() == scene)
            scene->removeItem(frameShow);
        for(int i = 0; i < pixmapItems.count(); i++)
            if(pixmapItems.at(i)->scene() == scene)
                scene->removeItem(pixmapItems.at(i));
        for(int i = 0; i < scene->items().count(); i++)
            if(scene->items().at(i)->parentItem() == 0)
                scene->removeItem(scene->items().at(i));
    if(isGrid) {
        isGrid = false;
        ui->actionGrid->setIcon(QIcon(":images/grid.svg"));
    } else {
        isGrid = true;
        ui->actionGrid->setIcon(QIcon(":images/gridSelected.svg"));
    }
    restoreWorkspace();
}

void CutoutDialog::exportSelected()
{
    if(scene->selectedItems().count() > 0) {
        QString fileName = QFileDialog::getSaveFileName(this, "Export     Drawing", QDir::homePath(), "PNG Files (*.png *.PNG)");
        if(!fileName.isEmpty()) {
            if(!fileName.endsWith("png", Qt::CaseInsensitive)) {
                fileName = fileName + ".png";
            }
            if(scene->selectedItems().first()->type() == GraphicsPixmapItem::Type) {
                GraphicsPixmapItem *selectedItem = (GraphicsPixmapItem *)scene->selectedItems().first();
                QPixmap pixmap = selectedItem->pixmap();
                pixmap.save(fileName);           
            } else if(scene->selectedItems().first()->type() == GraphicsSvgItem::Type) {
                GraphicsSvgItem *selectedItem = (GraphicsSvgItem *)scene->selectedItems().first();
                QPixmap pixmap(selectedItem->boundingRect().width(), selectedItem->boundingRect().height());
                pixmap.fill(Qt::transparent);
                QPainter painter(&pixmap);
                QStyleOptionGraphicsItem opt;
                selectedItem->paint(&painter, &opt, this);
                painter.end();
                pixmap.save(fileName);
            }
        }
    }
}

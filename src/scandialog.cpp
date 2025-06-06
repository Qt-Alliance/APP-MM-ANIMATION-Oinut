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

#include "scandialog.h"
#include "ui_scandialog.h"
#include <QtWidgets>
#include <sane/sane.h>
#include <sane/saneopts.h>
#include "mainwindow.h"

ScanDialog::ScanDialog(QWidget *) :
    QMainWindow(),
    ui(new Ui::ScanDialog)
{
    ui->setupUi(this);
    setMinimumSize(820, 620);
    zoomFactor = 1.0;
    isColor = true;

    actionScan = new QAction(this);
    actionScan->setIcon(QIcon(":images/scanner.svg"));
    actionScan->setShortcut(QKeySequence(tr("S")));
    actionScan->setToolTip(tr("Scan (S)"));
    ui->toolBar->addAction(actionScan);
    actionToggleColor = new QAction(this);
    actionToggleColor->setIcon(QIcon(":images/colorSelected.svg"));
    actionToggleColor->setToolTip(tr("Toggle Color (C)"));
    actionToggleColor->setShortcut(QKeySequence(tr("C")));
    ui->toolBar->addAction(actionToggleColor);
    resolutionComboBox = new QComboBox;
    resolutionComboBox->addItem(tr("75 dpi"));
    resolutionComboBox->addItem(tr("150 dpi"));
    resolutionComboBox->addItem(tr("300 dpi"));
    resolutionComboBox->addItem(tr("600 dpi"));
    resolutionComboBox->addItem(tr("1200 dpi"));
    QSettings *settings = new QSettings;
    resolutionComboBox->setCurrentIndex(settings->value("Resolution", 1).toInt());
    changeResolution();
    ui->toolBar->addWidget(resolutionComboBox);
    actionSave = new QAction(this);
    actionSave->setIcon(QIcon(":images/save.svg"));
    actionSave->setToolTip(tr("Save (Ctrl+S)"));
    actionSave->setShortcut(QKeySequence(tr("Ctrl+S")));
    ui->toolBar->addAction(actionSave);
    actionSave->setDisabled(true);
    ui->toolBar->addSeparator();

    actionAddImage = new QAction(this);
    actionAddImage->setIcon(QIcon(":images/addImage.svg"));
    actionAddImage->setToolTip(tr("Add Frame (Space)"));
    actionAddImage->setShortcut(QKeySequence(tr("Space")));
    ui->toolBar->addAction(actionAddImage);
    actionInsertImage = new QAction(this);
    actionInsertImage->setIcon(QIcon(":images/insertEmpty.svg"));
    actionInsertImage->setToolTip(tr("Insert Frame (Ctrl+I)"));
    actionInsertImage->setShortcut(QKeySequence(tr("Ctrl+I")));
    ui->toolBar->addAction(actionInsertImage);
    actionReplaceImage = new QAction(this);
    actionReplaceImage->setIcon(QIcon(":images/replace.svg"));
    actionReplaceImage->setToolTip(tr("Replace Frame (Ctrl+R)"));
    actionReplaceImage->setShortcut(QKeySequence(tr("Ctrl+R")));
    ui->toolBar->addAction(actionReplaceImage);
    ui->toolBar->addSeparator();

    actionRotateClockwise = new QAction(this);
    actionRotateClockwise->setIcon(QIcon(":images/rotateClockwise.svg"));
    actionRotateClockwise->setToolTip(tr("Rotate Clockwise (Right)"));
    actionRotateClockwise->setShortcut(QKeySequence(tr("Right")));
    ui->toolBar->addAction(actionRotateClockwise);
    actionRotateCounterClockwise = new QAction(this);
    actionRotateCounterClockwise->setIcon(QIcon(":images/rotate.svg"));
    actionRotateCounterClockwise->setToolTip(tr("Rotate Counter Clockwise (Left)"));
    actionRotateCounterClockwise->setShortcut(QKeySequence(tr("Left")));
    ui->toolBar->addAction(actionRotateCounterClockwise);
    actionScaleUp = new QAction(this);
    actionScaleUp->setIcon(QIcon(":images/scaleUp.svg"));
    actionScaleUp->setToolTip(tr("Scale Up (Up)"));
    actionScaleUp->setShortcut(QKeySequence(tr("Up")));
    ui->toolBar->addAction(actionScaleUp);
    actionScaleDown = new QAction(this);
    actionScaleDown->setIcon(QIcon(":images/scaleDown.svg"));
    actionScaleDown->setToolTip(tr("Scale Down (Down)"));
    actionScaleDown->setShortcut(QKeySequence(tr("Down")));
    ui->toolBar->addAction(actionScaleDown);
    actionRestoreMove = new QAction(this);
    actionRestoreMove->setIcon(QIcon(":images/restoremove.svg"));
    actionRestoreMove->setToolTip(tr("Restore Move (Ctrl+Shift+M)"));
    actionRestoreMove->setShortcut(QKeySequence(tr("Ctrl+Shift+M")));
    ui->toolBar->addAction(actionRestoreMove);
    actionRestoreScale = new QAction(this);
    actionRestoreScale->setIcon(QIcon(":images/restorescale.svg"));
    actionRestoreScale->setToolTip(tr("Restore Scale (Ctrl+Shift+S)"));
    actionRestoreScale->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    ui->toolBar->addAction(actionRestoreScale);
    actionRestoreRotate = new QAction(this);
    actionRestoreRotate->setIcon(QIcon(":images/restorerotate.svg"));
    actionRestoreRotate->setToolTip(tr("Restore Rotate (Ctrl+Shift+R)"));
    actionRestoreRotate->setShortcut(QKeySequence(tr("Ctrl+Shift+R")));
    ui->toolBar->addAction(actionRestoreRotate);
    actionFlipHorizontal = new QAction(this);
    actionFlipHorizontal->setIcon(QIcon(":images/flipHorizontal.svg"));
    actionFlipHorizontal->setToolTip(tr("Flip Horizontal (Ctrl+H)"));
    actionFlipHorizontal->setShortcut(QKeySequence(tr("Ctrl+H")));
    ui->toolBar->addAction(actionFlipHorizontal);
    actionFlipVertical = new QAction(this);
    actionFlipVertical->setIcon(QIcon(":images/flipVertical.svg"));
    actionFlipVertical->setToolTip(tr("Flip Vertical (Ctrl+V)"));
    actionFlipVertical->setShortcut(QKeySequence(tr("Ctrl+V")));
    ui->toolBar->addAction(actionFlipVertical);
    ui->toolBar->addSeparator();

    actionQuit = new QAction(this);
    actionQuit->setIcon(QIcon(":images/quit.svg"));
    actionQuit->setToolTip(tr("Quit (Ctrl+Q)"));
    actionQuit->setShortcut(QKeySequence(tr("Ctrl+Q")));
    ui->toolBar->addAction(actionQuit);

    scene = new QGraphicsScene(this);
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
   
    ui->viewLayout->addWidget(graphicsView);

    connect(actionScan, SIGNAL(triggered()), this, SLOT(scan()));
    connect(actionAddImage, SIGNAL(triggered()), this, SLOT(addImage()));
    connect(actionInsertImage, SIGNAL(triggered()), this, SLOT(insertImage()));
    connect(actionReplaceImage, SIGNAL(triggered()), this, SLOT(replaceImage()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(actionRotateClockwise, SIGNAL(triggered()), this, SLOT(rotateClockwise()));
    connect(actionRotateCounterClockwise, SIGNAL(triggered()), this, SLOT(rotateCounterClockwise()));
    connect(actionScaleUp, SIGNAL(triggered()), this, SLOT(scaleUp()));
    connect(actionScaleDown, SIGNAL(triggered()), this, SLOT(scaleDown()));
    connect(actionRestoreMove, SIGNAL(triggered()), this, SLOT(restoreMove()));
    connect(actionRestoreScale, SIGNAL(triggered()), this, SLOT(restoreScale()));
    connect(actionRestoreRotate, SIGNAL(triggered()), this, SLOT(restoreRotate()));
    connect(actionFlipHorizontal, SIGNAL(triggered()), this, SLOT(flipHorizontal()));
    connect(actionFlipVertical, SIGNAL(triggered()), this, SLOT(flipVertical()));
    connect(ui->actionDragMode, SIGNAL(triggered()), this,                      SLOT(toggleDragMode()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(actionToggleColor, SIGNAL(triggered()), this, SLOT(toggleColor()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(resolutionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeResolution()));
}

void ScanDialog::closeEvent(QCloseEvent *event)
{
    QSettings *settings = new QSettings;
    settings->setValue("ScanPositionX", background->pos().x());
    settings->setValue("ScanPositionY", background->pos().y());
    win->enableActions();
    sane_exit();
    event->accept();
}

void ScanDialog::quit()
{
    QSettings *settings = new QSettings;
    settings->setValue("ScanPositionX", background->pos().x());
    settings->setValue("ScanPositionY", background->pos().y());
    win->enableActions();
    sane_exit();
    close();
}

void ScanDialog::scan()
{
    if(sane_init(NULL, NULL) != SANE_STATUS_GOOD) {
        QMessageBox::warning(this, tr("Oinut"), tr("Couldn't init sane."), QMessageBox::Ok);
        return;
    } else {
        const SANE_Device ** device_list;
        SANE_Handle h;
        SANE_Byte buf[32768];
        SANE_Int maxlen = 32768;
        SANE_Int len = 0;
        sane_get_devices(&device_list, false);
        int num_devices = 0;
        while(device_list[num_devices] != NULL)
            num_devices++;
        if(num_devices == 0) {
            QMessageBox::warning(this, tr("Oinut"), tr("Couldn't find any scanners."), QMessageBox::Ok);
            return;
        } else {
            if(sane_open(device_list[0]->name, &h) != SANE_STATUS_GOOD) {
                QMessageBox::warning(this, tr("Oinut"), tr("Couldn't open scanner."), QMessageBox::Ok);
                return;
            } else {
                SANE_Parameters p;
                int n = 0;
                while(sane_get_option_descriptor(h, n) != NULL) {
                   const SANE_Option_Descriptor *descriptor = sane_get_option_descriptor(h, n);
                   if((QString)descriptor->name != "depth") 
                       n++;
                   else 
                       break;
                }
                SANE_Int val = 8;
                SANE_Int i = 0;
                sane_control_option(h, n, SANE_ACTION_SET_VALUE, (void *)&val, &i);

                n = 0;
                while(sane_get_option_descriptor(h, n) != NULL) {
                   const SANE_Option_Descriptor *descriptor = sane_get_option_descriptor(h, n);
                   if((QString)descriptor->name != "mode") 
                       n++;
                   else 
                       break;
                }
                SANE_String_Const mode;
                if(isColor)
                    mode = "Color";
                else
                    mode = "Gray";
                i = 0;
                sane_control_option(h, n, SANE_ACTION_SET_VALUE, (void *)mode, &i);

                n = 0;
                while(sane_get_option_descriptor(h, n) != NULL) {
                    const SANE_Option_Descriptor *descriptor = sane_get_option_descriptor(h, n);
                    if((QString)descriptor->name != "resolution") 
                        n++;
                    else 
                        break;
                }
                SANE_Int value = resolution;
                sane_control_option(h, n, SANE_ACTION_SET_VALUE, (void *)&value, &i);
                sane_get_parameters(h, &p);
                int width = p.pixels_per_line;
                int height = p.lines;
                if(sane_start(h) != SANE_STATUS_GOOD) {
                    QMessageBox::warning(this, tr("Oinut"), tr("Couldn't start scanner."), QMessageBox::Ok);
                    return;
                } else {
                    QByteArray ba;
                    ba.clear();
                    int y = 0;
                    while(y < p.lines) {
                        sane_read(h, buf, maxlen, &len);
                        ba.append(QByteArray::fromRawData((const char *)buf, len));
                        y++;
                    }
                    if(isColor)
                        image = new QImage((uchar *)ba.data(), width, height, 3*width, QImage::Format_RGB888);
                    else
                        image = new QImage((uchar *)ba.data(), width, height, width, QImage::Format_Indexed8);
                    pixmap.convertFromImage(*image);
                    item->setPixmap(pixmap);
                    item->setPos(scene->sceneRect().center().x() - width / 2, scene->sceneRect().center().y() - height / 2);
                    QSettings *settings = new QSettings;
                    qreal scale = settings->value("ScanScale", 1.0).toDouble();
                    qreal rot = settings->value("ScanRotate", 0.0).toDouble();
                    isFlippedH = settings->value("ScanFlippedH").toBool();
                    isFlippedV = settings->value("ScanFlippedV").toBool();
                    item->setTransformOriginPoint(item->boundingRect().center());
                    item->setScale(scale);
                    item->setRotation(rot);
                    QImage original = image->mirrored(isFlippedH, isFlippedV);
                    pixmap.convertFromImage(original);
                    item->setPixmap(pixmap);
                    actionSave->setDisabled(false);
                    sane_cancel(h);
                    sane_close(h);
                    sane_exit();
                }
            }
        }
    }
}

void ScanDialog::addImage()
{
    QPixmap emtpyBg(w, h);
    emtpyBg.fill(Qt::transparent);
    background->setPixmap(emtpyBg);
    QPixmap empty(3000, 3000);
    empty.fill(Qt::transparent);
    QPainter painter(&empty);
    scene->render(&painter, scene->sceneRect(), scene->sceneRect());
    painter.end();
    empty.save("/home/bahri/dnm.png");
    QPixmap added = empty.copy(background->pos().x(), background->pos().y(), w, h);
    win->addImage(added);
    background->setPixmap(backgroundPixmap);
}

void ScanDialog::insertImage()
{
    QPixmap emtpyBg(w, h);
    emtpyBg.fill(Qt::transparent);
    background->setPixmap(emtpyBg);
    QPixmap empty(3000, 3000);
    empty.fill(Qt::transparent);
    QPainter painter(&empty);
    scene->render(&painter, scene->sceneRect(), scene->sceneRect());
    painter.end();
    QPixmap inserted = empty.copy(background->pos().x(), background->pos().y(), w, h);
    win->insertImage(inserted);
    background->setPixmap(backgroundPixmap);
}

void ScanDialog::replaceImage()
{
    QPixmap emtpyBg(w, h);
    emtpyBg.fill(Qt::transparent);
    background->setPixmap(emtpyBg);
    QPixmap empty(3000, 3000);
    empty.fill(Qt::transparent);
    QPainter painter(&empty);
    scene->render(&painter, scene->sceneRect(), scene->sceneRect());
    painter.end();
    QPixmap replaced = empty.copy(background->pos().x(), background->pos().y(), w, h);
    win->replaceImage(replaced);
    background->setPixmap(backgroundPixmap);
}

void ScanDialog::rotateClockwise()
{
    qreal rot = item->rotation();
    rot += 90;
    if(rot == 360)
        rot = 0;
    item->setTransformOriginPoint(item->boundingRect().center());
    item->setRotation(rot);
    QSettings *settings = new QSettings;
    settings->setValue("ScanRotate", rot);
}

void ScanDialog::rotateCounterClockwise()
{
    qreal rot = item->rotation();
    rot -= 90;
    if(rot == -360)
        rot = 0;
    item->setTransformOriginPoint(item->boundingRect().center());
    item->setRotation(rot);
    QSettings *settings = new QSettings;
    settings->setValue("ScanRotate", rot);
}

void ScanDialog::newImage()
{
    item = new QGraphicsPixmapItem;
    scene->addItem(item);
    image = new QImage(w, h, QImage::Format_RGB32);
    image->fill(qRgb(255, 255, 255));
    QVector<QRgb> colors;
    for(int i = 0; i < 256; ++i)
        colors << qRgb(i, i, i);
    image->setColorTable(colors);
    pixmap.convertFromImage(*image);
    item->setPixmap(pixmap);
    item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
    backgroundPixmap = QPixmap(w, h);
    backgroundPixmap.fill(QColor(255, 255, 255, 5));
    QPen pen(Qt::red, 2);
    pen.setStyle(Qt::DotLine);
    QPainter painter(&backgroundPixmap);
    painter.setPen(pen);
    painter.drawRect(0, 0, w, h);
    painter.end();
    background = new QGraphicsPixmapItem;
    background->setFlag(QGraphicsItem::ItemIsMovable);
    background->setPixmap(backgroundPixmap);
    scene->addItem(background);
    QSettings *settings = new QSettings;
    qreal posX = settings->value("ScanPositionX", scene->sceneRect().center().x() - w / 2).toDouble();
    qreal posY = settings->value("ScanPositionY", scene->sceneRect().center().y() - h / 2).toDouble();
    background->setPos(posX, posY);
    isFlippedH = settings->value("ScanFlippedH", false).toBool();
    isFlippedV = settings->value("ScanFlippedV", false).toBool();
    if(isFlippedH)
        actionFlipHorizontal->setIcon(QIcon(":images/flippedH.svg"));
    if(isFlippedV)
        actionFlipVertical->setIcon(QIcon(":images/flippedV.svg"));
} 

void ScanDialog::scaleUp()
{
    qreal scale = item->scale();
    scale += 0.01;
    item->setTransformOriginPoint(item->boundingRect().center());
    item->setScale(scale);
    QSettings *settings = new QSettings;
    settings->setValue("ScanScale", scale);
}

void ScanDialog::scaleDown()
{
    qreal scale = item->scale();
    scale -= 0.01;
    item->setTransformOriginPoint(item->boundingRect().center());
    item->setScale(scale);
    QSettings *settings = new QSettings;
    settings->setValue("ScanScale", scale);
}

void ScanDialog::restoreMove()
{
    background->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
}

void ScanDialog::restoreScale()
{
    item->setTransformOriginPoint(item->boundingRect().center());
    item->setScale(1.0);
    QSettings *settings = new QSettings;
    settings->setValue("ScanScale", 1.0);
}

void ScanDialog::restoreRotate()
{
    item->setTransformOriginPoint(item->boundingRect().center());
    item->setRotation(0.0);
    QSettings *settings = new QSettings;
    settings->setValue("ScanRotate", 0.0);
}

void ScanDialog::flipHorizontal()
{
    QImage original = pixmap.toImage();
    QImage mirrored = original.mirrored(true, false);
    pixmap.convertFromImage(mirrored);
    item->setPixmap(pixmap);
    isFlippedH = !isFlippedH;
    QSettings *settings = new QSettings;
    settings->setValue("ScanFlippedH", isFlippedH);
    if(isFlippedH)
        actionFlipHorizontal->setIcon(QIcon(":images/flippedH.svg"));
    else
        actionFlipHorizontal->setIcon(QIcon(":images/flipHorizontal.svg"));
}

void ScanDialog::flipVertical()
{
    QImage original = pixmap.toImage();
    QImage mirrored = original.mirrored(false, true);    
    pixmap.convertFromImage(mirrored);
    item->setPixmap(pixmap);
    isFlippedV = !isFlippedV;
    QSettings *settings = new QSettings;
    settings->setValue("ScanFlippedV", isFlippedV);
    if(isFlippedV)
        actionFlipVertical->setIcon(QIcon(":images/flippedV.svg"));
    else
        actionFlipVertical->setIcon(QIcon(":images/flipVertical.svg"));
}

void ScanDialog::toggleDragMode()
{
        if(graphicsView->dragMode() == QGraphicsView::ScrollHandDrag) {
                graphicsView->setDragMode(QGraphicsView::NoDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragMode.svg"));
                background->setFlag(QGraphicsItem::ItemIsMovable, true);
        } else {
                graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragModeSelected.svg"));
                background->setFlag(QGraphicsItem::ItemIsMovable, false);
        }
}

void ScanDialog::zoomIn()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor += 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void ScanDialog::zoomOut()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor -= 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void ScanDialog::toggleColor()
{
        if(isColor) {
            isColor = false;
            actionToggleColor->setIcon(QIcon(":images/color.svg"));
        } else {
            isColor = true;
            actionToggleColor->setIcon(QIcon(":images/colorSelected.svg"));
        }
}

void ScanDialog::save()
{
        QString fileName = QFileDialog::getSaveFileName(this, "Save Image", QDir::homePath(), "PNG Files (*.png *.PNG)");
        if(!fileName.isEmpty()) {
            if(!fileName.endsWith("png", Qt::CaseInsensitive)) {
                fileName = fileName + ".png";
            }
            background->hide();
            QPixmap sceneImage(3000, 3000);
            sceneImage.fill(Qt::transparent);
            QPainter painter(&sceneImage);
            scene->render(&painter, scene->sceneRect(), scene->sceneRect());
            painter.end();
            QGraphicsPixmapItem *sceneItem = new QGraphicsPixmapItem;
            sceneItem->setPixmap(sceneImage);
            QPixmap saved = sceneImage.copy(sceneItem->shape().boundingRect().toRect());
            saved.save(fileName);
            background->show();
        }
}

void ScanDialog::changeResolution()
{
        QString resolutionStr = resolutionComboBox->currentText().split(" ").at(0);
        resolution = resolutionStr.toInt();
        QSettings *settings = new QSettings;
        settings->setValue("Resolution", resolutionComboBox->currentIndex());
}

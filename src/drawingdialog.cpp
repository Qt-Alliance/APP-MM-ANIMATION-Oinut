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

#include "drawingdialog.h"
#include "ui_drawingdialog.h"
#include "layernamedialog.h"
#include "brushsettings.h"
#include "opacitydialog.h"
#include "blurdialog.h"
#include "mainwindow.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <QtWidgets>
#include <QWindowStateChangeEvent>
#include "tabletcanvas.h"
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <stdlib.h>
#ifdef Q_OS_WIN
    #include <stdlib.h>
    #include <ctime>
#endif

using namespace std;

DrawingDialog::DrawingDialog(QWidget *) :
    QMainWindow(),
    ui(new Ui::DrawingDialog)
{
    ui->setupUi(this);

    setMinimumSize(1200, 620);
    isGrid = false;
    ui->actionGrid->setIcon(QIcon(":images/grid.svg"));
    zoomFactor = 1.0;
    fontComboBox = new QFontComboBox;
    fontComboBox->setDisabled(true);
    ui->statusToolBar->addWidget(fontComboBox);
    fontSpinBox = new QSpinBox;
    fontSpinBox->setMaximum(255);
    fontSpinBox->setMinimum(1);
    fontSpinBox->setValue(12);
    fontSpinBox->setDisabled(true);
    ui->statusToolBar->addWidget(fontSpinBox);
    firstRun();

    connect(ui->actionAddImage, SIGNAL(triggered()), this, SLOT(addFile()));
    connect(ui->actionAddLayer, SIGNAL(triggered()), this, SLOT(addLayer()));
    connect(ui->actionAddLayerBelow, SIGNAL(triggered()), this, SLOT(addLayerBelow()));
    connect(ui->actionRemoveLayer, SIGNAL(triggered()), this, SLOT(removeLayer()));
    connect(ui->actionUp, SIGNAL(triggered()), this, SLOT(layerUp()));
    connect(ui->actionDown, SIGNAL(triggered()), this, SLOT(layerDown()));
    connect(ui->actionTopLayer, SIGNAL(triggered()), this, SLOT(layerTop()));
    connect(ui->actionBottomLayer, SIGNAL(triggered()), this, SLOT(layerBottom()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->actionBrushColor, SIGNAL(triggered()), this, SLOT(brushColor()));
    connect(actionLayerName, SIGNAL(triggered()), this, SLOT(changeLayerName()));
    connect(ui->actionToggleBrush, SIGNAL(triggered()), this, SLOT(toggleBrush()));
    connect(ui->actionLockLayer, SIGNAL(triggered()), this, SLOT(toggleLock()));
    connect(ui->actionHideLayer, SIGNAL(triggered()), this, SLOT(toggleVisibility()));
    connect(ui->actionBrushSettings, SIGNAL(triggered()), this, SLOT(brushSettingsDialog()));
    connect(ui->actionAddFrame, SIGNAL(triggered()), this, SLOT(addFrame()));
    connect(ui->actionInsertFrame, SIGNAL(triggered()), this, SLOT(insertFrame()));
    connect(ui->actionReplaceFrame, SIGNAL(triggered()), this, SLOT(replaceFrame()));
    connect(ui->actionExportImage, SIGNAL(triggered()), this, SLOT(exportImage()));
    connect(ui->actionExportLayer, SIGNAL(triggered()), this, SLOT(exportLayer()));
    connect(ui->actionExportDrawing, SIGNAL(triggered()), this, SLOT(exportDrawing()));
    connect(ui->actionOpacity, SIGNAL(triggered()), this, SLOT(setLayerOpacity()));
    connect(ui->actionGrid, SIGNAL(triggered()), this, SLOT(toggleGrid()));
    connect(ui->actionLoadBrush, SIGNAL(triggered()), this, SLOT(loadBrush()));
    connect(ui->actionDragMode, SIGNAL(triggered()), this,                      SLOT(toggleDragMode()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(ui->actionOnion, SIGNAL(triggered()), this, SLOT(onion()));
    connect(ui->actionSelect, SIGNAL(triggered()), this, SLOT(selectMode()));
    connect(ui->actionCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(ui->actionExportSelection, SIGNAL(triggered()), this, SLOT(exportSelection()));
    connect(ui->actionAnchor, SIGNAL(triggered()), this, SLOT(anchor()));
    connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelected()));
    connect(ui->actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));
    connect(ui->actionBlur, SIGNAL(triggered()), this, SLOT(blur()));
    connect(ui->actionColorize, SIGNAL(triggered()), this, SLOT(colorize()));
    connect(ui->actionFitWidth, SIGNAL(triggered()), this, SLOT(fitWidth()));
    connect(ui->actionFitHeight, SIGNAL(triggered()), this, SLOT(fitHeight()));
    connect(ui->actionTextEntry, SIGNAL(triggered()), this, SLOT(textEntry()));
    connect(fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(changeFont()));
    connect(fontSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeFont()));

    new QShortcut(QKeySequence(Qt::Key_7), this, SLOT(increaseBrushSize()));
    new QShortcut(QKeySequence(Qt::Key_6), this, SLOT(decreaseBrushSize()));
    new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_Z), this, SLOT(undo()));
    new QShortcut(QKeySequence(Qt::Key_Up), this, SLOT(keyUp()));
    new QShortcut(QKeySequence(Qt::Key_Down), this, SLOT(keyDown()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(keyRight()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(keyLeft()));

    actionLayerName->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_L));
    connect(brushSizeBox, SIGNAL(valueChanged(int)), this, SLOT(brushSizeChanged()));
}

void DrawingDialog::addImage(QString addFileName)
{
    mergeDrawing();
    QPixmap tempPixmap;
    tempPixmap.load(addFileName);
    selected = tempPixmap;
    selectionPos = QPointF(scene->sceneRect().center().x() - selected.width() / 2, scene->sceneRect().center().y() - selected.height() / 2);
    isClipboard = true;
    paste();
    drawImage();
}

void DrawingDialog::addLayer()
{
    if(layerCount > 0) {
        mergeDrawing();
    }
    currentLayer++;
    layerCount++;
    QGraphicsPixmapItem * tempItem = new QGraphicsPixmapItem;
    QPixmap pixmap = QPixmap(w, h);
    pixmap.fill(QColor(255,255,255,0));
    tempItem->setPixmap(pixmap);
    allImages.insert(currentLayer - 1, tempItem);
    isLocked.insert(currentLayer - 1, false);
    isHidden.insert(currentLayer - 1, false);
    opacityList.insert(currentLayer - 1, 255);
    clearScene();
    drawImage();
    QString newLayerStr = "New Layer";
    layerNames.insert(currentLayer - 1, newLayerStr);
    updateLayers();
}

void DrawingDialog::addLayerBelow()
{
    mergeDrawing();
    QPixmap pixmap = QPixmap(w, h);
    pixmap.fill(QColor(255,255,255,0));
    QGraphicsPixmapItem *tempItem = new QGraphicsPixmapItem;
    tempItem->setPixmap(pixmap);
    allImages.insert(currentLayer - 1, tempItem);
    isLocked.insert(currentLayer - 1, false);
    isHidden.insert(currentLayer - 1, false);
    opacityList.insert(currentLayer - 1, 255);
    layerCount++;
    clearScene();
    drawImage();
    QString newLayerStr = "New Layer";
    layerNames.insert(currentLayer - 1, newLayerStr);
    updateLayers();
}

void DrawingDialog::updateLayers()
{
    QString layerNumberStr;
    layerNumberStr.setNum(currentLayer);
    if(currentLayer < 10)
        layerLabel->setText(" Layer: 0" + layerNumberStr + " - ");
    else
        layerLabel->setText(" Layer: " + layerNumberStr + " - ");
    QString layerAttributesStr = "";
    if(isHidden.at(currentLayer - 1))
        layerAttributesStr += " [Hidden]";
    if(isLocked.at(currentLayer - 1))
        layerAttributesStr += " [Locked]";
    layerAttributesLabel->setText(layerAttributesStr);
    actionLayerName->setText(layerNames.at(currentLayer - 1));
}

void DrawingDialog::removeLayer()
{
     mergeDrawing();
     layerCount--;
     if(layerCount >= 1) {
        allImages.removeAt(currentLayer - 1);
        layerNames.removeAt(currentLayer - 1);
        isLocked.removeAt(currentLayer - 1);
        isHidden.removeAt(currentLayer - 1);
        opacityList.removeAt(currentLayer - 1);
     } else {
        currentLayer = 0;
        allImages.clear();
        layerNames.clear();
        isLocked.clear();
        isHidden.clear();
        opacityList.clear();
        addLayer();
     }
     if(currentLayer > layerCount) {
        currentLayer--;
     }
     drawImage();
     updateLayers();
}

void DrawingDialog::layerUp()
{
    mergeDrawing();
    currentLayer++;
    if (currentLayer > layerCount)
        currentLayer = layerCount;
    updateLayers();
    drawImage();
}

void DrawingDialog::layerDown()
{
    mergeDrawing();
    currentLayer--;
    if (currentLayer < 1)
        currentLayer = 1;
    updateLayers();
    drawImage();
}

void DrawingDialog::layerTop()
{
    mergeDrawing();
    currentLayer = layerCount;
    updateLayers();
    drawImage();
}

void DrawingDialog::layerBottom()
{
    mergeDrawing();
    currentLayer = 1;
    updateLayers();
    drawImage();
}

void DrawingDialog::addFile()
{
    QString addFileName = QFileDialog::getOpenFileName(
                    this,
                    "Select a file to open",
                    QDir::homePath(),
                    "Images (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG)");
    if (!addFileName.isEmpty())
        addImage(addFileName);
}

void DrawingDialog::newImage()
{
        if (setSize())
        {
            makeView();
            makeVars();
            isFirstRun = false;
        }
}

void DrawingDialog::drawImage()
{
    if(isTextEntry) {
        isTextEntry = false;
        fontComboBox->setDisabled(true);
        fontSpinBox->setDisabled(true);
        ui->actionTextEntry->setIcon(QIcon(":images/textEntry.svg"));
        if(textEntryItem->scene() == scene)
            scene->removeItem(textEntryItem);
    } 
    clearScene();
    tabletCanvas->setBrushSize(brushSizeBox->value());
    if(!tabletCanvas->getIfTexture())
        tabletCanvas->setColor(brushPaint);
    else
        tabletCanvas->setTexture(texture);
    tabletCanvas->resize(w, h);
    if(!isPencil) 
        prepareBrush();
    if(isBrush) {
        tabletCanvas->setStylus();
    } else if(isPencil) {
        tabletCanvas->setStylus();
        tabletCanvas->setBrushSize(1);
        tabletCanvas->setAlphaChannelType(TabletCanvas::AlphaPressure);
        tabletCanvas->setLineWidthType(TabletCanvas::NoLineWidth);
        tabletCanvas->setColorSaturationType(TabletCanvas::NoSaturation);
    } else if(isEraser) {
        tabletCanvas->setEraser();
    }
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
    bgItem->setPixmap(bgPixmap);
    scene->addItem(bgItem);
    bgItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);

    scene->addItem(tabletItem);

    if(isFirstFrame && isOnion) {
        QGraphicsPixmapItem *firstItem = new QGraphicsPixmapItem;
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect;
        effect->setColor(Qt::blue);
        firstItem->setOpacity(0.2);
        firstItem->setPixmap(firstFrame);
        firstItem->setGraphicsEffect(effect);
        scene->addItem(firstItem);
        firstItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
    }

    if(isBackFrame && isOnion) {
        QGraphicsPixmapItem *backItem = new QGraphicsPixmapItem;
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect;
        effect->setColor(Qt::blue);
        backItem->setOpacity(0.4);
        backItem->setPixmap(backFrame);
        backItem->setGraphicsEffect(effect);
        scene->addItem(backItem);
        backItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
    }
    if(isForwardFrame && isOnion) {
        QGraphicsPixmapItem *forwardItem = new QGraphicsPixmapItem;
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect;
        effect->setColor(Qt::red);
        forwardItem->setOpacity(0.4);
        forwardItem->setPixmap(forwardFrame);
        forwardItem->setGraphicsEffect(effect);
        scene->addItem(forwardItem);
        forwardItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
    }
    if(isLastFrame && isOnion) {
        QGraphicsPixmapItem *lastItem = new QGraphicsPixmapItem;
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect;
        effect->setColor(Qt::red);
        lastItem->setOpacity(0.2);
        lastItem->setPixmap(lastFrame);
        lastItem->setGraphicsEffect(effect);
        scene->addItem(lastItem);
        lastItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
    }

    for(int i = 0; i < allImages.count(); i++) {
        QGraphicsPixmapItem *tmpPixmap = new QGraphicsPixmapItem;
        tmpPixmap->setPixmap(allImages.at(i)->pixmap());
        tmpPixmap->setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));

        if(i != currentLayer - 1) {
            if(!isHidden.at(i))
                scene->addItem(tmpPixmap);
                tmpPixmap->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
        } else {
            tabletCanvas->hide();
            if(!isHidden.at(i)) {
                ui->actionHideLayer->setIcon(QIcon(":images/shown.svg"));
                if(!isLocked.at(i) && graphicsView->dragMode() == QGraphicsView::NoDrag) {
                    tabletCanvas->setCursor(QPixmap(":images/cursor.png"));
                    ui->actionAddImage->setDisabled(false);
                    ui->actionLockLayer->setIcon(QIcon(":images/unlocked.svg"));
                    tabletCanvas->setOpacity(opacityList.at(currentLayer - 1));
                    tabletCanvas->initPixmap(allImages.at(currentLayer - 1)->pixmap());
                    tabletCanvas->show();
                } else {
                    ui->actionAddImage->setDisabled(true);
                    ui->actionLockLayer->setIcon(QIcon(":images/locked.svg"));
                    scene->addItem(tmpPixmap);
                    tmpPixmap->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
                }
            } else {
                ui->actionHideLayer->setIcon(QIcon(":images/hidden.svg"));
                if(!isLocked.at(i)) {
                    ui->actionAddImage->setDisabled(false);
                    ui->actionLockLayer->setIcon(QIcon(":images/unlocked.svg"));
                    ui->actionAddImage->setDisabled(false);
                } else {
                    ui->actionAddImage->setDisabled(true);
                    ui->actionLockLayer->setIcon(QIcon(":images/locked.svg"));
                    ui->actionAddImage->setDisabled(true);
                }
            }
        }
    }
    if(isPasted) {
        scene->addItem(pastedItem);
        scene->addItem(boundaries);
    }
    if(isSelected) 
        scene->addItem(selection);
}

void DrawingDialog::quit()
{
        win->enableActions();
        close();
}

void DrawingDialog::closeEvent(QCloseEvent *event)
{
        win->enableActions();
        event->accept();
}

void DrawingDialog::brushColor()
{
    QColorDialog chooseColor(this);
    chooseColor.setOption(QColorDialog::ShowAlphaChannel);
    chooseColor.setCurrentColor(brushPaint);
    if(chooseColor.exec() == QDialog::Accepted) {
        brushPaint = chooseColor.selectedColor();

        if (brushPaint.isValid()) {
            tabletCanvas->setColor(brushPaint);
            tabletCanvas->setBrushOpacity(brushPaint.alpha());
            QPixmap colorPixmap(20, 20);
            colorPixmap.fill(brushPaint);
            ui->actionBrushColor->setIcon(QIcon(colorPixmap));
            settings = new QSettings;
            settings->setValue("BrushPaintRed", brushPaint.red());
            settings->setValue("BrushPaintGreen", brushPaint.green());
            settings->setValue("BrushPaintBlue", brushPaint.blue());
            settings->setValue("BrushPaintAlpha", brushPaint.alpha());
            if(isTextEntry) {
                changeFont();
            }
        }
    }
}

void DrawingDialog::increaseBrushSize()
{
    int value = brushSizeBox->value();
    value++;
    if(value > brushSizeBox->maximum()) {
        value = brushSizeBox->maximum();
    }
    brushSizeBox->setValue(value);
}

void DrawingDialog::decreaseBrushSize()
{
    int value = brushSizeBox->value();
    value--;
    if(value < brushSizeBox->minimum()) {
        value = brushSizeBox->minimum();
    }
    brushSizeBox->setValue(value);
}

void DrawingDialog::mergeDrawing()
{
    if(!isLocked.at(currentLayer - 1) && !isHidden.at(currentLayer - 1)) {
        QGraphicsPixmapItem *tmpItem = allImages.at(currentLayer - 1);
        QPixmap pixmap = tabletCanvas->pixmapDrawn();
        tmpItem->setPixmap(pixmap);
        allImages.replace(currentLayer - 1, tmpItem);
        drawImage();
    }
}

void DrawingDialog::changeLayerName()
{
    layerNameDialog->setName(actionLayerName->text());
    if(layerNameDialog->exec() == QDialog::Accepted) {
        QString currentLayerName = layerNameDialog->getName();
        layerNames.replace(currentLayer - 1, currentLayerName);
        updateLayers();
    }
}

void DrawingDialog::toggleBrush()
{
    settings = new QSettings;
    if(isBrush) {
        isBrush = false;
        isPencil = true;
        isEraser = false;
        ui->actionToggleBrush->setIcon(QIcon(":images/pencilMode.svg"));
        tabletCanvas->setStylus();
        tabletCanvas->setBrushSize(1);
        tabletCanvas->setAlphaChannelType(TabletCanvas::AlphaPressure);
        tabletCanvas->setLineWidthType(TabletCanvas::NoLineWidth);
        tabletCanvas->setColorSaturationType(TabletCanvas::NoSaturation);
        settings->setValue("BrushType", "Pencil");
    } else if(isPencil) {
        isPencil = false;
        isBrush = false;
        isEraser = true;
        ui->actionToggleBrush->setIcon(QIcon(":images/eraserMode.svg"));
        tabletCanvas->setBrushSize(brushSizeBox->value());
        tabletCanvas->setEraser();
        settings->setValue("BrushType", "Eraser");
    } else if(isEraser) {
        isBrush = true;
        isPencil = false;
        isEraser = false;
        ui->actionToggleBrush->setIcon(QIcon(":images/brushMode.svg"));
        tabletCanvas->setBrushSize(brushSizeBox->value());
        tabletCanvas->setStylus();
        settings->setValue("BrushType", "Brush");
   }
   if(!isPencil) 
       prepareBrush();
}

void DrawingDialog::brushSizeChanged()
{
    tabletCanvas->setBrushSize(brushSizeBox->value());
    settings = new QSettings;
    settings->setValue("BrushSize", brushSizeBox->value());
    if(isPencil) {
        isPencil = false;
        isBrush = true;
        isEraser = false;
        ui->actionToggleBrush->setIcon(QIcon(":images/brushMode.svg"));
    }
}

void DrawingDialog::undo()
{
    tabletCanvas->setPixmap(tabletCanvas->undo());
}

void DrawingDialog::toggleLock()
{
    mergeDrawing();
    if(isLocked.at(currentLayer - 1)) {
        isLocked.replace(currentLayer - 1, false);
        ui->actionLockLayer->setIcon(QIcon(":images/unlocked.svg"));
    } else {
        isLocked.replace(currentLayer - 1, true);
        ui->actionLockLayer->setIcon(QIcon(":images/locked.svg"));
    }
    drawImage();
    updateLayers();
}

void DrawingDialog::toggleVisibility()
{
    mergeDrawing();
    if(isHidden.at(currentLayer - 1)) {
        isHidden.replace(currentLayer - 1, false);
        ui->actionHideLayer->setIcon(QIcon(":images/shown.svg"));
    } else {
        isHidden.replace(currentLayer - 1, true);
        ui->actionHideLayer->setIcon(QIcon(":images/hidden.svg"));
    }
    drawImage();
    updateLayers();
}

void DrawingDialog::makeView()
{
    scene->setSceneRect(0, 0, 3000, 3000);
    graphicsView = new DrawingGraphicsView(this);
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

    bgItem = new QGraphicsPixmapItem;
    QPixmap bgPixmap(w, h);
    bgPixmap.fill(Qt::white);
    bgItem->setPixmap(bgPixmap);
    bgItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);

    tabletCanvas = new TabletCanvas;
    tabletCanvas->hide();
    tabletCanvas->resize(w, h);
    tabletItem = new QGraphicsProxyWidget;
    tabletItem->setAutoFillBackground(false);
    tabletItem = scene->addWidget(tabletCanvas);
    tabletItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
    tabletItem->setZValue(1000);
    ui->viewLayout->addWidget(graphicsView);
    graphicsView->show();
}

void DrawingDialog::makeVars()
{
    isSizeWithPressure = true;
    isAlphaWithPressure = true;
    isSaturationWithPressure = false;
    tabletCanvas->setAlphaChannelType(TabletCanvas::AlphaPressure);
    tabletCanvas->setLineWidthType(TabletCanvas::LineWidthPressure);
    tabletCanvas->setColorSaturationType(TabletCanvas::NoSaturation);
    layerCount = 0;
    currentLayer = 0;
    isBackFrame = false;
    isFirstFrame = false;
    isForwardFrame = false;
    isLastFrame = false;
    isTopFrame = false;
    isSelect = false;
    isSelected = false;
    isPasted = false;
    isClipboard = false;
    isTextEntry = false;
    selection = new QGraphicsRectItem;
    selection->setZValue(3000);
    QSettings *settings = new QSettings;
    isOnion = settings->value("DrawingOnion", false).toBool();
    if(isOnion)
        ui->actionOnion->setIcon(QIcon(":images/onion3Selected.svg"));
    else
        ui->actionOnion->setIcon(QIcon(":images/onion3.svg"));
    allImages.clear();
    for(int i = 0; i < scene->items().count(); i++)
        scene->removeItem(scene->items().at(i));
    scene->clear();
    layerNames.clear();
    isLocked.clear();
    isHidden.clear();
    opacityList.clear();
    addLayer();
    updateLayers();
    isBrush = true;
    isPencil = false;
    isEraser = false;
    ui->actionToggleBrush->setIcon(QIcon(":images/brushMode.svg"));
    ui->actionLockLayer->setIcon(QIcon(":images/unlocked.svg"));
    ui->actionHideLayer->setIcon(QIcon(":images/shown.svg"));
    settings = new QSettings;
    int brushPaintRed = settings->value("BrushPaintRed", 0).toInt();
    int brushPaintGreen = settings->value("BrushPaintGreen", 0).toInt();
    int brushPaintBlue = settings->value("BrushPaintBlue", 0).toInt();
    int brushPaintAlpha = settings->value("BrushPaintAlpha", 255).toInt();
    int brushSize = settings->value("BrushSize", 10).toInt();
    tabletCanvas->setBrushSize(brushSize);
    brushSizeBox->setValue(brushSize);
    brushPaint = QColor(brushPaintRed, brushPaintGreen, brushPaintBlue);
    tabletCanvas->setColor(brushPaint);
    tabletCanvas->setBrushOpacity(brushPaintAlpha);
    QPixmap colorPixmap(20, 20);
    colorPixmap.fill(brushPaint);
    ui->actionBrushColor->setIcon(QIcon(colorPixmap));
    QString brushType = settings->value("BrushType", "Brush").toString();
    if(brushType == "Brush") {
        isBrush = true;
        isPencil = false;
        isEraser = false;
        ui->actionToggleBrush->setIcon(QIcon(":images/brushMode.svg"));
        tabletCanvas->setBrushSize(brushSizeBox->value());
        tabletCanvas->setStylus();
    } else if(brushType == "Pencil") {
        isBrush = false;
        isPencil = true;
        isEraser = false;
        ui->actionToggleBrush->setIcon(QIcon(":images/pencilMode.svg"));
        tabletCanvas->setStylus();
        tabletCanvas->setBrushSize(1);
        tabletCanvas->setAlphaChannelType(TabletCanvas::AlphaPressure);
        tabletCanvas->setLineWidthType(TabletCanvas::NoLineWidth);
        tabletCanvas->setColorSaturationType(TabletCanvas::NoSaturation);
    } else {
        isPencil = false;
        isBrush = false;
        isEraser = true;
        ui->actionToggleBrush->setIcon(QIcon(":images/eraserMode.svg"));
        tabletCanvas->setBrushSize(brushSizeBox->value());
        tabletCanvas->setEraser();
    }
}

bool DrawingDialog::setSize()
{
            if (!isFirstRun)
            {
                delete graphicsView;
            }

            return true;
        return false;
}

void DrawingDialog::firstRun()
{
    isFirstRun = true;
    isBrush = true;
    isPencil = false;
    isEraser = false;
    isRendering = false;
    w = 640;
    h = 480;
    ui->actionToggleBrush->setIcon(QIcon(":images/brushMode.svg"));
    minWidth = 1000;
    minHeight = 520;
    scene = new QGraphicsScene(this);
    layerLabel = new QLabel;
    layerLabel->setText(tr(" Layer: 01 - "));
    actionLayerName = new QAction(this);
    actionLayerName->setText(tr("New Layer"));
    actionLayerName->setDisabled(false);
    actionLayerName->setToolTip("Rename Layer (Ctrl+L)");
    layerNames.clear();
    emptyLabel = new QLabel;
    emptyLabel->setText(" ");
    emptyLabel2 = new QLabel;
    emptyLabel2->setText(" ");
    layerAttributesLabel = new QLabel;
    layerAttributesLabel->setText("");
   
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* topSpacer = new QWidget();
    topSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    brushSizeLabel = new QLabel;
    brushSizeLabel->setText(tr("Brush Size: "));
    brushSizeBox = new QSpinBox;
    brushSizeBox->setMaximum(3000);
    brushSizeBox->setMinimum(1);
    brushSizeBox->setSingleStep(1);
    brushSizeBox->setValue(10);
    brushSizeBox->setFocusPolicy(Qt::NoFocus);

    ui->toolBar->addWidget(emptyLabel);
    ui->toolBar->addWidget(layerLabel);
    ui->toolBar->addAction(actionLayerName);
    ui->toolBar->addWidget(layerAttributesLabel);
    ui->toolBar->addWidget(topSpacer);
    ui->toolBar->addWidget(spacer);
    ui->toolBar->addWidget(brushSizeLabel);
    ui->toolBar->addWidget(brushSizeBox);

    layerNameDialog = new LayerNameDialog(this);
    brushSettings = new BrushSettingsDialog(this);
    opacityDialog = new OpacityDialog(this);

    QColor defaultColor(Qt::black);
    QPixmap colorPixmap(20, 20);
    colorPixmap.fill(defaultColor);
    ui->actionBrushColor->setIcon(QIcon(colorPixmap));
}

void DrawingDialog::brushSettingsDialog()
{
    brushSettings->setSizeWithPressure(isSizeWithPressure);
    brushSettings->setAlphaWithPressure(isAlphaWithPressure);
    brushSettings->setSaturationWithPressure(isSaturationWithPressure);
    if(brushSettings->exec() == QDialog::Accepted) {
        isSizeWithPressure = brushSettings->getSizeWithPressure();
        isAlphaWithPressure = brushSettings->getAlphaWithPressure();
        isSaturationWithPressure = brushSettings->getSaturationWithPressure();
        if(!isPencil)
            prepareBrush();
    }
}

void DrawingDialog::prepareBrush()
{
    if(isSizeWithPressure)
        tabletCanvas->setLineWidthType(TabletCanvas::LineWidthPressure);
    else {
        tabletCanvas->setLineWidthType(TabletCanvas::NoLineWidth);
        if(!tabletCanvas->getIfTexture())
            tabletCanvas->setColor(brushPaint);
        else
            tabletCanvas->setTexture(texture);
    }
    if(isAlphaWithPressure)
        tabletCanvas->setAlphaChannelType(TabletCanvas::AlphaPressure);
    else {
        tabletCanvas->setAlphaChannelType(TabletCanvas::NoAlpha);
        if(!tabletCanvas->getIfTexture())
            tabletCanvas->setColor(brushPaint);
        else
            tabletCanvas->setTexture(texture);
    }
    if(isSaturationWithPressure)
        tabletCanvas->setColorSaturationType(TabletCanvas::SaturationPressure);
    else {
        tabletCanvas->setColorSaturationType(TabletCanvas::NoSaturation);
        if(!tabletCanvas->getIfTexture())
            tabletCanvas->setColor(brushPaint);
        else
            tabletCanvas->setTexture(texture);
    }
}

void DrawingDialog::addFrame()
{
    mergeDrawing();
    QPixmap pixmap(w, h);
    pixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::                  SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    for(int i = 0; i < allImages.size(); i++) {
        if(!isHidden.at(i)) {
            painter.setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));
            painter.drawPixmap(0, 0, allImages.at(i)->pixmap());
        }
    }
    painter.end();

    win->addImage(pixmap);
    drawImage();
}

void DrawingDialog::insertFrame()
{
    mergeDrawing();
    QPixmap pixmap(w, h);
    pixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::                  SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    for(int i = 0; i < allImages.size(); i++) {
        if(!isHidden.at(i)) {
            painter.setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));
            painter.drawPixmap(0, 0, allImages.at(i)->pixmap());
        }
    }
    painter.end();

    win->insertImage(pixmap);
    drawImage();
}

void DrawingDialog::replaceFrame()
{
    mergeDrawing();
    QPixmap pixmap(w, h);
    pixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::                  SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    for(int i = 0; i < allImages.size(); i++) {
        if(!isHidden.at(i)) {
            painter.setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));
            painter.drawPixmap(0, 0, allImages.at(i)->pixmap());
        }
    }
    painter.end();

    win->replaceImage(pixmap);
    drawImage();
}

void DrawingDialog::setPixmap(QPixmap pixmap)
{
    QGraphicsPixmapItem *tmpItem = new QGraphicsPixmapItem;
    tmpItem->setPixmap(pixmap);
    allImages.insert(0, tmpItem);
    drawImage();
}

void DrawingDialog::exportImage()
{
        mergeDrawing();
        clearScene();
        QString exportFileName = QFileDialog::getSaveFileName(this, "Export     Image", QDir::homePath(), "PNG Files (*.png *.PNG)");
        if(!exportFileName.isEmpty()) {
            if(!exportFileName.endsWith("png", Qt::CaseInsensitive)) {
                exportFileName = exportFileName + ".png";
            }
            QPixmap pixmap(w, h);
            pixmap.fill(Qt::white);
            QPainter painter(&pixmap);
            for(int i = 0; i < allImages.count(); i++)
                if(!isHidden.at(i)) {
                    painter.setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));
                    painter.drawPixmap(0, 0, allImages.at(i)->pixmap());
                }
            painter.end();
            pixmap.save(exportFileName);
        }
        drawImage();
}

void DrawingDialog::exportDrawing()
{
        mergeDrawing();
        clearScene();
        QString exportFileName = QFileDialog::getSaveFileName(this, "Export     Drawing", QDir::homePath(), "PNG Files (*.png *.PNG)");
        if(!exportFileName.isEmpty()) {
            if(!exportFileName.endsWith("png", Qt::CaseInsensitive)) {
                exportFileName = exportFileName + ".png";
            }
            QPixmap pixmap(w, h);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            for(int i = 0; i < allImages.count(); i++)
                if(!isHidden.at(i)) {
                    painter.setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));
                    painter.drawPixmap(0, 0, allImages.at(i)->pixmap());
                }
            painter.end();
            QGraphicsPixmapItem *item = new QGraphicsPixmapItem;
            item->setPixmap(pixmap);
            scene->addItem(item);
            item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
            QPixmap copied = pixmap.copy(item->shape().boundingRect().toRect());
            copied.save(exportFileName);
            scene->removeItem(item);
        }
        drawImage();
}

void DrawingDialog::exportSelection()
{
    if(isSelected) {
        QRectF selectionRect = selection->rect();
        selectionPos = QPointF(selection->rect().x(), selection->rect().y());

        mergeDrawing();
        clearScene();
        QString exportFileName = QFileDialog::getSaveFileName(this, "Export     Drawing", QDir::homePath(), "PNG Files (*.png *.PNG)");
        if(!exportFileName.isEmpty()) {
            if(!exportFileName.endsWith("png", Qt::CaseInsensitive)) {
                exportFileName = exportFileName + ".png";
            }
            QPixmap pixmap(w, h);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            for(int i = 0; i < allImages.count(); i++)
                if(!isHidden.at(i)) {
                    painter.setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));
                    painter.drawPixmap(0, 0, allImages.at(i)->pixmap());
                }
            painter.end();
            QGraphicsPixmapItem *item = new QGraphicsPixmapItem;
            item->setPixmap(pixmap);
            scene->addItem(item);
            item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
            QPixmap sceneImage(3000, 3000);
            sceneImage.fill(Qt::transparent);
            QPainter painter2(&sceneImage);
            scene->render(&painter2, scene->sceneRect(), scene->sceneRect());
            painter2.end();
            QPixmap copied = sceneImage.copy(selectionRect.toRect());
            copied.save(exportFileName);
            scene->removeItem(item);
        }
        drawImage();
    }
}

void DrawingDialog::exportLayer()
{
        mergeDrawing();
        clearScene();
        QString exportFileName = QFileDialog::getSaveFileName(this, "Export     Layer", QDir::homePath(), "PNG Files (*.png *.PNG)");
        if(!exportFileName.isEmpty()) {
            if(!exportFileName.endsWith("png", Qt::CaseInsensitive)) {
                exportFileName = exportFileName + ".png";
            }
            QPixmap pixmap(w, h);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            if(!isHidden.at(currentLayer - 1)) {
                    painter.setOpacity(1.0 * qreal(qreal(opacityList.at(currentLayer - 1)) / 255));
                    painter.drawPixmap(0, 0, allImages.at(currentLayer - 1)->pixmap());
            }
            painter.end();
            pixmap.save(exportFileName);
        }
        drawImage();
}

void DrawingDialog::setLayerOpacity()
{
    mergeDrawing();
    opacityDialog->setOpacity(opacityList.at(currentLayer - 1));
    if(opacityDialog->exec() == QDialog::Accepted)
        opacityList.replace(currentLayer - 1 ,opacityDialog->getOpacity());
    drawImage();
}

void DrawingDialog::toggleGrid()
{
    if(isGrid) {
        isGrid = false;
        ui->actionGrid->setIcon(QIcon(":images/grid.svg"));
    } else {
        isGrid = true;
        ui->actionGrid->setIcon(QIcon(":images/gridSelected.svg"));
    }
    mergeDrawing();
    drawImage();
}

void DrawingDialog::loadBrush()
{
    QString fileName = QFileDialog::getOpenFileName(
                    this,
                    "Select a file to open",
                    QDir::homePath(),
                    "Images (*.png *.xpm *.jpg)");
    if(!fileName.isEmpty()) {
        texture.load(fileName);
        tabletCanvas->setTexture(texture);
        ui->actionBrushColor->setIcon(QIcon(texture));
    }
} 

void DrawingDialog::clearScene()
{
    if(bgItem->scene() == scene)
        scene->removeItem(bgItem);
    if(tabletItem->scene() == scene)
        scene->removeItem(tabletItem);
    if(isPasted) {
        if(pastedItem->scene() == scene)
            scene->removeItem(pastedItem);
        if(boundaries->scene() == scene)
            scene->removeItem(boundaries);
    }
    if(isSelected)
        if(selection->scene() == scene)
            scene->removeItem(selection);
    for(int i = 0; i < scene->items().count(); i++)
        if(scene->items().at(i)->scene() == scene)
            scene->removeItem(scene->items().at(i));
    scene->clear();
}

void DrawingDialog::zoomIn()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor += 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void DrawingDialog::zoomOut()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor -= 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void DrawingDialog::toggleDragMode()
{
        if(graphicsView->dragMode() == QGraphicsView::ScrollHandDrag) {
                graphicsView->setDragMode(QGraphicsView::NoDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragMode.svg"));
                drawImage();
        } else {
                graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragModeSelected.svg"));
                mergeDrawing();
                drawImage();
        }
}

void DrawingDialog::onion()
{
    QSettings *settings = new QSettings;
    if(!isOnion) {
        ui->actionOnion->setIcon(QIcon(":images/onion3Selected.svg"));
        isOnion = true;
    } else {
        isOnion = false;
        ui->actionOnion->setIcon(QIcon(":images/onion3.svg"));
    }
    settings->setValue("DrawingOnion", isOnion);
    drawImage();
}

void DrawingGraphicsView::mousePressEvent(QMouseEvent *event)
{
    DrawingDialog *parent = (DrawingDialog *)scene()->parent();
    isHidden = parent->getIfHidden();
    isLocked = parent->getIfLocked();
    if(isSelect && dragMode() == QGraphicsView::NoDrag && !isHidden && !isLocked) {
       parent->setIfSelected(false);
       isSelected = false;
       for(int i = 0; i < scene()->items().count(); i++)
           if(scene()->items().at(i)->type() == QGraphicsRectItem::Type)
               scene()->removeItem(scene()->items().at(i));
       selection = new QGraphicsRectItem;
       QPen pen(Qt::black, 1);
       pen.setStyle(Qt::DotLine);
       QBrush brush(QColor(255, 255, 255, 5));
       selection->setPen(pen);
       selection->setBrush(brush);
       selection->setRect(mapToScene(event->pos()).x(), mapToScene(event->pos()).y(), 1, 1);
       selection->setFlag(QGraphicsItem::ItemIsMovable);
       initialPos = mapToScene(event->pos());
       deviceDown = true;
       scene()->addItem(selection);
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void DrawingGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(isSelect && dragMode() == QGraphicsView::NoDrag && !isHidden && !isLocked) {
        if(deviceDown) {
            QPointF pos;
            if(initialPos.x() < mapToScene(event->pos()).x())
                pos.setX(mapToScene(event->pos()).x() - initialPos.x());
            else
                pos.setX(-mapToScene(event->pos()).x() + initialPos.x());
            if(initialPos.y() < mapToScene(event->pos()).y())
                pos.setY(mapToScene(event->pos()).y() - initialPos.y());
            else
                pos.setY(-mapToScene(event->pos()).y() + initialPos.y());
            if(initialPos.x() < mapToScene(event->pos()).x() && initialPos.y() < mapToScene(event->pos()).y())
                selection->setRect(initialPos.x(), initialPos.y(), pos.x(), pos.y());
            else if(initialPos.x() > mapToScene(event->pos()).x() && initialPos.y() < mapToScene(event->pos()).y())
                selection->setRect(mapToScene(event->pos()).x(), initialPos.y(), pos.x(), pos.y());
            else if(initialPos.x() < mapToScene(event->pos()).x() && initialPos.y() > mapToScene(event->pos()).y())
                selection->setRect(initialPos.x(), mapToScene(event->pos()).y(), pos.x(), pos.y());
            else
                selection->setRect(mapToScene(event->pos()).x(), mapToScene(event->pos()).y(), pos.x(), pos.y());
        }
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void DrawingGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if(isSelect && dragMode() == QGraphicsView::NoDrag && !isHidden && !isLocked) {
        deviceDown = false;            
        if(selection->boundingRect().width() > 2 && selection->boundingRect().height() > 2) {
            isSelected = true;
            DrawingDialog *parent = (DrawingDialog *)scene()->parent();
            qreal width = (qreal)parent->getWidth();
            qreal height = (qreal)parent->getHeight();
            QPointF position = parent->getPos();
            qreal x = selection->rect().x();
            qreal y = selection->rect().y();
            qreal w = selection->rect().width();
            qreal h = selection->rect().height();
            if(x < position.x()) {
                w = w - (position.x() - x);
                x = position.x();
            }
            if(y < position.y()) {
                h = h - (position.y() - y);
                y = position.y();
            }
            if(x + w > position.x() + width)
                w = w - (x + w) + (position.x() + width);
            if(y + h > position.y() + height)
                h = h - (y + h) + (position.y() + height);
            selection->setRect(x, y, w, h);
            parent->setIfSelected(isSelected);
            parent->setSelectionPos(selection->pos());
            parent->setSelection(selection);
            parent->mergeDrawing();
            parent->clearScene();
            parent->drawImage();
        } else {
            scene()->removeItem(selection);
        }
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void DrawingDialog::selectMode()
{
    if(!isTextEntry) {
        bool isSelect = graphicsView->getSelectMode();
        isSelect = !isSelect;
        graphicsView->setSelectMode(isSelect);
        if(isSelect) {
            ui->actionSelect->setIcon(QIcon(":images/selectSelected.svg"));
            if(graphicsView->dragMode() == QGraphicsView::NoDrag)
                graphicsView->setCursor(QPixmap(":images/cursor.png"));
        } else {
            ui->actionSelect->setIcon(QIcon(":images/select.svg"));
        }
    }
}

void DrawingDialog::cut()
{
    if(isSelected) {
        QRectF selectionRect = selection->rect();
        selectionPos = QPointF(selection->rect().x(), selection->rect().y());
        mergeDrawing();
        clearScene();
        scene->addItem(allImages.at(currentLayer - 1));
        allImages.at(currentLayer - 1)->setPos(bgItem->pos());
        QPixmap sceneImage(3000, 3000);
        sceneImage.fill(Qt::transparent);
        QPainter painter2(&sceneImage);
        scene->render(&painter2, scene->sceneRect(), scene->sceneRect());
        painter2.end();
        selected = QPixmap(selectionRect.width(), selectionRect.height());
        selected = sceneImage.copy(QRect(selectionRect.x(), selectionRect.y(), selectionRect.width(), selectionRect.height()));

        QPixmap cutPixmap(selectionRect.width(), selectionRect.height());
        cutPixmap.fill(Qt::white);
        QGraphicsPixmapItem *currentItem = allImages.at(currentLayer - 1);
        QPixmap currentPixmap = currentItem->pixmap();
        QPointF pos = QPointF(selectionPos.x() - bgItem->pos().x(), selectionPos.y() - bgItem->pos().y());
        QPainter painter(&currentPixmap);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.drawPixmap(pos.x(), pos.y(), cutPixmap);
        painter.end();
        currentItem->setPixmap(currentPixmap);
        allImages.replace(currentLayer - 1, currentItem);

        drawImage();
        isClipboard = true;
        isSelect = false;
        graphicsView->setSelectMode(isSelect);
        ui->actionSelect->setIcon(QIcon(":images/select.svg"));
    }
}

void DrawingDialog::paste()
{
    if(isClipboard) {
        mergeDrawing();
        boundaries = new QGraphicsRectItem;
        boundaries->setRect(bgItem->boundingRect());
        boundaries->setPos(bgItem->pos());
        boundaries->setBrush(QBrush(Qt::transparent));
        QPen boundPen(Qt::red, 2);
        boundPen.setStyle(Qt::DotLine);
        boundaries->setPen(boundPen);
        boundaries->setZValue(4000);
        pastedItem = new Pasted;
        QPixmap pastedPixmap(selected.width(), selected.height());
        pastedPixmap.fill(Qt::transparent);
        QPainter painter(&pastedPixmap);
        painter.drawPixmap(0, 0, selected);
        painter.end();
        pastedItem->setPixmap(pastedPixmap);
        QPointF pos = QPointF(selectionPos.x(), selectionPos.y());
        pastedItem->setPos(pos);
        pastedItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        pastedItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        pastedItem->setZValue(3000);
        pastedItem->setCursor(QPixmap(":images/moveCursor.png"));
        scene->addItem(pastedItem);
        scene->addItem(boundaries);
        pastedItem->setSelected(true);
        isPasted = true;

        QPen pen(Qt::black, 2);
        QBrush brush(QColor(255, 255, 255, 128));

        Pivot *pivot = new Pivot;
        pivot->setRect(0, 0, 10, 10);
        pivot->setPen(pen);
        pivot->setBrush(brush);
        pivot->setPos(pastedItem->boundingRect().center().x() - 5, pastedItem->boundingRect().center().y() - 5);
        pivot->setParentItem(pastedItem);
        pivot->setCursor(QPixmap(":images/rotateCursor.png"));

        BottomRightHandle *bottomRight = new BottomRightHandle;
        bottomRight->setRect(0, 0, 10, 10);
        bottomRight->setPen(pen);
        bottomRight->setBrush(brush);
        bottomRight->setPos(pastedItem->boundingRect().width() - 5, pastedItem->boundingRect().height() - 5);
        bottomRight->setParentItem(pastedItem);
        bottomRight->setCursor(QPixmap(":images/scaleCursor.png"));

        BottomLeftHandle *bottomLeft = new BottomLeftHandle;
        bottomLeft->setRect(0, 0, 10, 10);
        bottomLeft->setPen(pen);
        bottomLeft->setBrush(brush);
        bottomLeft->setPos(-5, pastedItem->boundingRect().height() - 5);
        bottomLeft->setParentItem(pastedItem);
        bottomLeft->setCursor(QPixmap(":images/scaleCursor.png"));

        TopRightHandle *topRight = new TopRightHandle;
        topRight->setRect(0, 0, 10, 10);
        topRight->setPen(pen);
        topRight->setBrush(brush);
        topRight->setPos(pastedItem->boundingRect().width() - 5, -5);
        topRight->setParentItem(pastedItem);
        topRight->setCursor(QPixmap(":images/scaleCursor.png"));

        TopLeftHandle *topLeft = new TopLeftHandle;
        topLeft->setRect(0, 0, 10, 10);
        topLeft->setPen(pen);
        topLeft->setBrush(brush);
        topLeft->setPos(-5, -5);
        topLeft->setParentItem(pastedItem);
        topLeft->setCursor(QPixmap(":images/scaleCursor.png"));

        isSelected = false;
        isSelect = false;
        graphicsView->setSelectMode(isSelect);
        ui->actionSelect->setIcon(QIcon(":images/select.svg"));
        clearScene();
        drawImage();
    }
}

void DrawingDialog::copy()
{
    if(isSelected) {
        mergeDrawing();
        clearScene();
        QRectF selectionRect = selection->rect();
        selectionPos = QPointF(selection->rect().x(), selection->rect().y());
        scene->addItem(allImages.at(currentLayer - 1));
        allImages.at(currentLayer - 1)->setPos(bgItem->pos());
        QPixmap sceneImage(3000, 3000);
        sceneImage.fill(Qt::transparent);
        QPainter painter2(&sceneImage);
        scene->render(&painter2, scene->sceneRect(), scene->sceneRect());
        painter2.end();
        selected = QPixmap(selectionRect.width(), selectionRect.height());
        selected = sceneImage.copy(QRect(selectionRect.x(), selectionRect.y(), selectionRect.width(), selectionRect.height()));
        drawImage();
        isClipboard = true;
        isSelect = false;
        graphicsView->setSelectMode(isSelect);
        ui->actionSelect->setIcon(QIcon(":images/select.svg"));
    }
}

void Pivot::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    initialPos = mapToScene(event->pos());
}

void Pivot::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Pasted *parent = (Pasted *)parentItem();
    qreal rot = parent->getRot();
    qreal scx = parent->getScaleX();
    qreal scy = parent->getScaleY();
    QPointF pos = mapToScene(event->pos());
    qreal width = parent->boundingRect().width();
    qreal height = parent->boundingRect().height();
    if(pos.y() < initialPos.y())
        rot--;
    else
        rot++;
    QTransform transform;
    transform.translate(width / 2, height / 2);
    transform.rotate(rot);
    transform.scale(scx, scy);
    transform.translate(-width / 2, -height / 2);
    parent->setTransform(transform);
    parent->setRot(rot);

    initialPos = pos;
}

void BottomRightHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    initialPos = mapToScene(event->pos());
}

void BottomRightHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Pasted *parent = (Pasted *)parentItem();
    qreal rot = parent->getRot();
    QPointF pos = mapToScene(event->pos());
    qreal width = parent->boundingRect().width();
    qreal height = parent->boundingRect().height();
    qreal dsx = (pos.x() - initialPos.x()) *2 / width;
    qreal dsy = (pos.y() - initialPos.y()) *2/ height;
    QTransform transform;
    transform.translate(width / 2, height / 2);
    transform.rotate(rot);
    if(event->modifiers() == Qt::ControlModifier) {
        transform.scale(1.0 + dsx, 1.0 + dsx);
        parent->setScaleX(1.0 + dsx);
        parent->setScaleY(1.0 + dsx);
    } else {
        transform.scale(1.0 + dsx, 1.0 + dsy);
        parent->setScaleX(1.0 + dsx);
        parent->setScaleY(1.0 + dsx);
    }
    transform.translate(-width / 2, -height / 2);
    parent->setTransform(transform);
    QTransform transform2;
    transform2.translate(5, 5);
    transform2.rotate(0);
    if(event->modifiers() == Qt::ControlModifier)
        transform2.scale(1 / (1.0 + dsx), 1 / (1.0 + dsx));
    else
        transform2.scale(1 / (1.0 + dsx), 1 / (1.0 + dsy));
    transform2.translate(-5, -5);
    for(int i = 0; i < parent->childItems().count(); i++)
        parent->childItems().at(i)->setTransform(transform2);
}

void BottomLeftHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    initialPos = mapToScene(event->pos());
}

void BottomLeftHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Pasted *parent = (Pasted *)parentItem();
    qreal rot = parent->getRot();
    QPointF pos = mapToScene(event->pos());
    qreal width = parent->boundingRect().width();
    qreal height = parent->boundingRect().height();
    qreal dsx = (-pos.x() + initialPos.x()) *2/ width;
    qreal dsy = (pos.y() - initialPos.y()) *2/ height;
    QTransform transform;
    transform.translate(width / 2, height / 2);
    transform.rotate(rot);
    if(event->modifiers() == Qt::ControlModifier)
        transform.scale(1.0 + dsx, 1.0 + dsx);
    else
        transform.scale(1.0 + dsx, 1.0 + dsy);
    transform.translate(-width / 2, -height / 2);
    parent->setTransform(transform);
    parent->setScaleX(1.0 + dsx);
    parent->setScaleY(1.0 + dsy);
    QTransform transform2;
    transform2.translate(5, 5);
    transform2.rotate(0);
    if(event->modifiers() == Qt::ControlModifier)
        transform2.scale(1 / (1.0 + dsx), 1 / (1.0 + dsx));
    else
        transform2.scale(1 / (1.0 + dsx), 1 / (1.0 + dsy));
    transform2.translate(-5, -5);
    for(int i = 0; i < parent->childItems().count(); i++)
        parent->childItems().at(i)->setTransform(transform2);
}

void TopLeftHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    initialPos = mapToScene(event->pos());
}

void TopLeftHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Pasted *parent = (Pasted *)parentItem();
    qreal rot = parent->getRot();
    QPointF pos = mapToScene(event->pos());
    qreal width = parent->boundingRect().width();
    qreal height = parent->boundingRect().height();
    qreal dsx = (-pos.x() + initialPos.x()) *2/ width;
    qreal dsy = (-pos.y() + initialPos.y()) *2/ height;
    QTransform transform;
    transform.translate(width / 2, height / 2);
    transform.rotate(rot);
    if(event->modifiers() == Qt::ControlModifier)
        transform.scale(1.0 + dsx, 1.0 + dsx);
    else
        transform.scale(1.0 + dsx, 1.0 + dsy);
    transform.translate(-width / 2, -height / 2);
    parent->setTransform(transform);
    parent->setScaleX(1.0 + dsx);
    parent->setScaleY(1.0 + dsy);
    QTransform transform2;
    transform2.translate(5, 5);
    transform2.rotate(0);
    if(event->modifiers() == Qt::ControlModifier)
        transform2.scale(1 / (1.0 + dsx), 1 / (1.0 + dsx));
    else
        transform2.scale(1 / (1.0 + dsx), 1 / (1.0 + dsy));
    transform2.translate(-5, -5);
    for(int i = 0; i < parent->childItems().count(); i++)
        parent->childItems().at(i)->setTransform(transform2);
}

void TopRightHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    initialPos = mapToScene(event->pos());
}

void TopRightHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Pasted *parent = (Pasted *)parentItem();
    qreal rot = parent->getRot();
    QPointF pos = mapToScene(event->pos());
    qreal width = parent->boundingRect().width();
    qreal height = parent->boundingRect().height();
    qreal dsx = (pos.x() - initialPos.x()) *2/ width;
    qreal dsy = (-pos.y() + initialPos.y()) *2/ height;
    QTransform transform;
    transform.translate(width / 2, height / 2);
    transform.rotate(rot);
    if(event->modifiers() == Qt::ControlModifier)
        transform.scale(1.0 + dsx, 1.0 + dsx);
    else
        transform.scale(1.0 + dsx, 1.0 + dsy);
    transform.translate(-width / 2, -height / 2);
    parent->setTransform(transform);
    parent->setScaleX(1.0 + dsx);
    parent->setScaleY(1.0 + dsy);
    QTransform transform2;
    transform2.translate(5, 5);
    transform2.rotate(0);
    if(event->modifiers() == Qt::ControlModifier)
        transform2.scale(1 / (1.0 + dsx), 1 / (1.0 + dsx));
    else
        transform2.scale(1 / (1.0 + dsx), 1 / (1.0 + dsy));
    transform2.translate(-5, -5);
    for(int i = 0; i < parent->childItems().count(); i++)
        parent->childItems().at(i)->setTransform(transform2);
}

void DrawingDialog::anchor()
{
        if(isPasted) {
            mergeDrawing();
            QPixmap pixmap = allImages.at(currentLayer - 1)->pixmap();
            QGraphicsPixmapItem *tmpItem = allImages.at(currentLayer - 1);
            QPixmap sceneImage(3000, 3000);
            sceneImage.fill(Qt::transparent);
            clearScene();
            scene->addItem(pastedItem);
            pastedItem->setSelected(false);
            for(int i = 0; i < pastedItem->childItems().count(); i++)
                pastedItem->childItems().at(i)->setOpacity(0.0);
            QPainter painter(&sceneImage);
            scene->render(&painter, scene->sceneRect(), scene->sceneRect());
            painter.end();
            QGraphicsPixmapItem *sceneShape = new QGraphicsPixmapItem;
            sceneShape->setPixmap(sceneImage);
            QRectF rect = sceneShape->shape().boundingRect();
            QPixmap pasted = sceneImage.copy(rect.toRect());
            QPainter painter2(&pixmap);
            painter2.drawPixmap(rect.x() - bgItem->pos().x(), rect.y() - bgItem->pos().y(), pasted);
            painter2.end();
            isPasted = false;
            tmpItem->setPixmap(pixmap);
            allImages.replace(currentLayer - 1, tmpItem);
            drawImage();
        }
}

void DrawingDialog::deleteSelected()
{
    if(isSelected) {
        QRectF selectionRect = selection->rect();
        selectionPos = QPointF(selection->rect().x(), selection->rect().y());
        mergeDrawing();
        QPixmap cutPixmap(selectionRect.width(), selectionRect.height());
        cutPixmap.fill(Qt::white);
        QGraphicsPixmapItem *currentItem = allImages.at(currentLayer - 1);
        QPixmap currentPixmap = currentItem->pixmap();
        QPointF pos = QPointF(selectionPos.x() - bgItem->pos().x(), selectionPos.y() - bgItem->pos().y());
        QPainter painter(&currentPixmap);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.drawPixmap(pos.x(), pos.y(), cutPixmap);
        painter.end();
        currentItem->setPixmap(currentPixmap);
        allImages.replace(currentLayer - 1, currentItem);

        drawImage();
    }
    if(isPasted) {
        scene->removeItem(pastedItem);
        scene->removeItem(boundaries);
        isPasted = false;
    }
}

void DrawingDialog::selectAll()
{
    isSelect = true;
    graphicsView->setSelectMode(isSelect);
    ui->actionSelect->setIcon(QIcon(":images/selectSelected.svg"));
    if(graphicsView->dragMode() == QGraphicsView::NoDrag)
        graphicsView->setCursor(QPixmap(":images/cursor.png"));
    QPen pen(Qt::black, 1);
    pen.setStyle(Qt::DotLine);
    QBrush brush(QColor(255, 255, 255, 5));
    selection->setPen(pen);
    selection->setBrush(brush);
    selection->setRect(bgItem->pos().x(), bgItem->pos().y(), w, h);
    isSelected = true;
    drawImage();
}

void DrawingDialog::blur()
{
    blurDialog = new BlurDialog(this);
    int radius;
    if(blurDialog->exec() == QDialog::Accepted) {
        radius = blurDialog->getRadius();
            mergeDrawing();
            clearScene();
            QGraphicsPixmapItem *item = allImages.at(currentLayer - 1);
            scene->addItem(item);
            item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
            QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
            blurEffect->setBlurRadius(radius);
            item->setGraphicsEffect(blurEffect);
            QPixmap sceneImage(3000, 3000);
            sceneImage.fill(Qt::transparent);
            QPainter painter(&sceneImage);
            scene->render(&painter, scene->sceneRect(), scene->sceneRect());
            painter.end();
            QPixmap blurred = sceneImage.copy(QRect(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2, w, h));
            blurEffect->setBlurRadius(0);
            item->setGraphicsEffect(blurEffect);
            item->setPixmap(blurred);
            allImages.replace(currentLayer - 1, item);
            scene->removeItem(item);
            drawImage();
    }
}

void DrawingDialog::colorize()
{
    QColorDialog chooseColor(this);
    chooseColor.setOption(QColorDialog::ShowAlphaChannel);
    chooseColor.setCurrentColor(brushPaint);
    if(chooseColor.exec() == QDialog::Accepted) {
            QColor c =  chooseColor.selectedColor();
            mergeDrawing();
            clearScene();
            QGraphicsPixmapItem *item = allImages.at(currentLayer - 1);
            scene->addItem(item);
            item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
            QGraphicsColorizeEffect *colorizeEffect = new QGraphicsColorizeEffect;
            colorizeEffect->setColor(c);
            item->setGraphicsEffect(colorizeEffect);
            QPixmap sceneImage(3000, 3000);
            sceneImage.fill(Qt::transparent);
            QPainter painter(&sceneImage);
            scene->render(&painter, scene->sceneRect(), scene->sceneRect());
            painter.end();
            QPixmap colored = sceneImage.copy(QRect(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2, w, h));
            colorizeEffect->setStrength(0);
            item->setGraphicsEffect(colorizeEffect);
            item->setPixmap(colored);
            allImages.replace(currentLayer - 1, item);
            scene->removeItem(item);
            drawImage();
    }
}

void DrawingDialog::keyUp()
{
    int dx = 0;
    int dy = -1;
    if(isPasted)
        pastedItem->moveBy(dx, dy);
    else
        layerUp();
}

void DrawingDialog::keyDown()
{
    int dx = 0;
    int dy = 1;
    if(isPasted)
        pastedItem->moveBy(dx, dy);
    else
        layerDown();
}

void DrawingDialog::keyRight()
{
    int dx = 1;
    int dy = 0;
    if(isPasted)
        pastedItem->moveBy(dx, dy);
}

void DrawingDialog::keyLeft()
{
    int dx = -1;
    int dy = 0;
    if(isPasted)
        pastedItem->moveBy(dx, dy);
}

void DrawingDialog::fitWidth()
{
    if(isPasted) {
        pastedItem->resetTransform();
        for(int i = 0; i < pastedItem->childItems().count(); i++)
            pastedItem->childItems().at(i)->resetTransform();
        QPixmap pixmap = selected.scaledToWidth(w);
        pastedItem->setPos(scene->sceneRect().center().x() - pixmap.width() / 2, scene->sceneRect().center().y() - pixmap.height() / 2);
        pastedItem->setPixmap(pixmap);
        pastedItem->childItems().at(0)->setPos(pixmap.width() / 2, pixmap.height() / 2);
        pastedItem->childItems().at(1)->setPos(pixmap.width() - 5, pixmap.height() - 5);
        pastedItem->childItems().at(2)->setPos(-5, pixmap.height() - 5);
        pastedItem->childItems().at(3)->setPos(pixmap.width() - 5, -5);
        pastedItem->childItems().at(4)->setPos(-5, -5);
    }
}

void DrawingDialog::fitHeight()
{
    if(isPasted) {
        pastedItem->resetTransform();
        for(int i = 0; i < pastedItem->childItems().count(); i++)
            pastedItem->childItems().at(i)->resetTransform();
        QPixmap pixmap = selected.scaledToHeight(h);
        pastedItem->setPos(scene->sceneRect().center().x() - pixmap.width() / 2, scene->sceneRect().center().y() - pixmap.height() / 2);
        pastedItem->setPixmap(pixmap);
        pastedItem->childItems().at(0)->setPos(pixmap.width() / 2, pixmap.height() / 2);
        pastedItem->childItems().at(1)->setPos(pixmap.width() - 5, pixmap.height() - 5);
        pastedItem->childItems().at(2)->setPos(-5, pixmap.height() - 5);
        pastedItem->childItems().at(3)->setPos(pixmap.width() - 5, -5);
        pastedItem->childItems().at(4)->setPos(-5, -5);
    }
}

void DrawingDialog::textEntry()
{
        if(isTextEntry) {
            isTextEntry = false;
            fontComboBox->setDisabled(true);
            fontSpinBox->setDisabled(true);
            ui->actionTextEntry->setIcon(QIcon(":images/textEntry.svg"));
            QPixmap pixmap(w, h);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            textEntryTextEdit->document()->documentLayout()->setPaintDevice(painter.device());
            textEntryTextEdit->document()->drawContents(&painter);
            painter.end();
            QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem;
            pixmapItem->setPixmap(pixmap);
            pixmapItem->setPos(bgItem->pos());
            QPixmap cropped = pixmap.copy(pixmapItem->shape().boundingRect().toRect());
            selected = cropped;
            selectionPos = pixmapItem->mapToScene(pixmapItem->shape().boundingRect().topLeft()).toPoint();
            isClipboard = true;
            paste();
            if(textEntryItem->scene() == scene)
                scene->removeItem(textEntryItem);
        } else {
            mergeDrawing();
            isSelected = false;
            isSelect = false;
            graphicsView->setSelectMode(false);
            ui->actionSelect->setIcon(QIcon(":images/select.svg"));
            drawImage();
            isTextEntry = true;
            fontComboBox->setDisabled(false);
            fontSpinBox->setDisabled(false);
            ui->actionTextEntry->setIcon(QIcon(":images/textEntrySelected.svg"));
            textEntryTextEdit = new QTextEdit;
            textEntryTextEdit->setStyleSheet(tr("QTextEdit { background: transparent; }"));
            textEntryTextEdit->setFocus();
            textEntryTextEdit->setFixedSize(w, h);
            QFont font = fontComboBox->currentFont();
            QTextCharFormat fmt;
            fmt.setFont(font);
            QBrush brush(brushPaint);
            fmt.setForeground(brush);
            textEntryTextEdit->setCurrentCharFormat(fmt);
            textEntryTextEdit->setFontPointSize(fontSpinBox->value());
            textEntryItem = scene->addWidget(textEntryTextEdit);
            textEntryItem->setPos(bgItem->pos());
            textEntryItem->setZValue(5000);
        }
}

void DrawingDialog::changeFont()
{
        QTextCharFormat fmt;
        fmt.setFont(fontComboBox->currentFont());
        fmt.setFontPointSize(fontSpinBox->value());
        QBrush brush(brushPaint);
        fmt.setForeground(brush);
        textEntryTextEdit->setCurrentCharFormat(fmt);
}

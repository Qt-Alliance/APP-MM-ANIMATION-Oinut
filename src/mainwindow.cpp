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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sizedialog.h"
#include "videodialog.h"
#include "cutoutdialog.h"
#include "drawingdialog.h"
#include "layernamedialog.h"
#include "opacitydialog.h"
#include "scandialog.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QFileDialog>
#include <QLabel>
#include <QSpinBox>
#include <QBitmap>
#include <QShortcut>
#include <QTranslator>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setMinimumSize(1000, 680);
    username = QString(getenv("USER"));
    tmpDir = QDir::tempPath() + "/oinut-" + username + "/";
    tmpname = "";
    path = "";
    fileName = "";
    zoomFactor = 1.0;
    ui->layerToolBar->hide();
    ui->frameToolBar->hide();

    isFirstRun = true;
    isReplace = false;
    isSaved = true;
    isPlaying = false;
    sizeDialog = new SizeDialog(this);
    layerNameDialog = new LayerNameDialog(this);
    cutoutDialog = new CutoutDialog(this);
    videoDialog = new VideoDialog(this);
    drawingDialog = new DrawingDialog(this);
    opacityDialog = new OpacityDialog(this);
    scanDialog = new ScanDialog(this);
    timelineWidget = new TableWidget;
    timelineWidget->hide();

    w = 640;
    h = 480;
    frame = 1;
    soundFileNames.clear();
    soundFiles.clear();
    frameLabel = new QLabel;
    frameLabel->setText(tr("     Frame: "));
    frameNumberLabel = new QLabel;
    frameNumberLabel->setText("0001");
    layerLabel = new QLabel;
    layerLabel->setText(tr("     Layer: "));
    actionLayerName = new QAction(this);
    actionLayerName->setText(tr("New Layer"));
    actionLayerName->setDisabled(true);
    actionLayerName->setToolTip("Rename Layer (Ctrl+L)");
    layerNames.clear();
    QWidget* spacer1 = new QWidget();
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->statusToolBar->addWidget(spacer1);

    timer = new QTimer(this);
    frameCountLabel = new QLabel;
    frameCountLabel->setText(tr("   0000 frames"));  
    ui->frameStatusToolBar->addWidget(frameCountLabel);
    spacer3 = new QWidget();
    spacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->frameStatusToolBar->addWidget(spacer1);
    ui->frameStatusToolBar->addWidget(frameLabel);
    ui->frameStatusToolBar->addWidget(frameNumberLabel);
    ui->layerStatusToolBar->addWidget(layerLabel);
    ui->layerStatusToolBar->addAction(actionLayerName);
    ui->toolBar->addWidget(spacer3);
    ui->frameToolBar->addWidget(ui->frameStatusToolBar);
    ui->layerToolBar->addWidget(ui->layerStatusToolBar);

    fpsLabel = new QLabel;
    fpsLabel->setText(tr("     FPS: "));
    fps = new QSpinBox;
    fps->setMaximum(30);
    fps->setMinimum(1);
    fps->setValue(12);
    ui->toolBar->addWidget(fpsLabel);
    ui->toolBar->addWidget(fps);

    greenScreenLabel = new QLabel;
    greenScreenLabel->setText(tr("    Green Screen Strength:"));
    greenScreenSpinBox = new QSpinBox;
    greenScreenSpinBox->setMaximum(255);
    greenScreenSpinBox->setMinimum(0);
    greenScreenSpinBox->setValue(235);
    ui->toolBar->addWidget(greenScreenLabel);
    ui->toolBar->addWidget(greenScreenSpinBox);
    greenScreenSpinBox->setDisabled(true);

    ui->actionAddEmpty->setDisabled(true);
    ui->actionAddLayer->setDisabled(true);
    ui->actionAddLayerBelow->setDisabled(true);
    ui->actionDown->setDisabled(true);
    ui->actionNext_Frame->setDisabled(true);
    ui->actionPlay->setDisabled(true);
    ui->actionPrevious_Frame->setDisabled(true);
    ui->actionRemoveLayer->setDisabled(true);
    ui->actionRender->setDisabled(true);
    ui->actionRenderVideo->setDisabled(true);
    ui->actionToggleLoop->setDisabled(true);
    ui->actionUp->setDisabled(true);
    ui->actionMultiply->setDisabled(true);
    ui->actionGreenScreen->setDisabled(true);
    ui->actionFirst->setDisabled(true);
    ui->actionLast->setDisabled(true);
    ui->actionTopLayer->setDisabled(true);
    ui->actionBottomLayer->setDisabled(true);
    ui->actionDelete->setDisabled(true);
    ui->actionAddImages->setDisabled(true);
    ui->actionHide->setDisabled(true);
    ui->actionVideo->setDisabled(true);
    ui->actionDrawing->setDisabled(true);
    ui->actionCutout->setDisabled(true);
    ui->actionInsert->setDisabled(true);
    ui->actionInsertEmpty->setDisabled(true);
    ui->actionDuplicate->setDisabled(true);
    ui->actionReplace->setDisabled(true);
    ui->actionExportFrame->setDisabled(true);
    ui->actionExportFrameInCurrentLayer->setDisabled(true);
    ui->actionAddSound->setDisabled(true);
    ui->actionRemoveSound->setDisabled(true);
    ui->actionExportLayer->setDisabled(true);
    ui->actionMergeDown->setDisabled(true);
    ui->actionSave->setDisabled(true);
    ui->actionSaveAs->setDisabled(true);
    ui->actionOpacity->setDisabled(true);
    ui->actionImportVideo->setDisabled(true);
    ui->actionScan->setDisabled(true);
    ui->actionDragMode->setDisabled(true);
    ui->actionZoomIn->setDisabled(true);
    ui->actionZoomOut->setDisabled(true);
    ui->actionDuplicateAndAppend->setDisabled(true);
    ui->actionMoveLayerUp->setDisabled(true);
    ui->actionMoveLayerDown->setDisabled(true);
    ui->actionResize->setDisabled(true);
    
    connect(timer, SIGNAL(timeout()), this, SLOT(playNextFrame()));
    connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(play()));
    connect(ui->actionAddImages, SIGNAL(triggered()), this, SLOT(addFiles()));
    connect(ui->actionPrevious_Frame, SIGNAL(triggered()), this, SLOT(showPreviousFrame()));
    connect(ui->actionNext_Frame, SIGNAL(triggered()), this, SLOT(showNextFrame()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newAnim()));
    connect(ui->actionRender, SIGNAL(triggered()), this, SLOT(render()));
    connect(ui->actionRenderVideo, SIGNAL(triggered()), this, SLOT(renderVideo()));
    connect(ui->actionToggleLoop, SIGNAL(triggered()), this, SLOT(toggleLoop()));
    connect(ui->actionAddLayer, SIGNAL(triggered()), this, SLOT(addLayer()));
    connect(ui->actionAddLayerBelow, SIGNAL(triggered()), this, SLOT(addLayerBelow()));
    connect(ui->actionRemoveLayer, SIGNAL(triggered()), this, SLOT(removeLayer()));
    connect(ui->actionUp, SIGNAL(triggered()), this, SLOT(layerUp()));
    connect(ui->actionDown, SIGNAL(triggered()), this, SLOT(layerDown()));
    connect(ui->actionAddEmpty, SIGNAL(triggered()), this, SLOT(addEmptyFrame()));
    connect(ui->actionMultiply, SIGNAL(triggered()), this, SLOT(makeWhiteTransparent()));
    connect(ui->actionGreenScreen, SIGNAL(triggered()), this, SLOT(makeGreenScreen()));
    connect(ui->actionFirst, SIGNAL(triggered()), this, SLOT(showFirstFrame()));
    connect(ui->actionLast, SIGNAL(triggered()), this, SLOT(showLastFrame()));
    connect(ui->actionTopLayer, SIGNAL(triggered()), this, SLOT(layerTop()));
    connect(ui->actionBottomLayer, SIGNAL(triggered()), this, SLOT(layerBottom()));
    connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(removeFrame()));
    connect(ui->actionHide, SIGNAL(triggered()), this, SLOT(hideLayer()));
    connect(ui->actionVideo, SIGNAL(triggered()), this, SLOT(showVideo()));
    connect(ui->actionCutout, SIGNAL(triggered()), this, SLOT(showCutout()));
    connect(ui->actionDrawing, SIGNAL(triggered()), this, SLOT(showDrawing()));
    connect(ui->actionInsert, SIGNAL(triggered()), this, SLOT(insertFiles()));
    connect(ui->actionInsertEmpty, SIGNAL(triggered()), this, SLOT(insertEmptyFrame()));
    connect(ui->actionReplace, SIGNAL(triggered()), this, SLOT(replace()));
    connect(ui->actionDuplicate, SIGNAL(triggered()), this, SLOT(duplicate()));
    connect(ui->actionExportFrame, SIGNAL(triggered()), this, SLOT(exportFrame()));
    connect(ui->actionExportFrameInCurrentLayer, SIGNAL(triggered()), this, SLOT(exportFrameInCurrentLayer()));
    connect(ui->actionAddSound, SIGNAL(triggered()), this, SLOT(addSound()));
    connect(ui->actionRemoveSound, SIGNAL(triggered()), this, SLOT(removeSound()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->actionExportLayer, SIGNAL(triggered()), this, SLOT(exportLayer()));
    connect(ui->actionMergeDown, SIGNAL(triggered()), this, SLOT(mergeDown()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionLoadLast, SIGNAL(triggered()), this, SLOT(loadLast()));
    connect(ui->actionOpacity, SIGNAL(triggered()), this, SLOT(setLayerOpacity()));
    connect(greenScreenSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeGreenScreenStrength()));
    connect(ui->actionImportVideo, SIGNAL(triggered()), this, SLOT(importVideo()));
    connect(ui->actionScan, SIGNAL(triggered()), this, SLOT(showScan()));
    connect(ui->actionDragMode, SIGNAL(triggered()), this, SLOT(toggleDragMode()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(ui->actionDuplicateAndAppend, SIGNAL(triggered()), this, SLOT(duplicateAndAppend()));
    connect(ui->actionMoveLayerUp, SIGNAL(triggered()), this, SLOT(moveLayerUp()));
    connect(ui->actionMoveLayerDown, SIGNAL(triggered()), this, SLOT(moveLayerDown()));
    connect(actionLayerName, SIGNAL(triggered()), this,                         SLOT(changeLayerName()));
    connect(timelineWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChangedSlot()));
    connect(ui->actionResize, SIGNAL(triggered()), this, SLOT(resizeAnim()));
}

void MainWindow::addImages(QStringList fileNames)
{
    fileNames.sort();
    QPixmap tempPixmap;
    QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
    if(isEmpty.at(currentLayer - 1))
    {
        tmpList.removeFirst();
        isEmpty.replace(currentLayer - 1, false);
    }
    for (int i = 0; i < fileNames.size(); i++)
    {
        tempPixmap.load(fileNames.at(i));
        tmpList.append(tempPixmap);
    }
    allImages.replace(currentLayer - 1, tmpList);
    calculateFrameCount();
    frame = tmpList.count();
    tmpList.clear();
    drawFrame();
    isSaved = false;
}

void MainWindow::addLayer()
{
    layerCount++;
    currentLayer++;
    QList<QPixmap> tempList;
    QPixmap pixmap = QPixmap(w, h);
    pixmap.fill(QColor(0,0,0,0));
    tempList.append(pixmap);
    allImages.insert(currentLayer - 1, tempList);
    isEmpty.insert(currentLayer - 1, true);
    isHidden.insert(currentLayer - 1, false);
    isMultiply.insert(currentLayer - 1, false);
    isGreenScreen.insert(currentLayer - 1, false);
    greenScreenStrengths.insert(currentLayer - 1, 235);
    opacityList.insert(currentLayer - 1, 255);
    QString newLayerStr = "New Layer";
    layerNames.insert(currentLayer - 1, newLayerStr);
    drawFrame();
    updateLayers();
    isSaved = false;
}

void MainWindow::addLayerBelow()
{
    layerCount++;
    QList<QPixmap> tempList;
    QPixmap pixmap = QPixmap(w, h);
    pixmap.fill(QColor(0,0,0,0));
    tempList.append(pixmap);
    allImages.insert(currentLayer - 1, tempList);
    isEmpty.insert(currentLayer - 1, true);
    isHidden.insert(currentLayer - 1, false);
    isMultiply.insert(currentLayer - 1, false);
    isGreenScreen.insert(currentLayer - 1, false);
    greenScreenStrengths.insert(currentLayer - 1, 235);
    opacityList.insert(currentLayer - 1, 255);
    QString newLayerStr = "New Layer";
    layerNames.insert(currentLayer - 1, newLayerStr);
    drawFrame();
    updateLayers();
    isSaved = false;
}

void MainWindow::makeWhiteTransparent()
{
    if (!isMultiply.at(currentLayer - 1))
        isMultiply.replace(currentLayer - 1, true);
    else
        isMultiply.replace(currentLayer - 1, false);
    updateLayers();
    drawFrame();
    isSaved = false;
}

void MainWindow::makeGreenScreen()
{
    if (!isGreenScreen.at(currentLayer - 1))
        isGreenScreen.replace(currentLayer - 1, true);
    else
        isGreenScreen.replace(currentLayer - 1, false);
    updateLayers();
    drawFrame();
    isSaved = false;
}

void MainWindow::hideLayer()
{
    if (!isHidden.at(currentLayer - 1))
        isHidden.replace(currentLayer - 1, true);
    else
        isHidden.replace(currentLayer - 1, false);
    updateLayers();
    drawFrame();
    isSaved = false;
}

void MainWindow::updateLayers()
{
    actionLayerName->setText(layerNames.at(currentLayer - 1));
    if(isMultiply.at(currentLayer - 1))
         ui->actionMultiply->setIcon(QIcon(":images/multiplySelected.svg"));
    else
         ui->actionMultiply->setIcon(QIcon(":images/multiply.svg"));
    if(isHidden.at(currentLayer - 1))
         ui->actionHide->setIcon(QIcon(":images/hidden.svg"));
    else
         ui->actionHide->setIcon(QIcon(":images/shown.svg"));
    if(isGreenScreen.at(currentLayer - 1)) {
        ui->actionGreenScreen->setIcon(QIcon(":images/greenscreenSelected.svg"));
        greenScreenSpinBox->setDisabled(false);
        greenScreenSpinBox->setValue(greenScreenStrengths.at(currentLayer - 1));
    } else {
        ui->actionGreenScreen->setIcon(QIcon(":images/greenscreen.svg"));
        greenScreenSpinBox->setDisabled(true);
    }
    timelineWidget->scrollToItem(timelineWidget->item(layerCount - currentLayer, frame - 1));
}

void MainWindow::removeLayer()
{
    if(layerCount > 1) {
        layerCount--;
        allImages.removeAt(currentLayer - 1);
        isEmpty.removeAt(currentLayer - 1);
        isMultiply.removeAt(currentLayer - 1);
        isGreenScreen.removeAt(currentLayer - 1);
        greenScreenStrengths.removeAt(currentLayer - 1);
        isHidden.removeAt(currentLayer - 1);
        layerNames.removeAt(currentLayer - 1);
        opacityList.removeAt(currentLayer - 1);
        currentLayer = 1;
    } else {
        allImages.clear();
        QPixmap pixmap(w, h);
        pixmap.fill(Qt::transparent);
        QList<QPixmap> tmpList;
        tmpList.clear();
        tmpList.append(pixmap);
        allImages.append(tmpList);
        isEmpty.clear();
        isEmpty.append(true);
        isMultiply.clear();
        isMultiply.append(false);
        isGreenScreen.clear();
        isGreenScreen.append(false);
        greenScreenStrengths.clear();
        greenScreenStrengths.append(235);
        isHidden.clear();
        isHidden.append(false);
        opacityList.clear();
        opacityList.append(255);
        layerNames.clear();
        layerNames.append(tr("New Layer"));
        currentLayer = 1;
    }
    calculateFrameCount();
    drawFrame();
    updateLayers();
    isSaved = false;
}

void MainWindow::layerUp()
{
    currentLayer++;
    if (currentLayer > layerCount)
       currentLayer = layerCount;
    updateLayers();
    drawFrame();
}

void MainWindow::layerDown()
{
    currentLayer--;
    if (currentLayer < 1)
        currentLayer = 1;
    updateLayers();
    drawFrame();
}

void MainWindow::layerTop()
{
    currentLayer = layerCount;
    updateLayers();
    drawFrame();
}

void MainWindow::layerBottom()
{
    currentLayer = 1;
    updateLayers();
    drawFrame();
}

void MainWindow::play()
{
    if(!isPlaying)
    {
        if(!soundFiles.isEmpty())
            for(int s = 0; s < soundFiles.count(); s++)
                soundFiles.at(s)->play();
        isPlaying = true;
        frame = 1;
        ui->actionPlay->setIcon(QIcon(":images/stop.svg"));
        for (int i = 0; i < frameCount; i++)
            timer->start(1000 / fps->value());
    }
    else
    {
        if(!soundFiles.isEmpty())
            for(int s = 0; s < soundFiles.count(); s++)
                soundFiles.at(s)->stop();
        isPlaying = false;
        ui->actionPlay->setIcon(QIcon(":images/play.svg"));
        timer->stop();
    }
}

void MainWindow::playNextFrame()
{
    frame++;
    if (frame > frameCount)
    {
        frame = 1;
        if(!soundFiles.isEmpty())
            for(int s = 0; s < soundFiles.count(); s++)
                soundFiles.at(s)->stop();
        if(!isLooping)
        {
            isPlaying = false;
            timer->stop();
            ui->actionPlay->setIcon(QIcon(":images/play.svg"));
        } else {
            if(!soundFiles.isEmpty())
                for(int s = 0; s < soundFiles.count(); s++)
                    soundFiles.at(s)->play();
        }
    }
    drawFrame();
}

void MainWindow::addFiles()
{
    fileNames = QFileDialog::getOpenFileNames(
                    this,
                    "Select one or more files to open",
                    QDir::homePath(),
                    "Images (*.png *.xpm *.jpg)");
    if (!fileNames.isEmpty())
        addImages(fileNames);
}

void MainWindow::showNextFrame()
{
    frame++;
    if(frame > frameCount)
        frame = frameCount;
    drawFrame();
}

void MainWindow::showPreviousFrame()
{
    frame--;
    if(frame < 1)
        frame = 1;
    drawFrame();
}

void MainWindow::showFirstFrame()
{
    frame = 1;
    drawFrame();
}

void MainWindow::showLastFrame()
{
    frame = frameCount;
    drawFrame();
}

void MainWindow::addEmptyFrame()
{
    QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
    QPixmap pixmap = QPixmap(w, h);
    pixmap.fill(QColor(0,0,0,0));
    tmpList.append(pixmap);
    allImages.replace(currentLayer - 1, tmpList);
    isEmpty.replace(currentLayer - 1, false);
    calculateFrameCount();
    frame = tmpList.count();
    drawFrame();
    isSaved = false;
}

void MainWindow::removeFrame()
{
    if(!isEmpty.at(currentLayer - 1))
    {
        QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
        if(frame <= tmpList.count()) {
            tmpList.removeAt(frame - 1);
            if(tmpList.size() == 0)
            {
                QPixmap pixmap = QPixmap(w, h);
                pixmap.fill(QColor(0,0,0,0));
                tmpList.append(pixmap);
                isEmpty.replace(currentLayer - 1, true);
            }
            allImages.replace(currentLayer - 1, tmpList);
            calculateFrameCount();
            if(frame > tmpList.size())
                frame--;
            drawFrame();
        }
        isSaved = false;
    }
}

void MainWindow::newAnim()
{
  if(isPlaying) {
    if(!soundFiles.isEmpty())
        for(int s = 0; s < soundFiles.count(); s++)
            soundFiles.at(s)->stop();
    isPlaying = false;
    ui->actionPlay->setIcon(QIcon(":images/play.svg"));
    timer->stop();
  }
  if(maybeSave()) {
    sizeDialog->setWidth(w);
    sizeDialog->setHeight(h);
    if (sizeDialog->exec() == QDialog::Accepted)
    {
       if (!isFirstRun)
       {
            delete scene;
            delete graphicsView;
            delete thumbnailScene;
            delete frameThumbnail;
        }
        isFirstRun = false;
        enableActions();

        w = sizeDialog->getWidth();
        h = sizeDialog->getHeight();

        thumbnailScene = new QGraphicsScene(this);
        thumbnailScene->setSceneRect(0, 0, int(w / 5), int(h / 5));
        frameThumbnail = new QGraphicsView(thumbnailScene);
        frameThumbnail->setSceneRect(0, 0, int(w / 5), int(h / 5));
        frameThumbnail->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        frameThumbnail->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        frameThumbnail->setFixedSize(int(w / 5), int(h / 5));
        ui->previewLayout->addWidget(frameThumbnail);
        
        timelineWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        timelineWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        timelineWidget->setIconSize(QSize(16, 24));
        timelineWidget->horizontalHeader()->setDefaultSectionSize(16);
        timelineWidget->verticalHeader()->setDefaultSectionSize(24);
        timelineWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        timelineWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        timelineWidget->setAutoScroll(true);
        timelineWidget->setAutoScrollMargin(20);
        timelineWidget->horizontalHeader()->hide();
        timelineWidget->setMinimumHeight(100);
        ui->timelineLayout->addWidget(timelineWidget);
        timelineWidget->show();

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
        QPixmap empty(w, h);
        empty.fill(Qt::white);
        item = new QGraphicsPixmapItem;
        item->setPixmap(empty);
        scene->addItem(item);
        item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);

        fileName = "";
        frame = 1;
        frameCount = 1;
        layerCount = 0;
        currentLayer = 0;
        soundFileNames.clear();
        soundFiles.clear();
        allImages.clear();
        thumbnailScene->clear();
        isLooping = false;
        isPlaying = false;
        isMergeDown = false;
        isRender = false;
        isEmpty.clear();
        isHidden.clear();
        isMultiply.clear();
        opacityList.clear();
        isGreenScreen.clear();
        greenScreenStrengths.clear();
        layerNames.clear();
        addLayer();
        updateLayers();
        isSaved = true;
        ui->timelineVerticalLayout->insertWidget(0, ui->frameToolBar);
        ui->frameToolBar->show();
        ui->timelineVerticalLayout->addWidget(ui->layerToolBar);
        ui->layerToolBar->show();
    }
  }
}

void MainWindow::render()
{
    QString frameNumberStr;
    QString imageFile;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        isRender = true;
        for (int i = 0; i < frameCount; i++)
        {
            frame = i + 1;
            drawFrame();
            std::stringstream ss;
            ss << std::setw(4) << std::setfill('0') << (i + 1);
            frameNumberStr = QString(ss.str().c_str());
            imageFile = dir + "/frame" + frameNumberStr +".png";
            scene->clearSelection();
            QPixmap pixmap = setupFrame();
            pixmap.save(imageFile, "PNG");
        }
        isRender = false;
    }
}

void MainWindow::renderVideo()
{
    QString frameNumberStr;
    QString imageFile;
    QString videoFileName = QFileDialog::getSaveFileName(this, tr("Render   Video"),
                    QDir::homePath(),
                    "AVI Videos (*.avi *.AVI)");
    if (!videoFileName.isEmpty())
    {
        if(!videoFileName.endsWith(".avi", Qt::CaseInsensitive))
            videoFileName += ".avi";
        isRender = true;
            if(tmpname.isEmpty())
                makeTmpName();
            makePath();
            QDir tmpdir(tmpDir);
            tmpdir.mkpath(path);
            QDir pathDir(path);
            pathDir.mkdir("video");
            QDir videoDir(path + "/video");
            if(!soundFiles.isEmpty()) 
                for(int s = 0; s < soundFiles.count(); s++)
                    QFile::copy(soundFileNames.at(s), tmpDir + "/soundfile" + QString::number(s) + ".wav");
            if(videoDir.exists()) {
                videoDir.removeRecursively();
                pathDir.mkdir("video");
            }
            if(!soundFiles.isEmpty()) {
                for(int s = 0; s < soundFiles.count(); s++) {
                    QFile::copy(tmpDir + "/soundfile" + QString::number(s) + ".wav", path + "/video/soundfile" + QString::number(s) + ".wav");
                    QFile::remove(tmpDir + "/soundfile" + QString::number(s) + ".wav");
                }
            }

        for (int i = 0; i < frameCount; i++)
        {
            frame = i + 1;
            drawFrame();
            std::stringstream ss;
            ss << std::setw(4) << std::setfill('0') << (i + 1);
            frameNumberStr = QString(ss.str().c_str());
            imageFile = path + "/video/frame" + frameNumberStr +".png";
            scene->clearSelection();
            QPixmap pixmap = setupFrame();
            pixmap.save(imageFile, "PNG");
        }
        QProcess *ffmpegCommand = new QProcess(this);
        QString frameRate;
        frameRate.setNum(fps->value());
        if(!soundFiles.isEmpty()) {
            if(soundFiles.count() > 1) {
                QStringList parameters;
                parameters << "-y";
                for(int s = 0; s < soundFiles.count(); s++) {
                    parameters << "-i" << path + "/video/soundfile" + QString::number(s) + ".wav";
                }
                parameters << "-filter_complex" << "amix=inputs=" + QString::number(soundFiles.count()) + ":duration=longest" << "-c:a" << "pcm_s16le" << path + "/video/soundfile.wav";
                ffmpegCommand->start("ffmpeg", parameters);
            } else {
                ffmpegCommand->start("ffmpeg", QStringList() << "-i" << path + "/video/soundfile0.wav" << "-c:a" << "pcm_s16le" << path + "/video/soundfile.wav");
            }
            if(ffmpegCommand->waitForFinished(-1)) {
                ffmpegCommand->start("ffmpeg", QStringList() << "-y" << "-r" << frameRate << "-i" << path + "/video/frame%04d.png" << "-i" << path + "/video/soundfile.wav" << "-c:v" << "huffyuv" << "-r" << "25" << videoFileName);
            }
        } else {
            ffmpegCommand->start("ffmpeg", QStringList() << "-y" << "-r" << frameRate << "-i" << path + "/video/frame%04d.png" << "-c:v" << "huffyuv" << "-r" << "25" << videoFileName);
        }
        if(ffmpegCommand->waitForFinished(-1)) {
           QDir tmpVideoDir(path + "/video");
           if(tmpVideoDir.exists())
               tmpVideoDir.removeRecursively();
       }
        isRender = false;
    }
}

void MainWindow::toggleLoop()
{
    if (isLooping)
    {
        isLooping = false;
        ui->actionToggleLoop->setIcon(QIcon(":images/loop.svg"));
    }
    else
    {
        isLooping = true;
        ui->actionToggleLoop->setIcon(QIcon(":images/loopSelected.svg"));   
    }
}

void MainWindow::drawFrame()
{
    timelineWidget->deleteLater();
    timelineWidget = new TableWidget;

    timelineWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    timelineWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    timelineWidget->setIconSize(QSize(16, 24));
    timelineWidget->horizontalHeader()->setDefaultSectionSize(16);
    timelineWidget->verticalHeader()->setDefaultSectionSize(24);
    timelineWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    timelineWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    timelineWidget->setAutoScroll(true);
    timelineWidget->setAutoScrollMargin(20);
    timelineWidget->horizontalHeader()->hide();
    timelineWidget->setMinimumHeight(100);
    ui->timelineLayout->addWidget(timelineWidget);
    connect(timelineWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChangedSlot()));

    QPixmap emptyFrame(16, 24);
    emptyFrame.fill(Qt::white);
    QPixmap usedFrame(16, 24);
    usedFrame.fill(Qt::white);
    QPainter framePainter(&usedFrame);
    framePainter.drawEllipse(6, 8, 4, 4);
    framePainter.end();
    QPixmap selectedFrame(16, 24);
    selectedFrame.fill(QColor(170, 170, 0));
    QPixmap selectedUsedFrame(16, 24);
    selectedUsedFrame.fill(QColor(170, 170, 0));
    QPainter selectedPainter(&selectedUsedFrame);
    selectedPainter.drawEllipse(6, 8, 4, 4);
    selectedPainter.end();
    timelineWidget->setRowCount(allImages.count());
    timelineWidget->setColumnCount(frameCount);
    for(int i = 0; i < layerNames.count(); i++) {
        QTableWidgetItem *tableWidgetItem = new QTableWidgetItem;
        tableWidgetItem->setText(QString::number(layerCount - i) + " - " + layerNames.at(i));
        timelineWidget->setVerticalHeaderItem(layerNames.count() - 1 - i, tableWidgetItem);
        QFont fontBold;
        fontBold.setWeight(QFont::Bold);
        QFont fontNormal;
        fontNormal.setWeight(QFont::Normal);
        if(i == currentLayer - 1)
            tableWidgetItem->setFont(fontBold);
        else
            tableWidgetItem->setFont(fontNormal);
    }
    for(int i = 0; i < allImages.count() ; i++) {
        QList<QPixmap> tmpList = allImages.at(i);
        for(int l = 0; l < frameCount; l++) {
            QTableWidgetItem *tableItem = new QTableWidgetItem();
            tableItem->setFlags(tableItem->flags() ^ Qt::ItemIsEditable);
            if(l < tmpList.count())
                tableItem->setIcon(QIcon(usedFrame));
            else
                tableItem->setIcon(QIcon(emptyFrame));
            if(i == currentLayer - 1 && l == frame - 1) {
                if( l < tmpList.count())
                    tableItem->setIcon(QIcon(selectedUsedFrame));
                else
                    tableItem->setIcon(QIcon(selectedFrame));
            }
            tableItem->setSizeHint(QSize(16, 24));
            timelineWidget->setItem(allImages.count() - 1 - i, l, tableItem);
        }
    }
    timelineWidget->scrollToItem(timelineWidget->item(layerCount - currentLayer, frame - 1));
    thumbnailScene->clear();
    QPixmap previewPixmap;
    int emptyLayers = 0;
    QList<QPixmap> previewTmpList = allImages.at(currentLayer - 1);
    if(frame <= previewTmpList.size())
         previewPixmap = previewTmpList.at(frame - 1).scaledToWidth(thumbnailScene->width());
    else
         previewPixmap = previewTmpList.last().scaledToWidth(thumbnailScene->width());
    QPixmap tmpPixmap(w, h);
    tmpPixmap.fill(QColor(255, 255, 255));
    QPainter painter;
    painter.begin(&tmpPixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    for (int i = 0; i < allImages.size(); i++)
    {
        QList<QPixmap> tempList = allImages.at(i);
        QPixmap drawnPixmap(w, h);
        if(frame <= tempList.size())
            drawnPixmap = tempList.at(frame - 1);
        else
            drawnPixmap = tempList.last();
        painter.setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));
        if(!isHidden.at(i))
        {
            QImage image;
            if(isGreenScreen.at(i)) {
                if(frame <= tempList.size())
                    image = tempList.at(frame - 1).toImage().convertToFormat(QImage::Format_ARGB32); 
                else
                    image = tempList.last().toImage().convertToFormat(QImage::Format_ARGB32);
                for(int y = 0; y < image.height(); ++y) {
                    for(int x = 0; x < image.width(); ++x) {
                        QRgb color = image.pixel(x, y);
                        int alpha = qAlpha(color);
                        if(qGreen(color) > qRed(color) + (255 - greenScreenStrengths.at(i)) && qGreen(color) > qBlue(color) + (255 - greenScreenStrengths.at(i))) {
                            alpha = 0;
                            QRgb newColor = qRgba(qRed(color), qGreen(color), qBlue(color), alpha);
                            image.setPixel(x, y, newColor);
                        }
                    }
                }
                drawnPixmap = QPixmap::fromImage(image);
            }
            if(isMultiply.at(i)) {
                painter.setCompositionMode(QPainter::CompositionMode_Multiply);
                painter.drawPixmap(0, 0, drawnPixmap);
                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            } else {
                painter.drawPixmap(0, 0, drawnPixmap);
            }
        }
        thumbnailScene->addPixmap(previewPixmap);
        calculateFrameCount();
        if (isEmpty.at(i))
            emptyLayers++;
    }
    painter.end();
    item->setPixmap(tmpPixmap);
    frameNumberStr.setNum(frame);
    if(frame < 10) {
        frameNumberLabel->setText("000" + frameNumberStr);
    } else if (frame >= 10 && frame < 100){
        frameNumberLabel->setText("00" + frameNumberStr);
    } else if (frame >=100 && frame < 1000) {
        frameNumberLabel->setText("0" + frameNumberStr);
    } else {
        frameNumberLabel->setText(frameNumberStr);
    }
}

QPixmap MainWindow::setupFrame()
{
    QPixmap tmpPixmap(w, h);
    if(!isMergeDown || isRender)
        tmpPixmap.fill(QColor(255, 255, 255));
    else
        tmpPixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&tmpPixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    for (int i = 0; i < allImages.size(); i++)
    {
        QList<QPixmap> tempList = allImages.at(i);
        QPixmap drawnPixmap(w, h);
        if(frame <= tempList.size())
            drawnPixmap = tempList.at(frame - 1);
        else
            drawnPixmap = tempList.last();
        QImage image;
        if(isGreenScreen.at(i)) {
            if(frame <= tempList.size())
                image = tempList.at(frame - 1).toImage().convertToFormat(QImage::Format_ARGB32); 
            else
                image = tempList.last().toImage().convertToFormat(QImage::Format_ARGB32);
            for(int y = 0; y < image.height(); ++y) {
                for(int x = 0; x < image.width(); ++x) {
                    QRgb color = image.pixel(x, y);
                    int alpha = qAlpha(color);
                    if(qGreen(color) > qRed(color) + (255 - greenScreenStrengths.at(i)) && qGreen(color) > qBlue(color) + (255 - greenScreenStrengths.at(i))) {
                        alpha = 0;
                        QRgb newColor = qRgba(qRed(color), qGreen(color), qBlue(color), alpha);
                        image.setPixel(x, y, newColor);
                    }
                }
            }
            drawnPixmap = QPixmap::fromImage(image);
        }
        painter.setOpacity(1.0 * qreal(qreal(opacityList.at(i)) / 255));
        if(!isHidden.at(i))
        {
            if(isMultiply.at(i)) {
                painter.setCompositionMode(QPainter::CompositionMode_Multiply);
                painter.drawPixmap(0, 0, drawnPixmap);
                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            } else {
                painter.drawPixmap(0, 0, drawnPixmap);
            }
        }
    }
    painter.end();
    return tmpPixmap;
}

void MainWindow::calculateFrameCount()
{
    frameCount = 0;
    for(int i = 0; i < allImages.count(); i++) {
            QList<QPixmap> tmpList = allImages.at(i);
            if (frameCount < tmpList.size())
                frameCount = tmpList.size();
    }
    QString frameCountStr;
    frameCountStr.setNum(frameCount);
    if(frameCount < 10) {
        frameCountLabel->setText("   000" + frameCountStr + " frames");
    } else if (frameCount >= 10 && frameCount < 100){
        frameCountLabel->setText("   00" + frameCountStr + " frames ");
    } else if (frameCount >=100 && frameCount < 1000) {
        frameCountLabel->setText("   0" + frameCountStr + " frames");
    } else {
        frameCountLabel->setText("   " + frameCountStr + " frames");
    }
}

void MainWindow::showVideo()
{
    videoDialog = new VideoDialog(this);
    videoDialog->setMainWindow(this);
    videoDialog->setWidth(w);
    videoDialog->setHeight(h);
    videoDialog->newAnim();
    videoDialog->show();
    if(videoDialog->isVisible()) {
        ui->actionNew->setDisabled(true);
        ui->actionOpen->setDisabled(true);
        ui->actionLoadLast->setDisabled(true);
        ui->actionSave->setDisabled(true);
        ui->actionSaveAs->setDisabled(true);
        ui->actionRemoveLayer->setDisabled(true);
        ui->actionRender->setDisabled(true);
        ui->actionRenderVideo->setDisabled(true);
        ui->actionMultiply->setDisabled(true);
        ui->actionGreenScreen->setDisabled(true);
        ui->actionAddImages->setDisabled(true);
        ui->actionHide->setDisabled(true);
        ui->actionVideo->setDisabled(true);
        ui->actionCutout->setDisabled(true);
        ui->actionDrawing->setDisabled(true);
        ui->actionInsert->setDisabled(true);
        ui->actionReplace->setDisabled(true);
        ui->actionExportFrame->setDisabled(true);
        ui->actionExportFrameInCurrentLayer->setDisabled(true);
        ui->actionExportLayer->setDisabled(true);
        ui->actionMergeDown->setDisabled(true);
        ui->actionImportVideo->setDisabled(true);
        ui->actionScan->setDisabled(true);
        ui->actionDragMode->setDisabled(true);
        ui->actionZoomIn->setDisabled(true);
        ui->actionZoomOut->setDisabled(true);
        ui->actionMoveLayerUp->setDisabled(true);
        ui->actionMoveLayerDown->setDisabled(true);
        actionLayerName->setDisabled(true);
        ui->actionResize->setDisabled(true);
    }
}

void MainWindow::showScan()
{
    scanDialog = new ScanDialog(this);
    scanDialog->setMainWindow(this);
    scanDialog->setWidth(w);
    scanDialog->setHeight(h);
    scanDialog->newImage();
    scanDialog->show();
    if(scanDialog->isVisible()) {
        ui->actionNew->setDisabled(true);
        ui->actionOpen->setDisabled(true);
        ui->actionLoadLast->setDisabled(true);
        ui->actionSave->setDisabled(true);
        ui->actionSaveAs->setDisabled(true);
        ui->actionRender->setDisabled(true);
        ui->actionRenderVideo->setDisabled(true);
        ui->actionMultiply->setDisabled(true);
        ui->actionGreenScreen->setDisabled(true);
        ui->actionAddImages->setDisabled(true);
        ui->actionHide->setDisabled(true);
        ui->actionVideo->setDisabled(true);
        ui->actionCutout->setDisabled(true);
        ui->actionDrawing->setDisabled(true);
        ui->actionInsert->setDisabled(true);
        ui->actionReplace->setDisabled(true);
        ui->actionExportFrame->setDisabled(true);
        ui->actionExportFrameInCurrentLayer->setDisabled(true);
        ui->actionExportLayer->setDisabled(true);
        ui->actionMergeDown->setDisabled(true);
        ui->actionImportVideo->setDisabled(true);
        ui->actionScan->setDisabled(true);
        ui->actionDragMode->setDisabled(true);
        ui->actionZoomIn->setDisabled(true);
        ui->actionZoomOut->setDisabled(true);
        ui->actionMoveLayerUp->setDisabled(true);
        ui->actionMoveLayerDown->setDisabled(true);
        actionLayerName->setDisabled(true);
        ui->actionResize->setDisabled(true);
    }
}

void MainWindow::showDrawing()
{
    drawingDialog = new DrawingDialog(this);
    drawingDialog->setMainWindow(this);
    drawingDialog->setWidth(w);
    drawingDialog->setHeight(h);
    drawingDialog->newImage();
    drawingDialog->setImageOpacity(opacityList.at(currentLayer - 1));
    if(frame <= allImages.at(currentLayer - 1).count()) {
        drawingDialog->setPixmap(allImages.at(currentLayer - 1).at(frame - 1));
        if(frame > 1)
            drawingDialog->setBackFrame(allImages.at(currentLayer - 1).at(frame - 2));
        if(frame > 2)
            drawingDialog->setFirstFrame(allImages.at(currentLayer - 1).at(frame - 3));
        if(frame < allImages.at(currentLayer - 1).count())
            drawingDialog->setForwardFrame(allImages.at(currentLayer - 1).at(frame));
        if(frame < allImages.at(currentLayer - 1).count() - 1)
            drawingDialog->setLastFrame(allImages.at(currentLayer - 1).at(frame + 1));
    } else {
        QPixmap pixmap(w, h);
        pixmap.fill(Qt::transparent);
        drawingDialog->setPixmap(pixmap);
        QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
        drawingDialog->setBackFrame(tmpList.at(tmpList.count() - 1));
        if(tmpList.count() > 1)
            drawingDialog->setFirstFrame(tmpList.at(tmpList.count() - 2));
    }
    drawingDialog->show();
    if(drawingDialog->isVisible()) {
        ui->actionNew->setDisabled(true);
        ui->actionOpen->setDisabled(true);
        ui->actionLoadLast->setDisabled(true);
        ui->actionSave->setDisabled(true);
        ui->actionSaveAs->setDisabled(true);
        ui->actionRender->setDisabled(true);
        ui->actionRenderVideo->setDisabled(true);
        ui->actionMultiply->setDisabled(true);
        ui->actionGreenScreen->setDisabled(true);
        ui->actionAddImages->setDisabled(true);
        ui->actionHide->setDisabled(true);
        ui->actionVideo->setDisabled(true);
        ui->actionCutout->setDisabled(true);
        ui->actionDrawing->setDisabled(true);
        ui->actionInsert->setDisabled(true);
        ui->actionReplace->setDisabled(true);
        ui->actionExportFrame->setDisabled(true);
        ui->actionExportFrameInCurrentLayer->setDisabled(true);
        ui->actionExportLayer->setDisabled(true);
        ui->actionMergeDown->setDisabled(true);
        ui->actionImportVideo->setDisabled(true);
        ui->actionScan->setDisabled(true);
        ui->actionDragMode->setDisabled(true);
        ui->actionZoomIn->setDisabled(true);
        ui->actionZoomOut->setDisabled(true);
        ui->actionMoveLayerUp->setDisabled(true);
        ui->actionMoveLayerDown->setDisabled(true);
        actionLayerName->setDisabled(true);
        ui->actionResize->setDisabled(true);
    }
}

void MainWindow::showCutout()
{
    cutoutDialog = new CutoutDialog(this);
    cutoutDialog->setMainWindow(this);
    cutoutDialog->setWidth(w);
    cutoutDialog->setHeight(h);
    cutoutDialog->newAnim();
    cutoutDialog->show();
    if(cutoutDialog->isVisible()) {
        ui->actionNew->setDisabled(true);
        ui->actionOpen->setDisabled(true);
        ui->actionLoadLast->setDisabled(true);
        ui->actionSave->setDisabled(true);
        ui->actionSaveAs->setDisabled(true);
        ui->actionRemoveLayer->setDisabled(true);
        ui->actionRender->setDisabled(true);
        ui->actionRenderVideo->setDisabled(true);
        ui->actionMultiply->setDisabled(true);
        ui->actionGreenScreen->setDisabled(true);
        ui->actionAddImages->setDisabled(true);
        ui->actionHide->setDisabled(true);
        ui->actionVideo->setDisabled(true);
        ui->actionCutout->setDisabled(true);
        ui->actionDrawing->setDisabled(true);
        ui->actionInsert->setDisabled(true);
        ui->actionReplace->setDisabled(true);
        ui->actionExportFrame->setDisabled(true);
        ui->actionExportFrameInCurrentLayer->setDisabled(true);
        ui->actionExportLayer->setDisabled(true);
        ui->actionMergeDown->setDisabled(true);
        ui->actionImportVideo->setDisabled(true);
        ui->actionScan->setDisabled(true);
        ui->actionDragMode->setDisabled(true);
        ui->actionZoomIn->setDisabled(true);
        ui->actionZoomOut->setDisabled(true);
        ui->actionMoveLayerUp->setDisabled(true);
        ui->actionMoveLayerDown->setDisabled(true);
        actionLayerName->setDisabled(true);
        ui->actionResize->setDisabled(true);
    }
}

void MainWindow::addImage(QPixmap pixmap)
{
    frame = 1;
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.drawPixmap(0, 0, pixmap);
    painter.end();
    QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
    tmpList.append(pix);
    if(isEmpty.at(currentLayer - 1))
    {
        tmpList.removeFirst();
        isEmpty.replace(currentLayer - 1, false);
    }
    frame = tmpList.count();
    allImages.replace(currentLayer - 1, tmpList);
    calculateFrameCount();
    tmpList.clear();
    drawFrame();
    updateLayers();
    isSaved = false;
}

void MainWindow::replaceImage(QPixmap pixmap)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.drawPixmap(0, 0, pixmap);
    painter.end();
    QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
    if(frame <= tmpList.count()) {
        tmpList.replace(frame - 1, pix);
        if(isEmpty.at(currentLayer - 1))
        {
            isEmpty.replace(currentLayer - 1, false);
        }
    } else {
        tmpList.append(pix);
        if(isEmpty.at(currentLayer - 1))
        {
            tmpList.removeFirst();
            isEmpty.replace(currentLayer - 1, false);
        }
    }
    allImages.replace(currentLayer - 1, tmpList);
    calculateFrameCount();
    tmpList.clear();
    drawFrame();
    updateLayers();
    isSaved = false;
}

void MainWindow::insertImage(QPixmap pixmap)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.drawPixmap(0, 0, pixmap);
    painter.end();
    QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
    if(frame <= tmpList.count()) {
        tmpList.insert(frame - 1, pix);
        if(isEmpty.at(currentLayer - 1))
        {
            isEmpty.replace(currentLayer - 1, false);
            tmpList.removeLast();
        }
    } else {
        tmpList.append(pix);
         if(isEmpty.at(currentLayer - 1))
        {
            tmpList.removeFirst();
            isEmpty.replace(currentLayer - 1, false);
        }
    }       
    if(isEmpty.at(currentLayer - 1))
    {
        isEmpty.replace(currentLayer - 1, false);
        tmpList.removeLast();
    }
    allImages.replace(currentLayer - 1, tmpList);
    calculateFrameCount();
    tmpList.clear();
    drawFrame();
    updateLayers();
    isSaved = false;
}

void MainWindow::changeLayerName()
{
    layerNameDialog->setName(actionLayerName->text());
    if(layerNameDialog->exec() == QDialog::Accepted) {
        QString currentLayerName = layerNameDialog->getName();
        layerNames.replace(currentLayer - 1, currentLayerName);
        updateLayers();
        isSaved = false;
        drawFrame();
    }
}

void MainWindow::insertImages(QStringList fileNames)
{
    fileNames.sort();
    QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
    if(isEmpty.at(currentLayer - 1))
        tmpList.removeFirst();
    for (int i = 0; i < fileNames.size(); i++)
    {
        QPixmap tempPixmap;
        tempPixmap.load(fileNames.at(i));
            if(frame <= tmpList.size()) {
                tmpList.insert(frame - 1 + i, tempPixmap);
            } else {
                tmpList.append(tempPixmap);
            }
    }
    if(isReplace) {
        tmpList.removeAt(frame - 1 + fileNames.size());
        isReplace = false;
    } 
    allImages.replace(currentLayer - 1, tmpList);
    isEmpty.replace(currentLayer - 1, false);
    calculateFrameCount();
    tmpList.clear();
    drawFrame();
    isSaved = false;
}

void MainWindow::duplicate()
{
    if(!isEmpty.at(currentLayer - 1)) {
        QPixmap tempPixmap;
        QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
        if(frame <= tmpList.count()) {
            tempPixmap = tmpList.at(frame - 1);
            tmpList.insert(frame - 1, tempPixmap);
        } else {
            tempPixmap = tmpList.last();
            tmpList.append(tempPixmap);
        }    
        allImages.replace(currentLayer - 1, tmpList);
        calculateFrameCount();
        tmpList.clear();
        drawFrame();
        isSaved = false;
    }
}

void MainWindow::duplicateAndAppend()
{
    if(!isEmpty.at(currentLayer - 1)) {
        QPixmap tempPixmap;
        QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
        if(frame <= tmpList.count()) {
            tempPixmap = tmpList.at(frame - 1);
            tmpList.append(tempPixmap);
        } else {
            tempPixmap = tmpList.last();
            tmpList.append(tempPixmap);
        }    
        allImages.replace(currentLayer - 1, tmpList);
        calculateFrameCount();
        tmpList.clear();
        drawFrame();
        isSaved = false;
    }
}

void MainWindow::replace()
{
    QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
    if(frame <= tmpList.size()) {
        QStringList fileNames = QFileDialog::getOpenFileNames(
                        this,
                        "Select one or more files to open",
                        QDir::homePath(),
                        "Images (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG)");
        if (!fileNames.isEmpty())
        {
            isReplace = true;
            insertImages(fileNames);
            drawFrame();
            isSaved = false;
        }
    }
}

void MainWindow::insertFiles()
{
    fileNames = QFileDialog::getOpenFileNames(
                    this,
                    "Select one or more files to open",
                    QDir::homePath(),
                    "Images (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG)");
    if (!fileNames.isEmpty())
        insertImages(fileNames);
}

void MainWindow::insertEmptyFrame()
{
    QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
    QPixmap pixmap = QPixmap(w, h);
    pixmap.fill(QColor(0,0,0,0));
    if(isEmpty.at(currentLayer - 1)) {
        tmpList.removeFirst();
        tmpList.append(pixmap);
        isEmpty.replace(currentLayer - 1, false);
    }
    if(frame <= tmpList.count())
        tmpList.insert(frame - 1, pixmap);
    else
        tmpList.append(pixmap);
    allImages.replace(currentLayer - 1, tmpList);
    calculateFrameCount();
    tmpList.clear();
    drawFrame();
    isSaved = false;
}

void MainWindow::exportFrame()
{
        QString exportFileName = QFileDialog::getSaveFileName(this, "Export     Frame", QDir::homePath(), "PNG Files (*.png *.PNG)");
        if(!exportFileName.isEmpty()) {
            if(!exportFileName.endsWith("png", Qt::CaseInsensitive)) {
                exportFileName = exportFileName + ".png";
            }
            QPixmap pixmap = setupFrame();
            pixmap.save(exportFileName);
        }
}

void MainWindow::exportFrameInCurrentLayer()
{
    if(frame <= allImages.at(currentLayer - 1).count()) {
        QString exportFileName = QFileDialog::getSaveFileName(this, "Export     Frame", QDir::homePath(), "PNG Files (*.png *.PNG)");
        if(!exportFileName.isEmpty()) {
            if(!exportFileName.endsWith("png", Qt::CaseInsensitive)) {
                exportFileName = exportFileName + ".png";
            }
            QPixmap pixmap = allImages.at(currentLayer - 1).at(frame - 1);
            pixmap.save(exportFileName);
        }
    }
}

void MainWindow::addSound()
{
    QString soundFileName = QFileDialog::getOpenFileName(
                this,
                "Select a file to open",
                QDir::homePath(),
                "WAV files (*.wav *.WAV)");
    if(!soundFileName.isEmpty()) {
        soundFileNames.append(soundFileName);
        QMediaPlayer *sound = new QMediaPlayer(this);
        sound->setMedia(QUrl::fromLocalFile(soundFileName));
        sound->setVolume(100);
        soundFiles.append(sound);
    }
    isSaved = false;
}

void MainWindow::removeSound()
{
    if(soundFiles.count() > 0) {
        soundFileNames.removeLast();
        soundFiles.removeLast();
    } else {
        soundFileNames.clear();
        soundFiles.clear();
    }
    isSaved = false;
}

void MainWindow::enableActions()
{
        ui->actionNew->setDisabled(false);
        ui->actionOpen->setDisabled(false);
        ui->actionLoadLast->setDisabled(false);
        ui->actionSave->setDisabled(false);
        ui->actionSaveAs->setDisabled(false);
        ui->actionAddEmpty->setDisabled(false);
        ui->actionAddLayer->setDisabled(false);
        ui->actionAddLayerBelow->setDisabled(false);
        ui->actionDown->setDisabled(false);
        ui->actionNext_Frame->setDisabled(false);
        ui->actionPlay->setDisabled(false);
        ui->actionPrevious_Frame->setDisabled(false);
        ui->actionRemoveLayer->setDisabled(false);
        ui->actionRender->setDisabled(false);
        ui->actionToggleLoop->setDisabled(false);
        ui->actionUp->setDisabled(false);
        ui->actionMultiply->setDisabled(false);
        ui->actionGreenScreen->setDisabled(false);
        ui->actionFirst->setDisabled(false);
        ui->actionLast->setDisabled(false);
        ui->actionTopLayer->setDisabled(false);
        ui->actionBottomLayer->setDisabled(false);
        ui->actionDelete->setDisabled(false);
        ui->actionAddImages->setDisabled(false);
        ui->actionHide->setDisabled(false);
        ui->actionVideo->setDisabled(false);
        ui->actionDrawing->setDisabled(false);
        ui->actionCutout->setDisabled(false);
        ui->actionInsert->setDisabled(false);
        ui->actionInsertEmpty->setDisabled(false);
        ui->actionDuplicate->setDisabled(false);
        ui->actionReplace->setDisabled(false);
        ui->actionExportFrame->setDisabled(false);
        ui->actionExportFrameInCurrentLayer->setDisabled(false);
        ui->actionExportLayer->setDisabled(false);
        ui->actionAddSound->setDisabled(false);
        ui->actionRemoveSound->setDisabled(false);
        ui->actionMergeDown->setDisabled(false);
        ui->actionOpacity->setDisabled(false);
        ui->actionScan->setDisabled(false);
        ui->actionDragMode->setDisabled(false);
        ui->actionZoomIn->setDisabled(false);
        ui->actionZoomOut->setDisabled(false);
        ui->actionDuplicateAndAppend->setDisabled(false);
        ui->actionMoveLayerUp->setDisabled(false);
        ui->actionMoveLayerDown->setDisabled(false);
        actionLayerName->setDisabled(false);
        ui->actionResize->setDisabled(false);
        QProcess *ffmpegTest = new QProcess(this);
        ffmpegTest->start("ffmpeg");
        if(ffmpegTest->waitForStarted(-1))
        {
            ui->actionRenderVideo->setDisabled(false);
            ui->actionImportVideo->setDisabled(false);
        } else {
            ui->actionRenderVideo->setDisabled(true);
            ui->actionImportVideo->setDisabled(true);
        }
}

void MainWindow::removeLast()
{
    if(!isEmpty.at(currentLayer - 1))
    {
        QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
        tmpList.removeLast();
        if(tmpList.size() == 0)
        {
            QPixmap pixmap = QPixmap(w, h);
            pixmap.fill(QColor(0,0,0,0));
            tmpList.append(pixmap);
            isEmpty.replace(currentLayer - 1, true);
        }
        allImages.replace(currentLayer - 1, tmpList);
        calculateFrameCount();
        if(frame > tmpList.size())
            frame--;
        drawFrame();
        isSaved = false;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave()) {
        makePath();
        QDir oldDir(path);
        if(oldDir.exists()) {
            oldDir.removeRecursively();
        }
        settings = new QSettings;
        settings->setValue("BrushPaintRed", 0);
        settings->setValue("BrushPaintGreen", 0);
        settings->setValue("BrushPaintBlue", 0);
        settings->setValue("BrushPaintAlpha", 255);
        settings->setValue("BrushSize", 10);
        settings->setValue("BrushType", "Brush");
        sizeDialog->deleteLater();
        layerNameDialog->deleteLater();
        cutoutDialog->deleteLater();
        videoDialog->deleteLater();
        drawingDialog->deleteLater();
        opacityDialog->deleteLater();
        scanDialog->deleteLater();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::quit()
{
    if(maybeSave()) {
        makePath();
        QDir oldDir(path);
        if(oldDir.exists()) {
            oldDir.removeRecursively();
        }
        settings = new QSettings;
        settings->setValue("BrushPaintRed", 0);
        settings->setValue("BrushPaintGreen", 0);
        settings->setValue("BrushPaintBlue", 0);
        settings->setValue("BrushPaintAlpha", 255);
        settings->setValue("BrushSize", 10);
        settings->setValue("BrushType", "Brush");
        sizeDialog->deleteLater();
        layerNameDialog->deleteLater();
        cutoutDialog->deleteLater();
        videoDialog->deleteLater();
        drawingDialog->deleteLater();
        opacityDialog->deleteLater();
        scanDialog->deleteLater();
        qApp->quit();
    }
}

void MainWindow::exportLayer()
{
    QString frameNumberStr;
    QString imageFile;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
        for (int i = 0; i < tmpList.count(); i++)
        {
            frame = i + 1;
            drawFrame();
            std::stringstream ss;
            ss << std::setw(4) << std::setfill('0') << (i + 1);
            frameNumberStr = QString(ss.str().c_str());
            imageFile = dir + "/" + layerNames.at(currentLayer - 1) + frameNumberStr +".png";
            QPixmap pixmap(w, h);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setOpacity(1.0 * qreal(qreal(opacityList.at(currentLayer - 1)) / 255));
            painter.drawPixmap(0, 0, tmpList.at(i));
            painter.end();
            pixmap.save(imageFile, "PNG");
        }
    }
}

void MainWindow::mergeDown()
{
    if(currentLayer > 1) {
        isMergeDown = true;
        int frameBackup = frame;
        QList<QPixmap> tmpList = allImages.at(currentLayer - 1);
        QList<QPixmap> mergedList = allImages.at(currentLayer - 2);
        QList<bool> tmpHidden = isHidden;
        for(int i = 0; i < layerCount; i++)
            if(i != currentLayer - 1 && i != currentLayer - 2)
                isHidden.replace(i, true);
        int mergeFrameCount = tmpList.count();
        if(mergedList.count() > mergeFrameCount)
            mergeFrameCount = mergedList.count();
        for(int i = 0; i < mergeFrameCount; i++) {
            frame = i + 1;
            QPixmap pixmap = setupFrame();
            if(i < tmpList.count())
                tmpList.replace(i, pixmap);
            else
                tmpList.append(pixmap);
        }
        allImages.replace(currentLayer - 1, tmpList);
        currentLayer--;
        removeLayer();
        isHidden = tmpHidden;
        frame = frameBackup;
        drawFrame();
        isMergeDown = false;
    }
}

void MainWindow::makeTmpName()
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

void MainWindow::makePath()
{
    path = tmpDir + tmpname;
}

bool MainWindow::maybeSave()
{
    if (!isSaved) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Oinut"),
                    tr("The document hasn't been saved.\n"
                    "Do you want to save your changes?"),
                    QMessageBox::Save | QMessageBox::Cancel | QMessageBox::     Discard);
        if (ret == QMessageBox::Save)
            save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::getFileName()
{   
    fileName = QFileDialog::getOpenFileName(
                this,
                "Select a file to open",
                QDir::homePath(),
                "OINUT Files (*.oin *.OIN)");
}

void MainWindow::open()
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

void MainWindow::save()
{
    if(fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(
            this,
            "Save File",
            QDir::homePath(),
            "OINUT Files (*.oin *.OIN)");
    }
    if(!fileName.isEmpty()) {
        if(!fileName.endsWith(".oin", Qt::CaseInsensitive)) {
            fileName += ".oin";
        }
        settings = new QSettings;
        settings->setValue("LastFile", fileName);
        isSaved = true;
        QDir dir(tmpDir);
        makePath();
        QDir fileDir(path);
        if(!soundFiles.isEmpty())
            for(int s = 0; s < soundFiles.count(); s++)
                QFile::copy(soundFileNames.at(s), tmpDir + "/soundfile" + QString::number(s) + ".wav");
        if(fileDir.exists()) 
            fileDir.removeRecursively();
        dir.mkpath(path);
        if(!soundFiles.isEmpty()) { 
            for(int s = 0; s < soundFiles.count(); s++) {
                QFile::copy(tmpDir + "/soundfile" + QString::number(s) + ".wav", path + "/soundfile" + QString::number(s) + ".wav");
                QFile::remove(tmpDir + "/soundfile" + QString::number(s) + ".wav");
            }
            }
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
        xmlWriter.writeAttribute("extension", "oin");
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("DocumentSize");
        xmlWriter.writeAttribute("width", QString::number(w));
        xmlWriter.writeAttribute("height", QString::number(h));
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("ItemList");
        for(int i = 0; i < allImages.count(); i++)
        {
            xmlWriter.writeStartElement("Layer");
            xmlWriter.writeAttribute("layernum", QString::number(i));
            xmlWriter.writeAttribute("layername", layerNames.at(i));
            xmlWriter.writeAttribute("isempty", QString::number(isEmpty.at(i)));
            xmlWriter.writeAttribute("ismultiply", QString::number(isMultiply.at(i)));
            xmlWriter.writeAttribute("isgreenscreen", QString::number(isGreenScreen.at(i)));
            xmlWriter.writeAttribute("greenscreenStrength", QString::number(greenScreenStrengths.at(i)));
            xmlWriter.writeAttribute("ishidden", QString::number(isHidden.at(i)));
            xmlWriter.writeAttribute("opacity", QString::number(opacityList.at(i)));
            QList<QPixmap> tmpList = allImages.at(i);
            QStringList frames;
            for(int l = 0; l < tmpList.count(); l++) {
                QPixmap pixmap(w, h);
                std::stringstream ss;
                ss << std::setw(4) << std::setfill('0') << (l + 1);
                QString frameNumberStr = QString(ss.str().c_str());
                QString layerNumberStr;
                layerNumberStr.setNum(i);
                QString pixmapFile = layerNumberStr + "+frame" + frameNumberStr +".png";
                pixmap = tmpList.at(l);
                pixmap.save(path + "/" + pixmapFile);
                frames.append(pixmapFile);
            }
            xmlWriter.writeAttribute("frames", frames.join("@"));
            xmlWriter.writeEndElement();
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
        QStringList qsl = zipDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs|   QDir::Files);
        foreach (QString file, qsl) {
            QFileInfo finfo(QString("%1/%2").arg(zipDir.path()).arg(file));
            if (finfo.isDir()) {
                QDir sd(finfo.filePath());
                QStringList qslBelow = sd.entryList(QDir::NoDotAndDotDot |      QDir::Files);
                foreach (QString fileBelow, qslBelow) {
                    QFileInfo fbinfo(QString("%1/%2").arg(sd.path()).           arg(fileBelow));
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
            QString fileNameWithRelativePath = fileInfo.filePath().remove(0,    zipDir.absolutePath().length() + 1);
            inFile.setFileName(fileInfo.filePath());
            if(!inFile.open(QIODevice::ReadOnly)) {
                return;
            }
            if(!outFile.open(QIODevice::WriteOnly,                              QuaZipNewInfo(fileNameWithRelativePath, fileInfo.filePath()))) {
                return;
            }
            while(inFile.getChar(&c) && outFile.putChar(c));

            outFile.close();
            inFile.close();
        }
        isSaved = true;
    }
}

void MainWindow::load()
{
    if (!fileName.isEmpty()) {
        if(fileName.endsWith("oin", Qt::CaseInsensitive)) {
            makeTmpName();
            allImages.clear();
            QList<QGraphicsItem *> pixmapList;
            settings = new QSettings;
            isSaved = true;
            settings->setValue("LastFile", fileName);
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
                        if(xmlReader.attributes().value("extension").           toString() != "oin") {
                            QMessageBox::warning(this, tr("Oinut"), tr("File is not a Oinut file."), QMessageBox::Ok);
                            return;
                        }
                    }
                    if(xmlReader.name().toString() == "DocumentSize") {
                        enableActions();
                        w = xmlReader.attributes().value("width").toString().   toInt();
                        h = xmlReader.attributes().value("height").toString().  toInt();
                        if(!isFirstRun) {
                            delete scene;
                            delete graphicsView;
                            delete thumbnailScene;
                            delete frameThumbnail;
                        }
                        isFirstRun = false;
                        enableActions();

                        thumbnailScene = new QGraphicsScene(this);
                        thumbnailScene->setSceneRect(0, 0, int(w / 5), int(h / 5));
                        frameThumbnail = new QGraphicsView(thumbnailScene);
                        frameThumbnail->setSceneRect(0, 0, int(w / 5), int(h / 5));
                        frameThumbnail->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        frameThumbnail->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        frameThumbnail->setFixedSize(int(w / 5), int(h / 5));
                        ui->previewLayout->addWidget(frameThumbnail);

                        timelineWidget->setSelectionMode(QAbstractItemView::SingleSelection);
                        timelineWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
                        timelineWidget->setIconSize(QSize(16, 24));
                        timelineWidget->horizontalHeader()->setDefaultSectionSize(16);
                        timelineWidget->verticalHeader()->setDefaultSectionSize(24);
                        timelineWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                        timelineWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                        timelineWidget->setAutoScroll(true);
                        timelineWidget->setAutoScrollMargin(20);
                        timelineWidget->horizontalHeader()->hide();
                        timelineWidget->setMinimumHeight(100);
                        ui->timelineLayout->addWidget(timelineWidget);
                        timelineWidget->show();
                        ui->timelineVerticalLayout->insertWidget(0, ui->frameToolBar);
                        ui->frameToolBar->show();
                        ui->timelineVerticalLayout->addWidget(ui->layerToolBar);
                        ui->layerToolBar->show();

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
                        QPixmap empty(w, h);
                        empty.fill(Qt::white);
                        item = new QGraphicsPixmapItem;
                        item->setPixmap(empty);
                        scene->addItem(item);
                        item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);

                        frame = 1;
                        frameCount = 1;
                        layerCount = 0;
                        currentLayer = 0;
                        soundFileNames.clear();
                        soundFiles.clear();
                        allImages.clear();
                        thumbnailScene->clear();
                        isLooping = false;
                        isPlaying = false;
                        isMergeDown = false;
                        isRender = false;
                        isEmpty.clear();
                        isHidden.clear();
                        isMultiply.clear();
                        opacityList.clear();
                        isGreenScreen.clear();
                        greenScreenStrengths.clear();
                        layerNames.clear();
                    }
                    if(xmlReader.name().toString() == "Layer")
                    {
                        addLayer();
                        QString framesList = xmlReader.attributes().value("frames").toString();
                        QStringList frames = framesList.split("@");
                        for(int k = 0; k < frames.count(); k++)
                            frames.replace(k, path + "/" + frames.at(k));
                        addImages(frames);
                        QString isLayerHidden = xmlReader.attributes().value("ishidden").toString();
                        if(isLayerHidden == "1")
                            isHidden.replace(currentLayer - 1, true);
                        else
                            isHidden.replace(currentLayer - 1, false);
                        QString isLayerEmpty = xmlReader.attributes().value("isempty").toString();
                        if(isLayerEmpty == "1")
                            isEmpty.replace(currentLayer - 1, true);
                        else
                            isEmpty.replace(currentLayer - 1, false);
                        QString isLayerMultiply = xmlReader.attributes().value("ismultiply").toString();
                        if(isLayerMultiply == "1")
                            isMultiply.replace(currentLayer - 1, true);
                        else
                            isMultiply.replace(currentLayer - 1, false);
                        QString isLayerGreenScreen = xmlReader.attributes().value("isgreenscreen").toString();
                        if(isLayerGreenScreen == "1")
                            isGreenScreen.replace(currentLayer - 1, true);
                        else
                            isGreenScreen.replace(currentLayer - 1, false);
                        int layerGreenScreenStrength = xmlReader.attributes().value("greenscreenStrength").toString().toInt();
                        greenScreenStrengths.replace(currentLayer - 1, layerGreenScreenStrength);
                        QString layerName = xmlReader.attributes().value("layername").toString();
                        layerNames.replace(currentLayer - 1, layerName);
                        int layerOpacity = xmlReader.attributes().value("opacity").toString().toInt();
                        opacityList.replace(currentLayer - 1, layerOpacity);
                    }
                }
            }
            sceneData.close();
            QDir extractedDir(path);
            QStringList fileList = extractedDir.entryList();
            for(int l = 0; l < fileList.count(); l++) {
                if(fileList.at(l).startsWith("soundfile")) {
                    QString soundFileName = path + "/" + fileList.at(l);
                    soundFileNames.append(soundFileName);
                    QMediaPlayer *sound = new QMediaPlayer(this);
                    sound->setMedia(QUrl::fromLocalFile(soundFileName));
                    sound->setVolume(100);
                    soundFiles.append(sound);
                }
            }
        } else {
                QMessageBox::warning(this, tr("Oinut"),
                    tr("File is not an Oinut file."),
                    QMessageBox::Ok);
                return;
        }
        isSaved = true;
        if(layerCount == 0)
            addLayer();
        drawFrame();
        updateLayers();
    }
}

void MainWindow::saveAs()
{
    fileName = "";
    save();
}

void MainWindow::initialize()
{
    show();
    QStringList arguments = QCoreApplication::arguments();
    if(arguments.count() == 2)
    {
         fileName = arguments.at(1);
         load();
    }
}

void MainWindow::loadLast()
{
    if(maybeSave()) {
        settings = new QSettings;
        fileName = settings->value("LastFile", "").toString();
        load();
    }
}

void MainWindow::setLayerOpacity()
{
    opacityDialog->setOpacity(opacityList.at(currentLayer - 1));
    if(opacityDialog->exec() == QDialog::Accepted)
        opacityList.replace(currentLayer - 1 ,opacityDialog->getOpacity());
    drawFrame();
}

void MainWindow::changeGreenScreenStrength()
{
    int strength = greenScreenSpinBox->value();
    greenScreenStrengths.replace(currentLayer - 1, strength);
    updateLayers();
    drawFrame();
}

QImage *MainWindow::IplImageToQImage(const IplImage * iplImage, uchar **data,
                         double mini, double maxi)
{
    uchar *qImageBuffer = NULL;
    int width = iplImage->width;

    // Note here that OpenCV image is stored so that each lined is
    // 32-bits aligned thus * explaining the necessity to "skip"
    // the few last bytes of each line of OpenCV image buffer.
    int widthStep = iplImage->widthStep;
    int height = iplImage->height;

    switch (iplImage->depth)
    {
    case IPL_DEPTH_8U:
        if (iplImage->nChannels == 1)
        {
            // IplImage is stored with one byte grey pixel.
            // We convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uchar *iplImagePtr = (const uchar *)iplImage->imageData;
            for (int y = 0; y < height; y++)
            {
                // Copy line by line
                memcpy(QImagePtr, iplImagePtr, width);
                QImagePtr += width;
                iplImagePtr += widthStep;
            }
        }
        else if (iplImage->nChannels == 3)
        {
            // IplImage is stored with 3 byte color pixels (3 channels).
            // We convert it to a 32 bit depth QImage.
            qImageBuffer = (uchar *) malloc(width*height*4*sizeof(uchar));
             uchar *QImagePtr = qImageBuffer;
             const uchar *iplImagePtr = (const uchar *) iplImage->imageData;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // We cannot help but copy manually.
                    QImagePtr[0] = iplImagePtr[0];
                    QImagePtr[1] = iplImagePtr[1];
                    QImagePtr[2] = iplImagePtr[2];
                    QImagePtr[3] = 0;

                    QImagePtr += 4;
                    iplImagePtr += 3;
                }
                iplImagePtr += widthStep-3*width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported:\
                    depth=8U and %d channels\n", iplImage->nChannels);
        }
        break;

    case IPL_DEPTH_16U:
        if (iplImage->nChannels == 1)
        {
            // IplImage is stored with 2 bytes grey pixel.
            // We convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uint16_t *iplImagePtr = (const uint16_t *)iplImage->imageData;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // We take only the highest part of the 16 bit value.
                    // It is similar to dividing by 256.
                    *QImagePtr++ = ((*iplImagePtr++) >> 8);
                }
                iplImagePtr += widthStep/sizeof(uint16_t)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported:\
                    depth=16U and %d channels\n", iplImage->nChannels);
        }
        break;

    case IPL_DEPTH_32F:
        if (iplImage->nChannels == 1)
        {
            // IplImage is stored with float (4 bytes) grey pixel.
            // We convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const float *iplImagePtr = (const float *) iplImage->imageData;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    uchar p;
                    float pf = 255 * ((*iplImagePtr++) - mini) / (maxi - mini);

                    if (pf < 0) p = 0;
                    else if (pf > 255) p = 255;
                   else p = (uchar) pf;

                    *QImagePtr++ = p;
                }
                iplImagePtr += widthStep/sizeof(float)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported:\
                    depth=32F and %d channels\n", iplImage->nChannels);
        }
        break;

    case IPL_DEPTH_64F:
        if (iplImage->nChannels == 1)
        {
            // OpenCV image is stored with double (8 bytes) grey pixel.
            // We convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const double *iplImagePtr = (const double *) iplImage->imageData;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    uchar p;
                    double pf = 255 * ((*iplImagePtr++) - mini) / (maxi - mini);

                    if (pf < 0) p = 0;
                    else if (pf > 255) p = 255;
                    else p = (uchar) pf;

                    *QImagePtr++ = p;
                }
                iplImagePtr += widthStep/sizeof(double)-width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported:\
                    depth=64F and %d channels\n", iplImage->nChannels);
        }
        break;

    default:
        qDebug("IplImageToQImage: image format is not supported: depth=%d\
                and %d channels\n", iplImage->depth, iplImage->nChannels);
    }

    QImage *qImage;
    if (iplImage->nChannels == 1)
    {
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++)
        {
            colorTable.push_back(qRgb(i, i, i));
        }
        qImage = new QImage(qImageBuffer, width, height, QImage::               Format_Indexed8);
        qImage->setColorTable(colorTable);
    }
    else
    {
        qImage = new QImage(qImageBuffer, width, height, QImage::Format_RGB32);
    }
    *data = qImageBuffer;

    return qImage;
}

void MainWindow::importVideo()
{
   QString videoFileName = QFileDialog::getOpenFileName(
                    this,
                    "Select a video file to open",
                    QDir::homePath(),
                    "Videos (*.ogv *.avi *.mpg *.mp4 *.mov *.webm)");
    if(!videoFileName.isEmpty()) {
        CvCapture *capture = cvCaptureFromFile(videoFileName.toUtf8());
        if(!capture)
        {
                   QMessageBox::warning(this, tr("Oinut"),
                        tr("Video couldn't be loaded."),
                        QMessageBox::Ok);
                    return;
        }
        IplImage *img = 0;
        uchar* data = NULL;
        while(true) {
            img = cvQueryFrame(capture);
            if(!img)
                break;  
            QImage *qt_image = (QImage *)IplImageToQImage(img, &data, 0.0, 0.0);
            QImage aImage = qt_image->convertToFormat(QImage::Format_ARGB32_Premultiplied, Qt::AutoColor);
            QPixmap pixmap = QPixmap::fromImage(aImage);
            addImage(pixmap);
        }
        cvReleaseCapture(&capture);
        if(tmpname.isEmpty())
            makeTmpName();
        makePath();
        QDir tempDir(tmpDir);
        tempDir.mkpath(path);
        QProcess *ffmpegTest = new QProcess(this);
        ffmpegTest->start("ffmpeg");
        if(ffmpegTest->waitForStarted(-1))
        {
            if(ffmpegTest->waitForFinished(-1))
            {
                ffmpegTest->start("ffmpeg", QStringList() << "-i" << videoFileName << "-acodec" << "pcm_s16le" << "-ac" << "2" << path + "/importedsoundfile.wav");
                if(ffmpegTest->waitForFinished(-1)) {
                    QString soundFileName = path + "/importedsoundfile.wav";
                    soundFileNames.append(soundFileName);
                    QMediaPlayer *sound = new QMediaPlayer(this);
                    sound->setMedia(QUrl::fromLocalFile(soundFileName));
                    sound->setVolume(100);
                    soundFiles.append(sound);
                }
            }
        }
    }
}

void MainWindow::toggleDragMode()
{
        if(graphicsView->dragMode() == QGraphicsView::ScrollHandDrag) {
                graphicsView->setDragMode(QGraphicsView::NoDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragMode.svg"));
        } else {
                graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragModeSelected.svg"));
        }
}

void MainWindow::zoomIn()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor += 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void MainWindow::zoomOut()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor -= 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void MainWindow::moveLayerUp()
{
    if(layerCount > 1) {
        if(currentLayer < layerCount) {
            QList<QPixmap> downList = allImages.at(currentLayer - 1);
            QList<QPixmap> tmpList;
            QList<QPixmap> upList = allImages.at(currentLayer);
            tmpList = upList;
            upList = downList;
            downList = tmpList;
            allImages.replace(currentLayer - 1, downList);
            allImages.replace(currentLayer, upList);
            bool downBool = isEmpty.at(currentLayer - 1);
            bool tmpBool;
            bool upBool = isEmpty.at(currentLayer);
            tmpBool = upBool;
            upBool = downBool;
            downBool = tmpBool;
            isEmpty.replace(currentLayer - 1, downBool);
            isEmpty.replace(currentLayer, upBool);
            downBool = isHidden.at(currentLayer - 1);
            upBool = isHidden.at(currentLayer);
            tmpBool = upBool;
            upBool = downBool;
            downBool = tmpBool;
            isHidden.replace(currentLayer - 1, downBool);
            isHidden.replace(currentLayer, upBool);
            downBool = isMultiply.at(currentLayer - 1);
            upBool = isMultiply.at(currentLayer);
            tmpBool = upBool;
            upBool = downBool;
            downBool = tmpBool;
            isMultiply.replace(currentLayer - 1, downBool);
            isMultiply.replace(currentLayer, upBool);
            downBool = isGreenScreen.at(currentLayer - 1);
            upBool = isGreenScreen.at(currentLayer);
            tmpBool = upBool;
            upBool = downBool;
            downBool = tmpBool;
            isGreenScreen.replace(currentLayer - 1, downBool);
            isGreenScreen.replace(currentLayer, upBool);
            int downInt = greenScreenStrengths.at(currentLayer - 1);
            int tmpInt;
            int upInt = greenScreenStrengths.at(currentLayer);
            tmpInt = upInt;
            upInt = downInt;
            downInt = tmpInt;
            greenScreenStrengths.replace(currentLayer - 1, downInt);
            greenScreenStrengths.replace(currentLayer, upInt);
            downInt = opacityList.at(currentLayer - 1);
            upInt = opacityList.at(currentLayer);
            tmpInt = upInt;
            upInt = downInt;
            downInt = tmpInt;
            opacityList.replace(currentLayer - 1, downInt);
            opacityList.replace(currentLayer, upInt);
            QString downStr = layerNames.at(currentLayer - 1);
            QString tmpStr;
            QString upStr = layerNames.at(currentLayer);
            tmpStr = upStr;
            upStr = downStr;
            downStr = tmpStr;
            layerNames.replace(currentLayer - 1, downStr);
            layerNames.replace(currentLayer, upStr);
            currentLayer++;
            updateLayers();
            drawFrame();
        }
    }
}

void MainWindow::moveLayerDown()
{
    if(layerCount > 1) {
        if(currentLayer > 1) {
            QList<QPixmap> downList = allImages.at(currentLayer - 2);
            QList<QPixmap> tmpList;
            QList<QPixmap> upList = allImages.at(currentLayer - 1);
            tmpList = upList;
            upList = downList;
            downList = tmpList;
            allImages.replace(currentLayer - 2, downList);
            allImages.replace(currentLayer - 1, upList);
            bool downBool = isEmpty.at(currentLayer - 2);
            bool tmpBool;
            bool upBool = isEmpty.at(currentLayer - 1);
            tmpBool = upBool;
            upBool = downBool;
            downBool = tmpBool;
            isEmpty.replace(currentLayer - 2, downBool);
            isEmpty.replace(currentLayer - 1, upBool);
            downBool = isHidden.at(currentLayer - 2);
            upBool = isHidden.at(currentLayer - 1);
            tmpBool = upBool;
            upBool = downBool;
            downBool = tmpBool;
            isHidden.replace(currentLayer - 2, downBool);
            isHidden.replace(currentLayer - 1, upBool);
            downBool = isMultiply.at(currentLayer - 2);
            upBool = isMultiply.at(currentLayer - 1);
            tmpBool = upBool;
            upBool = downBool;
            downBool = tmpBool;
            isMultiply.replace(currentLayer - 2, downBool);
            isMultiply.replace(currentLayer - 1, upBool);
            downBool = isGreenScreen.at(currentLayer - 2);
            upBool = isGreenScreen.at(currentLayer - 1);
            tmpBool = upBool;
            upBool = downBool;
            downBool = tmpBool;
            isGreenScreen.replace(currentLayer - 2, downBool);
            isGreenScreen.replace(currentLayer - 1, upBool);
            int downInt = greenScreenStrengths.at(currentLayer - 2);
            int tmpInt;
            int upInt = greenScreenStrengths.at(currentLayer - 1);
            tmpInt = upInt;
            upInt = downInt;
            downInt = tmpInt;
            greenScreenStrengths.replace(currentLayer - 2, downInt);
            greenScreenStrengths.replace(currentLayer - 1, upInt);
            downInt = opacityList.at(currentLayer - 2);
            upInt = opacityList.at(currentLayer - 1);
            tmpInt = upInt;
            upInt = downInt;
            downInt = tmpInt;
            opacityList.replace(currentLayer - 2, downInt);
            opacityList.replace(currentLayer - 1, upInt);
            QString downStr = layerNames.at(currentLayer - 2);
            QString tmpStr;
            QString upStr = layerNames.at(currentLayer - 1);
            tmpStr = upStr;
            upStr = downStr;
            downStr = tmpStr;
            layerNames.replace(currentLayer - 2, downStr);
            layerNames.replace(currentLayer - 1, upStr);
            currentLayer--;
            updateLayers();
            drawFrame();
        }
    }
}

QStyleOptionViewItem TableWidget::viewOptions() const
{
    QStyleOptionViewItem option = QTableWidget::viewOptions();
    option.decorationAlignment = Qt::AlignHCenter | Qt::AlignCenter;
    option.decorationPosition = QStyleOptionViewItem::Top;
    return option;
}

void MainWindow::selectionChangedSlot()
{
    currentLayer = layerCount - timelineWidget->currentRow();
    frame = timelineWidget->currentColumn() + 1;
    updateLayers();
    drawFrame();
}

void MainWindow::resizeAnim()
{
    sizeDialog->setWidth(w);
    sizeDialog->setHeight(h);
    if (sizeDialog->exec() == QDialog::Accepted)
    {
        w = sizeDialog->getWidth();
        h = sizeDialog->getHeight();
        for(int i = 0; i < allImages.count(); i++) {
            QList<QPixmap> tmpList = allImages.at(i);
            for(int l = 0; l < tmpList.count(); l++) {
                QPixmap scaledPixmap = tmpList.at(l).scaled(w, h);
                tmpList.replace(l, scaledPixmap);
            }
            allImages.replace(i, tmpList);
        }
        QPixmap empty(w, h);
        empty.fill(Qt::white);
        item->setPixmap(empty);
        item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
       
        thumbnailScene->setSceneRect(0, 0, int(w / 5), int(h / 5));
        frameThumbnail->setSceneRect(0, 0, int(w / 5), int(h / 5));
        frameThumbnail->setFixedSize(int(w / 5), int(h / 5));
    }
    drawFrame();
}


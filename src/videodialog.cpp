/****************************************************************************
    Copyright (C) 2012-2015 Omer Bahri Gordebak <purplehuman at zoho.com>
    IplImageToQImage() function is
    Copyright © 2003 Rémi Ronfard, David Knossow and Matthieu Guilbert

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

#include "videodialog.h"
#include "ui_videodialog.h"
#include "mainwindow.h"
#include <QtWidgets>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "QOpenCVWidget.h"
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

using namespace cv;

VideoDialog::VideoDialog(QWidget *) :
    QMainWindow(),
    ui(new Ui::VideoDialog)
{
    ui->setupUi(this);
    setMinimumSize(820, 620);

    scene = new QGraphicsScene(this);
    isOnion = true;
    isGrid = false;
    ui->actionOnion3->setIcon(QIcon(":images/onion3Selected.svg"));
    ui->actionGrid->setIcon(QIcon(":images/grid.svg"));
    framePixmaps.clear();
    zoomFactor = 1.0;

    connect(ui->actionAddFrame, SIGNAL(triggered()), this, SLOT(addFrame()));
    connect(ui->actionInsertFrame, SIGNAL(triggered()), this, SLOT(insertFrame()));
    connect(ui->actionReplaceFrame, SIGNAL(triggered()), this, SLOT(replaceFrame()));
    connect(ui->actionNextCamera, SIGNAL(triggered()), this, SLOT(nextCamera()));
    connect(ui->actionPreviousCamera, SIGNAL(triggered()), this, SLOT(previousCamera()));
    connect(ui->actionOnion3, SIGNAL(triggered()), this, SLOT(toggleOnion()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->actionGrid, SIGNAL(triggered()), this, SLOT(toggleGrid()));
    connect(ui->actionDragMode, SIGNAL(triggered()), this,                      SLOT(toggleDragMode()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
}

void VideoDialog::quit()
{
    win->enableActions();
    cvReleaseCapture(&camera);
    killTimer(timerInt);
    close();
}

void VideoDialog::addFrame()
{
    uchar* data = NULL;
    qt_image = IplImageToQImage(image, &data, 0.0, 0.0);
    QImage aImage = qt_image->convertToFormat(QImage::Format_ARGB32_Premultiplied, Qt::AutoColor);
    QPixmap pixmap = QPixmap::fromImage(aImage);
    framePixmaps.append(pixmap);
    if(framePixmaps.count() > 3) 
        framePixmaps.removeFirst();
    drawFrame();
    win->addImage(pixmap);
}

void VideoDialog::insertFrame()
{
    uchar* data = NULL;
    qt_image = IplImageToQImage(image, &data, 0.0, 0.0);
    QImage aImage = qt_image->convertToFormat(QImage::Format_ARGB32_Premultiplied, Qt::AutoColor);
    QPixmap pixmap = QPixmap::fromImage(aImage);
    framePixmaps.append(pixmap);
    if(framePixmaps.count() > 3)
        framePixmaps.removeFirst();
    drawFrame();
    win->insertImage(pixmap);
}

void VideoDialog::replaceFrame()
{
    uchar* data = NULL;
    qt_image = IplImageToQImage(image, &data, 0.0, 0.0);
    QImage aImage = qt_image->convertToFormat(QImage::Format_ARGB32_Premultiplied, Qt::AutoColor);
    QPixmap pixmap = QPixmap::fromImage(aImage);
    framePixmaps.append(pixmap);
    if(framePixmaps.count() > 3)
        framePixmaps.removeFirst();
    drawFrame();
    win->replaceImage(pixmap);
}

QImage *VideoDialog::IplImageToQImage(const IplImage * iplImage, uchar **data,
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
        qImage = new QImage(qImageBuffer, width, height, QImage::Format_Indexed8);
        qImage->setColorTable(colorTable);
    }
    else
    {
        qImage = new QImage(qImageBuffer, width, height, QImage::Format_RGB32);
    }
    *data = qImageBuffer;
 
    return qImage;
}

void VideoDialog::newAnim()
{
        currentCamera = 0;
        cvWidget = new QOpenCVWidget;
        camera = cvCaptureFromCAM(0);
        if(!camera) {
            QMessageBox::warning(this, tr("Video"),
                       tr("Couldn't capture from the camera."),
                       QMessageBox::Ok);
            return;
        }
        cvSetCaptureProperty(camera,CV_CAP_PROP_FRAME_HEIGHT,h); 
        cvSetCaptureProperty(camera,CV_CAP_PROP_FRAME_WIDTH ,w); 

        image = cvQueryFrame(camera);
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
   
        graphicsView->show();
        QGraphicsProxyWidget *item = scene->addWidget(cvWidget);
        item->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
    
        int width = w + 20;
        int height = h + 100; 
        setMinimumSize(width, height);

        timerInt = startTimer(100);
}

void VideoDialog::nextCamera()
{
    CvCapture *tmpCamera;
    currentCamera++;
    tmpCamera = cvCaptureFromCAM(currentCamera);
    if(!tmpCamera) 
        currentCamera--;
    cvReleaseCapture(&tmpCamera);
    killTimer(timerInt);
    cvReleaseCapture(&camera);
    camera = cvCaptureFromCAM(currentCamera);
    cvSetCaptureProperty(camera,CV_CAP_PROP_FRAME_HEIGHT,h); 
    cvSetCaptureProperty(camera,CV_CAP_PROP_FRAME_WIDTH ,w); 
    timerInt = startTimer(100);
}            

void VideoDialog::previousCamera()
{
    CvCapture *tmpCamera;
    currentCamera--;
    tmpCamera = cvCaptureFromCAM(currentCamera);
    if(!tmpCamera) 
        currentCamera++;
    cvReleaseCapture(&tmpCamera);
    killTimer(timerInt);
    cvReleaseCapture(&camera);
    camera = cvCaptureFromCAM(currentCamera);
    cvSetCaptureProperty(camera,CV_CAP_PROP_FRAME_HEIGHT,h); 
    cvSetCaptureProperty(camera,CV_CAP_PROP_FRAME_WIDTH ,w); 
    timerInt = startTimer(100);
}

void VideoDialog::timerEvent(QTimerEvent*)
{
    if(camera) {
        image = cvQueryFrame(camera);
        cvWidget->putImage(image);
    } else {
        killTimer(timerInt);
    }
}

void VideoDialog::closeEvent(QCloseEvent *event)
{
    win->enableActions();
    cvReleaseCapture(&camera);
    delete ui;
    event->accept();
}

void VideoDialog::toggleOnion()
{
    if(isOnion) {
        isOnion = false;
        ui->actionOnion3->setIcon(QIcon(":images/onion3.svg"));
    } else {
        isOnion = true;
        ui->actionOnion3->setIcon(QIcon(":images/onion3Selected.svg"));
    }
    drawFrame();
}

void VideoDialog::drawFrame()
{
    for(int i = 0; i < scene->items().count(); i++) {
        if(scene->items().at(i)->type() != QGraphicsProxyWidget::Type)
            scene->removeItem(scene->items().at(i));
    }
    int c = framePixmaps.count();
    if(isOnion) {
        if(c > 2) {
            for(int i = 3; i > 0; i--) {
                QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem;
                pixmapItem->setPixmap(framePixmaps.at(c - i));
                pixmapItem->setOpacity(0.3 / i);
                scene->addItem(pixmapItem);
                pixmapItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
            }
        } else if(c > 1) {
            for(int i = 2; i > 0; i--) {
                QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem;
                pixmapItem->setPixmap(framePixmaps.at(c - i));
                pixmapItem->setOpacity(0.3 / i);
                scene->addItem(pixmapItem);
                pixmapItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
            }
        } else if(c > 0) {
            QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem;
            pixmapItem->setPixmap(framePixmaps.at(c - 1));
            pixmapItem->setOpacity(0.3);
            scene->addItem(pixmapItem);
            pixmapItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
        }
    }
    if(isGrid) {
        QPixmap bgPixmap(w, h);
        bgPixmap.fill(Qt::transparent);
        QPainter painter(&bgPixmap);
        painter.setPen(QPen(Qt::blue, 2));
        painter.setOpacity(0.1);
        for(int i = 0; i < w; i = i + 20)
            painter.drawLine(QPointF(i, 0), QPointF(i, h));
        for(int i = 0; i < h; i = i + 20)
            painter.drawLine(QPointF(0, i), QPointF(w, i));
        painter.end();
        QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem;
        pixmapItem->setPixmap(bgPixmap);
        scene->addItem(pixmapItem);
        pixmapItem->setPos(scene->sceneRect().center().x() - w / 2, scene->sceneRect().center().y() - h / 2);
    }
}

void VideoDialog::toggleGrid()
{
    if(isGrid) {
        isGrid = false;
        ui->actionGrid->setIcon(QIcon(":images/grid.svg"));
    } else {
        isGrid = true;
        ui->actionGrid->setIcon(QIcon(":images/gridSelected.svg"));
    }
    drawFrame();
}

void VideoDialog::toggleDragMode()
{
        if(graphicsView->dragMode() == QGraphicsView::ScrollHandDrag) {
                graphicsView->setDragMode(QGraphicsView::NoDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragMode.svg"));
        } else {
                graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
                ui->actionDragMode->setIcon(QIcon(":images/dragModeSelected.svg"));
        }
}

void VideoDialog::zoomIn()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor += 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

void VideoDialog::zoomOut()
{
        graphicsView->scale(1 /zoomFactor, 1 /zoomFactor);
        zoomFactor -= 0.05;
        graphicsView->scale(zoomFactor, zoomFactor);
}

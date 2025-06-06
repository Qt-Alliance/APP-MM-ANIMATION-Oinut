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

#ifndef VIDEODIALOG_H
#define VIDEODIALOG_H

#include <QDialog>
#include <QtWidgets>
#include "QOpenCVWidget.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstring>
#include <iostream>
#include <QImage>

class QOpenCVWidget;
class MainWindow;

using namespace cv;

namespace Ui {
class VideoDialog;
}

class VideoDialog : public QMainWindow
{
    Q_OBJECT
    
public:
    VideoDialog(QWidget *parent = 0);
    void setWidth(int width)
    { w = width; }
    void setHeight(int height)
    { h = height; }
    void newAnim();
    void setMainWindow(MainWindow *sent)
    { win = sent; }

private:
    Ui::VideoDialog *ui;
    void closeEvent(QCloseEvent *event);

private:
    QGraphicsScene *scene;
    QGraphicsView *graphicsView;
    QOpenCVWidget *cvWidget;
    CvCapture *camera;
    IplImage *image;
    QImage *qt_image;
    QImage *IplImageToQImage(
            const IplImage* iplImage,
            uchar** data,
            double mini=0.0,
            double maxi=0.0);
    void drawFrame();
    int currentCamera;
    int w;
    int h;
    bool isViewAvailable;
    QSettings *settings;
    int timerInt;
    bool isOnion;
    QList<QPixmap> framePixmaps;
    MainWindow *win;
    bool isGrid;
    qreal zoomFactor;
    QGraphicsPixmapItem *bg;

private slots:
    void quit();
    void addFrame();
    void insertFrame();
    void replaceFrame();
    void nextCamera();
    void previousCamera();
    void toggleOnion();
    void toggleGrid();
    void zoomIn();
    void zoomOut();
    void toggleDragMode();

protected:
    void timerEvent(QTimerEvent*);
};

#endif // VIDEODIALOG_H

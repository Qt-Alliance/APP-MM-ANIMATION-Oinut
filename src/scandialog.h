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

#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <QtWidgets>

class MainWindow;

namespace Ui {
class ScanDialog;
}

class ScanDialog : public QMainWindow
{
    Q_OBJECT
    
public:
    ScanDialog(QWidget *parent = 0);
    void setMainWindow(MainWindow *sent)
    { win = sent; }
    void setWidth(int width)
    { w = width; }
    void setHeight(int height)
    { h = height; }
    void newImage();

private:
    Ui::ScanDialog *ui;
    void closeEvent(QCloseEvent *event);

private:
    QGraphicsScene *scene;
    QGraphicsView *graphicsView;
    QAction *actionScan;
    QAction *actionAddImage;
    QAction *actionInsertImage;
    QAction *actionReplaceImage;
    QAction *actionQuit;
    QAction *actionRotateClockwise;
    QAction *actionRotateCounterClockwise;
    QAction *actionScaleUp;
    QAction *actionScaleDown;
    QAction *actionRestoreMove;
    QAction *actionRestoreScale;
    QAction *actionRestoreRotate;
    QAction *actionFlipHorizontal;
    QAction *actionFlipVertical;
    QAction *actionToggleColor;
    QAction *actionSave;
    QComboBox *resolutionComboBox;
    int resolution;
    bool isColor;
    QPixmap pixmap;
    QGraphicsPixmapItem *item;
    QPixmap backgroundPixmap;
    QGraphicsPixmapItem *background;
    MainWindow *win;
    int w;
    int h;
    QImage *image;
    bool isFlippedH;
    bool isFlippedV;
    qreal zoomFactor;

private slots:
    void scan();
    void addImage();
    void insertImage();
    void replaceImage();
    void quit();
    void rotateClockwise();
    void rotateCounterClockwise();
    void scaleUp();
    void scaleDown();
    void restoreMove();
    void restoreScale();
    void restoreRotate();
    void flipHorizontal();
    void flipVertical();
    void toggleDragMode();
    void zoomIn();
    void zoomOut();
    void toggleColor();
    void save();
    void changeResolution();
};

#endif // SCANDIALOG_H

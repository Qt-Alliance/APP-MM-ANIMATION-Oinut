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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsView>
#include <QLabel>
#include <QSpinBox>
#include <QSettings>
#include <QMediaPlayer>
#include <QTableWidget>
#include <QStandardItemModel>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;

class SizeDialog;
class VideoDialog;
class LayerNameDialog;
class CutoutDialog;
class DrawingDialog;
class OpacityDialog;
class ScanDialog;

namespace Ui {
class MainWindow;
}

class TableWidget : public QTableWidget
{
private:
    QStyleOptionViewItem viewOptions() const;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void addImage(QPixmap pixmap);
    void replaceImage(QPixmap pixmap);
    void insertImage(QPixmap pixmap);
    void removeLast();
    void enableActions();
   
private:
    Ui::MainWindow *ui;

    int frame;
    int frameCount;
    int layerCount;
    int currentLayer;
    bool isLooping;
    bool isFirstRun;
    bool isPlaying;
    int w;
    int h;
    QList<QList<QPixmap> > allImages;
    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
    QGraphicsScene *thumbnailScene;
    QGraphicsView *frameThumbnail;
    TableWidget *timelineWidget;
    QTimer *timer;
    QStringList fileNames;
    QList<bool> isEmpty;
    QList<bool> isMultiply;
    QList<bool> isGreenScreen;
    QList<bool> isHidden;
    QList<int> opacityList;
    QList<int> greenScreenStrengths;
    QString layerNumberStr;
    QString frameNumberStr;
    SizeDialog *sizeDialog;
    VideoDialog *videoDialog;
    CutoutDialog *cutoutDialog;
    DrawingDialog *drawingDialog;
    OpacityDialog *opacityDialog;
    ScanDialog *scanDialog;
    QLabel *frameLabel;
    QLabel *frameNumberLabel;
    QLabel *layerNumberLabel;
    QLabel *fpsLabel;
    QLabel *emptyLabel;
    QLabel *emptyLabel2;
    QSpinBox *fps;
    QSpinBox *greenScreenSpinBox;
    QSettings *settings;
    QLabel *frameCountLabel;
    QStringList layerNames;
    LayerNameDialog *layerNameDialog;
    bool isReplace;
    QList<QMediaPlayer *> soundFiles;
    QStringList soundFileNames;
    QString fileName;
    QString username;
    QString tmpname;
    QString path;
    QString tmpDir;
    bool isSaved;
    QLabel *layerLabel;
    QAction *actionLayerName;

private:
    void drawFrame();
    void updateLayers();
    void calculateFrameCount();
    void addImages(QStringList fileNames);
    QPixmap setupFrame();
    void insertImages(QStringList fileNames);
    QPixmap setupCurrent();
    void closeEvent(QCloseEvent *event);
    void load();
    void makeTmpName();
    void makePath();
    bool maybeSave();
    void getFileName();
    QImage *IplImageToQImage(
            const IplImage* iplImage,
            uchar** data,
            double mini=0.0,
            double maxi=0.0);
    QGraphicsPixmapItem *item;
    qreal zoomFactor;
    bool isMergeDown;
    QWidget *spacer3;
    QLabel *greenScreenLabel;
    bool isRender;

private slots:
    void play();
    void toggleLoop();
    void newAnim();
    void showNextFrame();
    void showPreviousFrame();
    void showFirstFrame();
    void showLastFrame();
    void addFiles();
    void playNextFrame();
    void render();
    void addLayer();
    void addLayerBelow();
    void removeLayer();
    void layerUp();
    void layerDown();
    void layerTop();
    void layerBottom();
    void addEmptyFrame();
    void makeWhiteTransparent();
    void hideLayer();
    void showVideo();
    void showCutout();
    void showDrawing();
    void insertEmptyFrame();
    void insertFiles();
    void replace();
    void duplicate();
    void exportFrame();
    void addSound();
    void removeSound();
    void removeFrame();
    void quit();
    void exportLayer();
    void mergeDown();
    void open();
    void saveAs();
    void save();
    void initialize();
    void loadLast();
    void setLayerOpacity();
    void makeGreenScreen();
    void changeGreenScreenStrength();
    void importVideo();
    void renderVideo();
    void showScan();
    void toggleDragMode();
    void zoomIn();
    void zoomOut();
    void duplicateAndAppend();
    void moveLayerUp();
    void moveLayerDown();
    void changeLayerName();
    void selectionChangedSlot();
    void exportFrameInCurrentLayer();
    void resizeAnim();
};

#endif // MAINWINDOW_H

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

#ifndef DRAWINGDIALOG_H
#define DRAWINGDIALOG_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QSpinBox>
#include <QFontComboBox>
#include <QTextEdit>
#include <QSettings>
#include <QWindowStateChangeEvent>
#include <QGraphicsProxyWidget>

class LayerNameDialog;
class BrushSettingsDialog;
class TabletCanvas;
class MainWindow;
class OpacityDialog;
class BlurDialog;

namespace Ui {
class DrawingDialog;
}

class Pasted : public QGraphicsPixmapItem
{
public:
    Pasted()
    {
        scaleX = 1.0;
        scaleY = 1.0;
        rot = 0.0;
    }
    qreal getScaleX()
    { return scaleX; }
    qreal getScaleY()
    { return scaleY; }
    qreal getRot()
    { return rot; }
    void setScaleX(qreal sentScale)
    { scaleX = sentScale; }
    void setScaleY(qreal sentScale)
    { scaleY = sentScale; }
    void setRot(qreal sentRot)
    { rot = sentRot; }

private:
    qreal scaleX;
    qreal scaleY;
    qreal rot;
};

class Pivot : public QGraphicsEllipseItem
{
protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
   QPointF initialPos;
};

class BottomRightHandle : public QGraphicsRectItem
{
public:
   enum { Type = UserType + 1 };

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
   QPointF initialPos;
};

class BottomLeftHandle : public QGraphicsRectItem
{
public:
   enum { Type = UserType + 2 };

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
   QPointF initialPos;
};

class TopRightHandle : public QGraphicsRectItem
{
public:
   enum { Type = UserType + 3 };

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
   QPointF initialPos;
};

class TopLeftHandle : public QGraphicsRectItem
{
public:
   enum { Type = UserType + 4 };

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
   QPointF initialPos;
};

class DrawingGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    DrawingGraphicsView(QWidget *)
    {
        deviceDown = false;
        isSelect = false;
        isSelected = false;
    }

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    bool isSelect;
    QGraphicsRectItem *selection;
    QPointF initialPos;
    bool deviceDown;
    bool isHidden;
    bool isLocked;
    QPixmap pixmap;
    QPixmap selected;
    bool isSelected;

public:
    void setSelectMode(bool ifMode)
    { isSelect = ifMode; }
    bool getSelectMode()
    { return isSelect; }
    bool getIfSelected()
    { return isSelected; }
};

class DrawingDialog : public QMainWindow
{
    Q_OBJECT
    
public:
    DrawingDialog(QWidget *parent = 0);
    void newImage();
    void setWidth(int width)
    { w = width; }
    void setHeight(int height)
    { h = height; }
    int getWidth()
    { return w; }
    int getHeight()
    { return h; }
    QPointF getPos()
    { return bgItem->pos(); }
    void setPixmap(QPixmap pixmap);
    void setMainWindow(MainWindow *sent)
    { win = sent; }
    void setImageOpacity(int opacity)
    { opacityList.replace(0, opacity); }
    void setBackFrame(QPixmap sentPixmap)
    {
      backFrame = sentPixmap;
      isBackFrame = true; 
      drawImage();
    }
    void setForwardFrame(QPixmap sentPixmap)
    { 
      forwardFrame = sentPixmap;
      isForwardFrame = true;
      drawImage();
    }
    void setFirstFrame(QPixmap sentPixmap)
    {
      firstFrame = sentPixmap;
      isFirstFrame = true; 
      drawImage();
    }
    void setLastFrame(QPixmap sentPixmap)
    {
      lastFrame = sentPixmap;
      isLastFrame = true; 
      drawImage();
    }
    QPixmap getCurrentPixmap()
    { return allImages.at(currentLayer - 1)->pixmap(); }
    bool getIfHidden()
    { return isHidden.at(currentLayer - 1); }
    bool getIfLocked()
    { return isLocked.at(currentLayer - 1); }
    void setIfSelected(bool ifSelected)
    { isSelected = ifSelected; }
    void setSelectionPos(QPointF point)
    { selectionPos = point; }
    void drawImage();
    void clearScene();
    void mergeDrawing();
    void setSelection(QGraphicsRectItem *sentSelection)
    { selection = sentSelection; }
    
private:
    Ui::DrawingDialog *ui;

    int layerCount;
    int currentLayer;
    bool isFirstRun;
    bool isBgTransparent;
    bool isBrush;
    bool isEraser;
    bool isPencil;
    bool isRendering;
    bool isSizeWithPressure;
    bool isAlphaWithPressure;
    bool isSaturationWithPressure;
    int w;
    int h;
    QList<QGraphicsPixmapItem *> allImages;
    QList<bool> isLocked;
    QList<bool> isHidden;
    QList<int> opacityList;
    DrawingGraphicsView *graphicsView;
    QGraphicsScene *scene;
    QString layerNumberStr;
    LayerNameDialog *layerNameDialog;
    BrushSettingsDialog *brushSettings;
    OpacityDialog *opacityDialog;
    QAction *actionLayerName;
    QLabel *layerNumberLabel;
    QLabel *brushSizeLabel;
    QLabel *layerLabel;
    QLabel *layerAttributesLabel;
    QSpinBox *brushSizeBox;
    QLabel *emptyLabel;
    QLabel *emptyLabel2;
    QSettings *settings;
    TabletCanvas *tabletCanvas;
    int minWidth;
    int minHeight;
    QColor brushPaint;
    QStringList layerNames;
    bool isGrid;
    MainWindow *win;
    QPixmap texture;
    QGraphicsPixmapItem *bgItem;
    QGraphicsProxyWidget *tabletItem;
    qreal zoomFactor;
    QPixmap backFrame;
    QPixmap forwardFrame;
    QPixmap firstFrame;
    QPixmap lastFrame;
    bool isBackFrame;
    bool isForwardFrame;
    bool isFirstFrame;
    bool isLastFrame;
    bool isOnion;
    bool isTopFrame;
    bool isSelect;
    bool isSelected;
    bool isClipboard;
    QGraphicsRectItem *selection;
    QPixmap selected;
    QPointF selectionPos;
    bool isPasted;
    Pasted *pastedItem;
    QGraphicsRectItem *boundaries;
    BlurDialog *blurDialog;
    bool isTextEntry;
    QFontComboBox *fontComboBox;
    QSpinBox *fontSpinBox;
    QTextEdit *textEntryTextEdit;
    QGraphicsProxyWidget *textEntryItem;

private:
    void closeEvent(QCloseEvent *event);
    void firstRun();
    void makeView();
    void makeVars();
    bool setSize();
    void addImage(QString addFileName);
    void updateLayers();
    void prepareBrush();

private slots:
    void addFile();
    void addLayer();
    void addLayerBelow();
    void removeLayer();
    void layerUp();
    void layerDown();
    void layerTop();
    void layerBottom();
    void quit();
    void brushColor();
    void increaseBrushSize();
    void decreaseBrushSize();
    void changeLayerName();
    void toggleBrush();
    void brushSizeChanged();
    void undo();
    void toggleLock();
    void toggleVisibility();
    void brushSettingsDialog();
    void addFrame();
    void insertFrame();
    void replaceFrame();
    void exportImage();
    void exportDrawing();
    void setLayerOpacity();
    void toggleGrid();
    void loadBrush();
    void toggleDragMode();
    void zoomIn();
    void zoomOut();
    void onion();
    void exportLayer();
    void selectMode();
    void cut();
    void paste();
    void copy();
    void exportSelection();
    void anchor();
    void deleteSelected();
    void selectAll();
    void blur();
    void colorize();
    void keyUp();
    void keyDown();
    void keyRight();
    void keyLeft();
    void fitWidth();
    void fitHeight();
    void textEntry();
    void changeFont();
};

#endif // DRAWINGDIALOG_H

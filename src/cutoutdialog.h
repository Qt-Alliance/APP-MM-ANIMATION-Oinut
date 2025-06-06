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

#ifndef CUTOUTDIALOG_H
#define CUTOUTDIALOG_H

#include <QtWidgets>
#include <QSound>
#include <QGraphicsSvgItem>

class PivotItem : public QGraphicsEllipseItem
{
public:
    PivotItem()
    {
        rot = 0;
        setFlags(QGraphicsItem::ItemIsMovable);
    }
    enum { Type = UserType + 1 };

    int type() const
    {
        return Type;
    }

    int parentId() const
    {
        return ParentId;
    }

    int childId() const
    {
        return ChildId;
    }

    bool isLocked() const
    {
        return IsLocked;
    }
    void setMode(QString modeSent)
    {
	mode = modeSent;
    }
    bool hasChanged()
    {
	return !isSaved;
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QString mode;
    int ChildId;
    int ParentId;
    QPointF initialPos;
    qreal rot;
    QSettings *settings;
    QPointF moved;
    bool IsLocked;
    bool IsHorizontalFlipped;
    bool IsVerticalFlipped;
    bool isSaved;
};

class GraphicsSvgItem : public QGraphicsSvgItem
{
public:
    GraphicsSvgItem()
    {
        rot = 0;
        setFlags(flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    }
 
    enum { Type = UserType + 4 };

    int type() const
    {
        return Type;
    }

    QString filename() const
    {
        return fileName;
    }

    int childId() const
    {
        return ChildId;
    }

    int parentId() const
    {
        return ParentId;
    }

    bool isLocked() const
    {
        return IsLocked;
    }

    bool isHorizontalFlipped() const
    {
         return IsHorizontalFlipped;
    }

    bool isVerticalFlipped() const
    {
        return IsVerticalFlipped;
    }
    void setMode(QString modeSent)
    {
	mode = modeSent;
    }
    bool hasChanged()
    {
	return !isSaved;
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    QString mode;
    bool IsLocked;
    QString fileName;
    QPointF initialPos;
    qreal rot;
    QPointF parentInitialPos;
    QSettings *settings;
    int ChildId;
    int ParentId;
    bool IsHorizontalFlipped;
    bool IsVerticalFlipped;
    bool isSaved;
};

class GraphicsPixmapItem : public QGraphicsPixmapItem
{
public:
    GraphicsPixmapItem()
    {
        rot = 0;
        setFlags(flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    }
 
    enum { Type = UserType + 2 };

    int type() const
    {
        return Type;
    }

    QString filename() const
    {
        return fileName;
    }

    int childId() const
    {
        return ChildId;
    }

    int parentId() const
    {
        return ParentId;
    }

    bool isLocked() const
    {
        return IsLocked;
    }

    bool isHorizontalFlipped() const
    {
         return IsHorizontalFlipped;
    }

    bool isVerticalFlipped() const
    {
        return IsVerticalFlipped;
    }
    void setMode(QString modeSent)
    {
	mode = modeSent;
    }
    bool hasChanged()
    {
	return !isSaved;
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    QString mode;
    bool IsLocked;
    QString fileName;
    QPointF initialPos;
    qreal rot;
    QPointF parentInitialPos;
    QSettings *settings;
    int ChildId;
    int ParentId;
    bool IsHorizontalFlipped;
    bool IsVerticalFlipped;
    bool isSaved;
};

class GraphicsBackgroundPixmapItem : public QGraphicsPixmapItem
{
public:
    GraphicsBackgroundPixmapItem()
    {

    }
 
    enum { Type = UserType + 3 };

    int type() const
    {
        return Type;
    }

    QString filename()
    {
        return fileName;
    }

    QString fileName;
};

class GraphicsBackgroundSvgItem : public QGraphicsSvgItem
{
public:
    GraphicsBackgroundSvgItem()
    {

    }
 
    enum { Type = UserType + 5 };

    int type() const
    {
        return Type;
    }

    QString filename()
    {
        return fileName;
    }

    QString fileName;
};

class MainWindow;

namespace Ui {
class CutoutDialog;
}

class CutoutDialog : public QMainWindow
{
    Q_OBJECT
    
public:
    CutoutDialog(QWidget *parent = 0);
    void newAnim();
    void setWidth(int width)
    { w = width; }
    void setHeight(int height)
    { h = height; }
    void setMainWindow(MainWindow *sent)
    { win = sent; }
    
private:
    Ui::CutoutDialog *ui;

    QGraphicsScene *scene;
    QGraphicsView *graphicsView;
    QString mode;
    QSettings *settings;
    QList<QGraphicsItem *> oldSelected;
    QList<QGraphicsItem *> restoreSelected;
    bool isSetParent;
    bool isOnion;
    QList<QPixmap> framePixmaps;
    QList<QPixmap> transparencyPixmaps;
    QList<QGraphicsPixmapItem *> pixmapItems;
    QList<QGraphicsItem *> sceneItems;
    QStringList framePixmapFileNames;
    QStringList pixmapItemFileNames;
    int frame;
    int frameCount;
    int w;
    int h;
    QString fileName;
    bool isSaved;
    int parentId;
    int childId;
    int z;
    GraphicsPixmapItem *currentSelectedPixmap;
    bool isSetParentWithTab;
    QString username;
    bool isFirstRun;
    bool isSceneEmpty;
    bool isGrid;
    QString tmpname;
    QString path;
    QString tmpDir;
    QGraphicsPixmapItem *background;
    qreal zoomFactor;
    QSvgRenderer *renderer;
    int childIdCount;
    MainWindow *win;
    QGraphicsRectItem *frameShow;

private:
    void closeEvent(QCloseEvent *event);

    bool maybeSave();

    void restoreWorkspace();
    void firstRun();
    void getFileName();
    void load();
    void itemAdded(GraphicsPixmapItem *pixmapItem);
    void enableActions();
    void makePath();
    void makeTmpName();

private slots:
    void setModeRotate();
    void setModeRotate15();
    void setModeMove();
    void setModeEdit();
    void setModeScale();
    void selectionChangedSlot();
    void sendDown();
    void sendUp();
    void keyUp();
    void keyDown();
    void keyRight();
    void keyLeft();
    void setParent();
    void unsetParent();
    void addFrame();
    void insertFrame();
    void replaceFrame();
    void addImage();
    void removeImage();
    void save();
    void saveAs();
    void open();
    void loadLast();
    void restoreItemEdit();
    void restoreItemRotate();
    void restoreItemScale();
    void sendBelowParent();
    void sendAboveParent();
    void quitApp();
    void toggleLock();
    void changeImage();
    void escapeClicked();
    void onion3();
    void zoomIn();
    void zoomOut();
    void toggleDragMode();
    void importOinut();
    void deleteFrame();
    void selectNext();
    void selectPrevious();
    void toggleGrid();
    void exportSelected();
};

#endif // CUTOUTDIALOG_H

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2012-2015 Omer Bahri Gordebak <purplehuman at zoho.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TABLETCANVAS_H
#define TABLETCANVAS_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QTabletEvent>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPoint>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QString;
QT_END_NAMESPACE

//! [0]
class TabletCanvas : public QWidget
{
    Q_OBJECT

public:
    enum AlphaChannelType { AlphaPressure, NoAlpha };
    enum ColorSaturationType { SaturationPressure, NoSaturation };
    enum LineWidthType { LineWidthPressure, NoLineWidth };

    TabletCanvas();

    bool saveImage(const QString &file);
    bool loadImage(const QString &file);
    void setEraser();
    void setStylus();
    void setAlphaChannelType(AlphaChannelType type)
        { alphaChannelType = type; }
    void setColorSaturationType(ColorSaturationType type)
        { colorSaturationType = type; }
    void setBrushOpacity(int brushOpacitySent)
	{ brushOpacity = brushOpacitySent; }
    void setLineWidthType(LineWidthType type)
        { lineWidthType = type; }
    void setColor(const QColor &color)
        { 
            myColor = color; 
            isTexture = false;
        }
    void setTexture(const QPixmap &sentTexture)
    {
        myTexture = sentTexture;
        isTexture = true;
    }
    bool getIfTexture()
    { return isTexture; }
    QColor color() const
        { return myColor; }
    void setTabletDevice(QTabletEvent::TabletDevice device)
        { myTabletDevice = device; }
    int maximum(int a, int b)
        { return a > b ? a : b; }
    QPixmap pixmapDrawn()
	{ return pixmap; }
    void setBrushSize(int size)
	{ brushSize = size; }
    void setPixmap(const QPixmap pixmapSent)
	{ pixmap = pixmapSent;
          update(); }
    bool isModified()
	{ return isDrawn; }
    void initPixmap(const QPixmap firstPixmap);
    void setOpacity(int opacitySent)
	{ opacity = opacitySent; }

    QPixmap undo();

public:
    void tabletEvent(QTabletEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void paintPixmap(QPainter &painter, QTabletEvent *event);
    Qt::BrushStyle brushPattern(qreal value);
    void updateBrush(QTabletEvent *event);

    AlphaChannelType alphaChannelType;
    ColorSaturationType colorSaturationType;
    LineWidthType lineWidthType;
    QTabletEvent::PointerType pointerType;
    QTabletEvent::TabletDevice myTabletDevice;
    QColor myColor;

    QList<QPixmap> history;
    bool tabletInputBusy;
    bool isDrawn; 
    bool isEraser;
    bool isTexture;
    int brushSize;
    QPixmap pixmap;
    QPixmap tempPixmap;
    QBrush myBrush;
    QPen myPen;
    bool deviceDown;
    QPoint polyLine[3];
    int undoCount;
    int opacity;
    int brushOpacity;
    QPixmap myTexture;
};
//! [0]

#endif

/****************************************************************************nti
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

#include <QtWidgets>
#include <math.h>

#include "tabletcanvas.h"

TabletCanvas::TabletCanvas()
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
    QSettings *settings = new QSettings;
    int w = settings->value("Width", 1280).toInt();
    int h = settings->value("Height", 720).toInt();
    resize(w, h);
    myBrush = QBrush();
    myPen = QPen();
    deviceDown = false;
    isTexture = false;
    opacity = 255;
    myColor = QColor(0, 0, 0, 255);
    myTabletDevice = QTabletEvent::Stylus;
    alphaChannelType = AlphaPressure;
    colorSaturationType = NoSaturation;
    lineWidthType = LineWidthPressure;
    isEraser = false;
    isDrawn = false;
    tabletInputBusy = false;
    history.clear();
    undoCount = 0;
    brushOpacity = 255;
}

void TabletCanvas::initPixmap(const QPixmap firstPixmap)
{
    history.clear();
    undoCount = 0;
    QPixmap newPixmap = QPixmap(width(), height());
    newPixmap.fill(Qt::transparent);
    QPainter painter(&newPixmap);
    painter.drawPixmap(0, 0, firstPixmap);
    painter.end();
    pixmap = newPixmap;
    history.append(pixmap);
    isDrawn = false;
    update();
}

void TabletCanvas::tabletEvent(QTabletEvent *event)
{
    tabletInputBusy = true;
    switch (event->type()) {
        case QEvent::TabletMove:
            polyLine[2] = polyLine[1];
            polyLine[1] = polyLine[0];
            polyLine[0] = event->pos();

            if (deviceDown) {
                updateBrush(event);
		QPainter painter;
		if(isEraser) {
                    painter.begin(&pixmap);
		} else {
	            painter.begin(&tempPixmap);
		}
                paintPixmap(painter, event);
            }
	    break;
        case QEvent::TabletPress:
            if (!deviceDown) {
                deviceDown = true;
                tempPixmap = QPixmap(width(), height());
                tempPixmap.fill(Qt::transparent);
                polyLine[0] = polyLine[1] = polyLine[2] = event->pos();
            }
	    break;
        case QEvent::TabletRelease:
            if (deviceDown) {
                deviceDown = false;
                QPixmap mergePixmap(width(), height());
                mergePixmap.fill(Qt::transparent);
                QPainter mergePainter(&mergePixmap);
                mergePainter.drawPixmap(0, 0, pixmap);
                mergePainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                mergePainter.drawPixmap(0, 0, tempPixmap);
                tempPixmap.fill(Qt::transparent);
                pixmap = mergePixmap;
		isDrawn = true;
                history.append(pixmap);
		undoCount++;
		tabletInputBusy = false;
	    }
	    break;
	default:
	    break;
    }
    update();
}

void TabletCanvas::mousePressEvent(QMouseEvent *event)
{
    if(tabletInputBusy)
        tabletInputBusy = false;
    deviceDown = true;
    myColor.setAlpha(brushOpacity);
    tempPixmap = QPixmap(width(), height());
    tempPixmap.fill(Qt::transparent);
    polyLine[0] = polyLine[1] = polyLine[2] = event->pos();
    update();
}

void TabletCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(deviceDown) {
        myPen.setWidthF(brushSize);
        if(!isTexture) {
            myBrush.setStyle(Qt::SolidPattern);
            myBrush.setColor(myColor);
        } else {
            myBrush.setTexture(myTexture);
        }
        myPen.setCapStyle(Qt::RoundCap);
        myPen.setBrush(myBrush);
        QPainter painter;
        if(isEraser) {
	    painter.begin(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        } else {
	    painter.begin(&tempPixmap);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
	}
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setBrush(myBrush);
        painter.setPen(myPen);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.drawPoint(event->pos());
        painter.end();
        QPixmap mergePixmap(width(), height());
        mergePixmap.fill(Qt::transparent);
        QPainter mergePainter(&mergePixmap);
        mergePainter.drawPixmap(0, 0, pixmap);
        mergePainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        mergePainter.drawPixmap(0, 0, tempPixmap);
        tempPixmap.fill(Qt::transparent);
        pixmap = mergePixmap;
        deviceDown = false;
        isDrawn = true;
        history.append(pixmap);
	undoCount++;
    }
    update();
}

void TabletCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if(tabletInputBusy)
        return;
    polyLine[2] = polyLine[1];
    polyLine[1] = polyLine[0];
    polyLine[0] = event->pos();

    if(deviceDown) {
        myPen.setWidthF(brushSize);
        if(!isTexture) {
            myBrush.setStyle(Qt::SolidPattern);
            myBrush.setColor(myColor);
        } else {
            myBrush.setTexture(myTexture);
        }
        myPen.setCapStyle(Qt::RoundCap);
        myPen.setBrush(myBrush);
        QPainter painter;
        if(isEraser) {
	    painter.begin(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        } else {
	    painter.begin(&tempPixmap);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
	}
        painter.setBrush(myBrush);
        painter.setPen(myPen);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.drawLine(polyLine[1], event->pos());
	painter.end();
    }
    update();
}


void TabletCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setOpacity(1.0 * qreal(qreal(opacity) / 255));
    painter.drawPixmap(0, 0, pixmap);
    painter.drawPixmap(0, 0, tempPixmap);
}

void TabletCanvas::paintPixmap(QPainter &painter, QTabletEvent *event)
{
    QPoint brushAdjust(10, 10);

    switch (myTabletDevice) {
        case QTabletEvent::Airbrush:
            if(!isTexture) {
                myBrush.setStyle(Qt::SolidPattern);
                myBrush.setColor(myColor);
            } else {
                myBrush.setTexture(myTexture);
            }
            myBrush.setStyle(brushPattern(event->pressure()));
            painter.setPen(Qt::NoPen);
            painter.setBrush(myBrush);

            for (int i = 0; i < 3; ++i) {
                painter.drawEllipse(QRect(polyLine[i] - brushAdjust,
                                    polyLine[i] + brushAdjust));
            }
            break;
        case QTabletEvent::Puck:
        case QTabletEvent::FourDMouse:
        case QTabletEvent::RotationStylus:
            {
                const QString error(tr("This input device is not supported by the example."));
#ifndef QT_NO_STATUSTIP
                QStatusTipEvent status(error);
                QApplication::sendEvent(this, &status);
#else
                qWarning() << error;
#endif
            }
            break;
        default:
            {
                const QString error(tr("Unknown tablet device - treating as stylus"));
#ifndef QT_NO_STATUSTIP
                QStatusTipEvent status(error);
                QApplication::sendEvent(this, &status);
#else
                qWarning() << error;
#endif
            }
        case QTabletEvent::Stylus:
            if(isEraser) {
                painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
            } else {
                painter.setCompositionMode(QPainter::CompositionMode_Source);
	    }
            painter.setBrush(myBrush);
            painter.setPen(myPen);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::HighQualityAntialiasing);
            painter.drawLine(polyLine[1], event->pos());
            break;
    }
}

Qt::BrushStyle TabletCanvas::brushPattern(qreal value)
{
    int pattern = int((value) * 100.0) % 7;

    switch (pattern) {
        case 0:
            return Qt::SolidPattern;
        case 1:
            return Qt::Dense1Pattern;
        case 2:
            return Qt::Dense2Pattern;
        case 3:
            return Qt::Dense3Pattern;
        case 4:
            return Qt::Dense4Pattern;
        case 5:
            return Qt::Dense5Pattern;
        case 6:
            return Qt::Dense6Pattern;
        default:
            return Qt::Dense7Pattern;
    }
}

void TabletCanvas::updateBrush(QTabletEvent *event)
{
    int hue, saturation, value, alpha;
    myColor.getHsv(&hue, &saturation, &value, &alpha);

    switch (alphaChannelType) {
        case AlphaPressure:
            myColor.setAlpha(int(event->pressure() * 255));
            break;
        default:
            myColor.setAlpha(brushOpacity);
    }

    switch (colorSaturationType) {
        case SaturationPressure:
            myColor.setHsv(hue, int(event->pressure() * 255.0), value, alpha);
            break;
        default:
            ;
    }

    switch (lineWidthType) {
        case LineWidthPressure:
            myPen.setWidthF(event->pressure() * brushSize + 1);
            break;
        default:
            myPen.setWidthF(brushSize);
    }
    myBrush.setColor(myColor);
    myPen.setCapStyle(Qt::RoundCap);
    myPen.setBrush(myBrush);
}

void TabletCanvas::resizeEvent(QResizeEvent *)
{
    initPixmap(pixmap);
    polyLine[0] = polyLine[1] = polyLine[2] = QPoint();
}

void TabletCanvas::setEraser()
{
    isEraser = true;
    myBrush.setColor(Qt::white);
    myPen.setColor(Qt::white);
}

void TabletCanvas::setStylus()
{
    isEraser = false;
    if(!isTexture) {
        myBrush.setStyle(Qt::SolidPattern);
        myBrush.setColor(myColor);
    } else {
        myBrush.setTexture(myTexture);
    }
    myPen.setBrush(myBrush);
}

QPixmap TabletCanvas::undo()
{
    if(isDrawn) {
        undoCount--;
        if(undoCount >= 0) 
            history.removeAt(undoCount + 1);
	else 
            undoCount = 0;
        return history.at(undoCount);
    }
    return pixmap;
}

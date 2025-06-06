/****************************************************************************
    Copyright (C) 2012-2015 Omer Bahri Gordebak <purplehuman at zoho.com>
    Most of this code is from windel at qt-apps.org, there weren't any 
    copyright notice in the files, so I'll give you the link to the project 
    page: 
    <http://qt-apps.org/content/show.php/Qt+Opencv+webcam+viewer?content=89995>

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

#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include <opencv/cv.h>
#include <QPixmap>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QImage>

class QOpenCVWidget : public QWidget {
	
private:
    QHBoxLayout *layout;
        
    QImage image;
        
public:
    QOpenCVWidget(QWidget *parent = 0);
    ~QOpenCVWidget(void);
    void putImage(IplImage *);
    QLabel *imagelabel;
}; 

#endif

CONFIG += qt
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia svg

TARGET = oinut
TEMPLATE = app

QMAKE_STRIP = echo

RCC_DIR     = $$PWD/rcc
UI_DIR      = $$PWD/uic
MOC_DIR     = $$PWD/moc
OBJECTS_DIR = $$PWD/obj

LIBS += -lopencv_core -lopencv_highgui -lquazip-qt5 -lz -lsane

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
        src/sizedialog.cpp \
        src/videodialog.cpp \
        src/QOpenCVWidget.cpp \
        src/layernamedialog.cpp \
        src/cutoutdialog.cpp \
        src/drawingdialog.cpp \
        src/brushsettings.cpp \
        src/tabletcanvas.cpp \
        src/opacitydialog.cpp \
        src/scandialog.cpp \
        src/blurdialog.cpp

HEADERS  += src/mainwindow.h \
        src/sizedialog.h \
        src/videodialog.h \
        src/QOpenCVWidget.h \
        src/layernamedialog.h \
        src/cutoutdialog.h \
        src/drawingdialog.h \
        src/brushsettings.h \
        src/tabletcanvas.h \
        src/opacitydialog.h \
        src/scandialog.h \
        src/blurdialog.h

FORMS    += src/ui/mainwindow.ui \
        src/ui/sizedialog.ui \
        src/ui/videodialog.ui \
        src/ui/layernamedialog.ui \
        src/ui/cutoutdialog.ui \
        src/ui/drawingdialog.ui \
        src/ui/brushsettings.ui \
        src/ui/opacitydialog.ui \
        src/ui/scandialog.ui \
        src/ui/blurdialog.ui

RESOURCES += \
        resource/oinut.qrc \
        
target.path = /usr/bin
desktop.path = /usr/share/applications
desktop.files = resource/oinut.desktop
icon.path = /usr/share/pixmaps
icon.files = resource/oinut.png
mimeicon.path = /usr/share/pixmaps
mimeicon.files += resource/application-x-oi*.png
        
INSTALLS += target \
        desktop \
        icon \
        mimeicon

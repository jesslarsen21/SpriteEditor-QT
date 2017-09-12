#-------------------------------------------------
#
# Project created by QtCreator 2016-10-23T17:31:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JPixel
TEMPLATE = app


SOURCES += main.cpp\
        view.cpp \
    pen.cpp \
    spritesheet.cpp \
    animation.cpp \
    jgraphicsview.cpp \
    dialog.cpp \
    qclickablelabel.cpp

HEADERS  += view.h \
    pen.h \
    spritesheet.h \
    animation.h \
    jgraphicsview.h \
    dialog.h \
    qclickablelabel.h \
    gif.h

FORMS    += view.ui \
    dialog.ui

RESOURCES += \
    guiresources.qrc

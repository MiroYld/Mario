#-------------------------------------------------
#
# Project created by QtCreator 2018-02-11T20:42:27
#
#-------------------------------------------------

QT += core gui opengl qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mario
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    model.cpp \
    controller.cpp \
    tilemapeditor.cpp \
    viewgl.cpp \
    sprite.cpp \
    spriteselectdialog.cpp \
    spriteselect.cpp \
    serialize.cpp \
    spritemodeleditor.cpp \
    scripteditordialog.cpp \
    scripthighlighter.cpp \
    linenumberarea.cpp \
    scripteditor.cpp


HEADERS  += \
    mainwindow.h \
    model.h \
    controller.h \
    tilemapeditor.h \
    viewgl.h \
    sprite.h \
    spriteselectdialog.h \
    spriteselect.h \
    deselectabletreewidget.h \
    spritemodeleditor.h \
    scripteditordialog.h \
    scripthighlighter.h \
    linenumberarea.h \
    scripteditor.h

linux-g++ | linux-g++-64 | linux-g++-32 {
    LIBS += -lGL
    CONFIG += c++11
}

win32 {
    LIBS += -lOpenGL32
}

RESOURCES = ressources.qrc

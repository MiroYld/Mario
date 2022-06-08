#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "model.h"
#include "view.h"
#include "viewgl.h"
#include "controller.h"
#include "tilemapeditor.h"
#include "spritemodeleditor.h"

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void update();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    void quitDialog();
    void showFPS();
    QTime time;
    int frameCount = 0;
    float framerate = 1;

    QHBoxLayout* hbox;
    QPixmap *tileset, *spritesheet;
    Model model;
    ViewGL *view;
    Controller *controller;
    QTabWidget *editorLeft, *editorRight;
    TileMapEditor *tilemapEditor;
    SpriteModelEditor *spriteModelEditor;
    QTimer* timer, *viewTimer;
};

#endif // MAINWINDOW_H

#ifndef CONTROLLER_H
#define CONTROLLER_H

#define SCROLL_SPEED 180

#include <QtWidgets>
#include "model.h"
#include "unordered_map"

class Controller{
public:
    Controller(Model& m);
    void update(float dt);
    void tileEditor();
    void spriteEditor();
    void mouseButtonPressEvent(QMouseEvent*);
    void mouseButtonReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private:
    Model& m;
    std::unordered_map<int,bool> keys;
    bool clickLeft = false, clickRight = false;
    int mx = 0, my = 0;
    Sprite* sprite = NULL;
};

#endif // CONTROLLER_H

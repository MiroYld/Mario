#ifndef SPRITESELECT_H
#define SPRITESELECT_H

#include <QWidget>
#include "QtWidgets"

enum select_type{
    SPRITE,
    COLLIDER,
    ORIGIN
};

class SpriteSelect : public QWidget{
    Q_OBJECT
public:
    explicit SpriteSelect(QWidget *parent, QPixmap *image, float scale);
    QRect sprite, collider;
    QPoint origin;
    select_type type;
protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    int x1 = -1,y1 = -1,x2 = -1,y2 = -1;
    QPixmap* image;
    float scale;
};

#endif // SPRITESELECT_H

#include "spriteselect.h"

SpriteSelect::SpriteSelect(QWidget *parent, QPixmap *image, float scale) : QWidget(parent), image(image), scale(scale){
    setGeometry(0,0,image->width()*scale,image->height()*scale);
}

void SpriteSelect::paintEvent(QPaintEvent*){
    QPainter painter(this);
    painter.scale(scale,scale);
    //Draw gray background because the spritesheet is transparent
    painter.fillRect(this->geometry(),QBrush(QColor(150,150,150)));
    //draw spritesheet
    painter.drawPixmap(0,0,image->width(), image->height(),*image);
    //draw sprite selection in blue
    painter.setPen(QPen(QBrush(Qt::blue),0.3));
    painter.drawRect(sprite);
    //draw collider selection in red
    painter.setPen(QPen(QBrush(Qt::red),0.3));
    painter.drawRect(collider);
    //draw origin point in yellow
    painter.setPen(QPen(QBrush(Qt::yellow),0.3));
    painter.drawRect(origin.x(),origin.y(),1,1);
}

//Enter selection
void SpriteSelect::mousePressEvent(QMouseEvent *event){
    if(type == SPRITE){
        sprite.setX(event->x()/scale);
        sprite.setY(event->y()/scale);
        sprite.setWidth(0);
        sprite.setHeight(0);
    }else if(type == COLLIDER){
        collider.setX(event->x()/scale);
        collider.setY(event->y()/scale);
        collider.setWidth(0);
        collider.setHeight(0);
    }else if(type == ORIGIN){
        origin.setX(event->x()/scale);
        origin.setY(event->y()/scale);
    }
}

//Finish selection
void SpriteSelect::mouseMoveEvent(QMouseEvent *event){
    if(type == SPRITE){
        if(event->x()/scale < sprite.x())
            sprite.setX(event->x()/scale);
        sprite.setWidth(abs(event->x()/scale - sprite.x()));
        if(event->y()/scale < sprite.y())
            sprite.setY(event->y()/scale);
        sprite.setHeight(abs(event->y()/scale - sprite.y()));
    }else if(type == COLLIDER){
        if(event->x()/scale < collider.x())
            collider.setX(event->x()/scale);
        collider.setWidth(abs(event->x()/scale - collider.x()));
        if(event->y()/scale < collider.y())
            collider.setY(event->y()/scale);
        collider.setHeight(abs(event->y()/scale - collider.y()));
    }else if(type == ORIGIN){
        origin.setX(event->x()/scale);
        origin.setY(event->y()/scale);
    }
    update();
}

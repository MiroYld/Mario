#ifndef VIEWGL_H
#define VIEWGL_H

#include <QtWidgets>
#include <QGLWidget>
#include "model.h"

class ViewGL : public QGLWidget{
Q_OBJECT
public:
    explicit ViewGL(QWidget *parent, Model &m, QPixmap* tileset, QPixmap* spritesheet);
protected:
    void initializeGL();
    void paintGL();
private:
    Model &m;
    QOpenGLTexture *tileset, *spritesheet;
    QPixmap *tilesetpixmap, *spritesheetpixmap;
    QFont font;
};

#endif // VIEWGL_H

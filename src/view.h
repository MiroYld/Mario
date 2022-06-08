#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QtWidgets>
#include "model.h"

class View : public QWidget{
    Q_OBJECT
public:
    explicit View(QWidget *parent, Model &m, QPixmap *tileset, QPixmap *spritesheet);
protected:
    void paintEvent(QPaintEvent*);
private:
    Model &m;
    QPixmap *tileset, *spritesheet;
};

#endif // VIEW_H

#ifndef TILEMAPEDITOR_H
#define TILEMAPEDITOR_H

#include <QWidget>
#include <QtWidgets>
#include "model.h"

#define CHECKBOX_OFFSET 18

class TileMapEditor : public QWidget{
    Q_OBJECT
public:
    explicit TileMapEditor(QWidget *parent, Model& m, QPixmap* tileset);

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);

private:
    Model& m;
    QCheckBox* solid;
    QPixmap* tileset;
    QLabel* image;
    float scale;

public slots:
   void checkBoxChanged(int);
};

#endif // TILEMAPEDITOR_H

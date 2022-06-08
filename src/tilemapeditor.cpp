#include "tilemapeditor.h"

TileMapEditor::TileMapEditor(QWidget *parent, Model& m, QPixmap* tileset)
                        : QWidget(parent), m(m),tileset(tileset){
    solid = new QCheckBox("Solid", this);
    solid->setFocusPolicy(Qt::NoFocus);
    solid->setChecked(m.tilesolid[m.tileLeft] == 1);
    connect(solid, SIGNAL(stateChanged(int)), this, SLOT(checkBoxChanged(int)));
}

void TileMapEditor::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QPen pen = painter.pen();
    pen.setWidth(2);
    scale = static_cast<float>(parentWidget()->width())/tileset->width();
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0,CHECKBOX_OFFSET,tileset->width()*scale,tileset->height()*scale,*tileset);
    //show the solid tiles in debug mode
    if(m.useGrid){
        pen.setColor(Qt::blue);
        painter.setPen(pen);
        for(int x = 0; x < TILESET_WIDTH; ++x)
            for(int y = 0; y < TILESET_HEIGHT; ++y)
                if(m.tilesolid[x+TILESET_WIDTH*y] == 1)
                    painter.drawText(scale*TILE_SIZE/3+scale*TILE_SIZE*x,scale*TILE_SIZE*3/4+CHECKBOX_OFFSET+scale*TILE_SIZE*y,"S");
    }
    //display right click cursor
    pen.setColor(Qt::blue);
    painter.setPen(pen);
    painter.drawRect(scale*TILE_SIZE*(m.tileRight%TILESET_WIDTH), CHECKBOX_OFFSET+scale*TILE_SIZE*(m.tileRight/TILESET_WIDTH),
                     scale*TILE_SIZE, scale*TILE_SIZE);
    //display left click cursor
    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawRect(scale*TILE_SIZE*(m.tileLeft%TILESET_WIDTH), CHECKBOX_OFFSET+scale*TILE_SIZE*(m.tileLeft/TILESET_WIDTH),
                     scale*TILE_SIZE, scale*TILE_SIZE);
}

void TileMapEditor::mousePressEvent(QMouseEvent* event){
    if(event->x() > scale*tileset->width() || event->y()-CHECKBOX_OFFSET > scale*tileset->height() || event->y() < CHECKBOX_OFFSET)
        return;
    if(event->button() == Qt::LeftButton){
        m.tileLeft = static_cast<int>(event->x()/scale/TILE_SIZE) +
               static_cast<int>((event->y()-CHECKBOX_OFFSET)/scale/TILE_SIZE)*TILESET_WIDTH;
        solid->setChecked(m.tilesolid[m.tileLeft] == 1);
        this->update();
    }else if(event->button() == Qt::RightButton){
        m.tileRight = static_cast<int>(event->x()/scale/TILE_SIZE) +
               static_cast<int>((event->y()-CHECKBOX_OFFSET)/scale/TILE_SIZE)*TILESET_WIDTH;
        this->update();
    }
}

void TileMapEditor::checkBoxChanged(int state){
    m.tilesolid[m.tileLeft] = (state == Qt::Checked ? 1 : 0);
    update();
}

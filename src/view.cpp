#include "view.h"

View::View(QWidget *parent, Model &m, QPixmap *tileset, QPixmap *spritesheet)
    : QWidget(parent), m(m), tileset(tileset), spritesheet(spritesheet){

    this->setAttribute(Qt::WA_NoBackground);
    this->setAttribute(Qt::WA_NoSystemBackground);
    setMinimumSize(WIDTH, HEIGHT);
    setMaximumSize(WIDTH, HEIGHT);
}

void View::paintEvent(QPaintEvent*){
    int offset = m.useGrid ? -1 : 0;
    QPainter painter(this);
    //draw tilemap
    for(int x = 0; x < SCREEN_WIDTH_PLUS; ++x)
        for(int y = 0; y < SCREEN_HEIGHT_PLUS; ++y)
            painter.drawPixmap((x+m.ox)*TILE_SIZE-m.sx,(y+m.oy)*TILE_SIZE-m.sy,
                               TILE_SIZE + offset,TILE_SIZE + offset,
                               *tileset,
                               (m.map[x+m.ox][y+m.oy]%19)*TILE_SIZE,
                               (m.map[x+m.ox][y+m.oy]/19)*TILE_SIZE,
                               TILE_SIZE,TILE_SIZE);
}

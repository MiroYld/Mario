#include "viewgl.h"
/*(Uses old Open GL features)*/

ViewGL::ViewGL(QWidget *parent, Model &m, QPixmap* tileset, QPixmap* spritesheet):
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent), m(m),tilesetpixmap(tileset),spritesheetpixmap(spritesheet){
    setFixedSize(WIDTH*SCALE,HEIGHT*SCALE);
    //Load font for the stat display
    font.setFamily("Press Start 2P");
    font.setPixelSize(26);
}

//Initialize Open GL
void ViewGL::initializeGL(){
    //Setup for 2D rendering: enable transparent texture, disable depth test
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(2);
    glPointSize(4);
    //Load textures
    tileset = new QOpenGLTexture(tilesetpixmap->toImage());
    tileset->setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Nearest);
    spritesheet = new QOpenGLTexture(spritesheetpixmap->toImage());
    spritesheet->setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Nearest);
    //Setup the Viewport, the Projection matrix and mirror horizontally the display
    glViewport(0, 0, WIDTH*SCALE, HEIGHT*SCALE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glScalef(1, -1, 1);
    glTranslatef(0, -HEIGHT, 0);
}

//Render
void ViewGL::paintGL(){
    int offest = m.useGrid ? -1 : 0;
    float to = m.useGrid ? -1./TILE_SIZE/TILESET_HEIGHT : 0;
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Draw the tilemap
    tileset->bind();
    glDisable(GL_BLEND);
    glBegin(GL_QUADS);
    for(int x = 0; x < SCREEN_WIDTH_PLUS; ++x)
        for(int y = 0; y < SCREEN_HEIGHT_PLUS; ++y){
            int posx = (x+m.ox)*TILE_SIZE-round(m.sx);
            int posy = (y+m.oy)*TILE_SIZE-round(m.sy);
            float srcx = (float)(m.map[x+m.ox][y+m.oy]%TILESET_WIDTH)/TILESET_WIDTH;
            float srcy = (float)(m.map[x+m.ox][y+m.oy]/TILESET_WIDTH)/TILESET_HEIGHT;
            glTexCoord2f( srcx,                  srcy+1./TILESET_HEIGHT+to);
            glVertex2i(   posx,                  posy+TILE_SIZE+offest);
            glTexCoord2f( srcx,                  srcy);
            glVertex2i(   posx,                  posy);
            glTexCoord2f( srcx+1./TILESET_WIDTH+to/2, srcy);
            glVertex2i(   posx+TILE_SIZE+offest, posy);
            glTexCoord2f( srcx+1./TILESET_WIDTH+to/2, srcy+1./TILESET_HEIGHT+to);
            glVertex2i(   posx+TILE_SIZE+offest, posy+TILE_SIZE+offest);
        }
    glEnd();

    //Draw the sprites
    spritesheet->bind();
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    for(Sprite* sprite = (Sprite*)m.sprites.data(); sprite - m.sprites.data() < (int)m.sprites.size(); ++sprite){
        QRectF i;
        if(sprite->state->solid)
            i = sprite->imageRect(m.sx, m.sy);
        else
            i = sprite->imageRect(round(m.sx), round(m.sy));
        QRectF s = sprite->sourceRect(spritesheet->width(),spritesheet->height());
        glTexCoord2f( s.left(),  s.bottom());
        glVertex2i(   i.left(),  i.bottom());
        glTexCoord2f( s.left(),  s.top());
        glVertex2i(   i.left(),  i.top());
        glTexCoord2f( s.right(), s.top());
        glVertex2i(   i.right(), i.top());
        glTexCoord2f( s.right(), s.bottom());
        glVertex2i(   i.right(), i.bottom());
    }
    glEnd();

    //Draw the game stat infos
    if(m.gamemode){
        renderText(46,50,"MARIO         WORLD   TIME",font);
        renderText(46,80,QString("%1").arg(m.score, 6, 10, QChar('0'))
                   +QString("  $%1").arg(m.coins, 2, 10, QChar('0'))
                   +QString("    %1-").arg(m.world/10, 1, 10)+QString::number(m.world%10)
                   +"     "+(m.time > 0 ? QString("%1").arg((int)m.time, 3, 10, QChar(' ')) : "   "),font);
    }

    //Draw the debug infos
    if(m.useGrid){
        glDisable(GL_BLEND);
        for(Sprite* sprite = (Sprite*)m.sprites.data(); sprite - m.sprites.data() < (int)m.sprites.size(); ++sprite){
            glBegin(GL_LINE_LOOP);
            QRectF c = sprite->colliderRect(m.sx, m.sy);
            glVertex2f(c.left(),  c.bottom());
            glVertex2f(c.left(),  c.top());
            glVertex2f(c.right(), c.top());
            glVertex2f(c.right(), c.bottom());
            glEnd();
            glBegin(GL_POINTS);
            glVertex2f(sprite->x-m.sx, sprite->y-m.sy);
            glEnd();
            renderText(c.left()*SCALE,c.top()*SCALE-15,sprite->model->name);
            renderText(c.left()*SCALE,c.top()*SCALE-3,sprite->state->name);
        }
    }
}

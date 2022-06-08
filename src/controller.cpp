#include "controller.h"

Controller::Controller(Model& m):m(m){}

void Controller::update(float dt){
    //Don't update during dialogs
    if(!m.focus){
        keys.clear();
        return;
    }
    if(m.gamemode){
        //Game Engine update
        m.engine.globalObject().setProperty("KeyRight",QJSValue(keys[Qt::Key_Right] || keys[Qt::Key_D]));
        m.engine.globalObject().setProperty("KeyLeft",QJSValue(keys[Qt::Key_Left] || keys[Qt::Key_Q]));
        m.engine.globalObject().setProperty("KeyUp",QJSValue(keys[Qt::Key_Up] || keys[Qt::Key_Z]));
        m.engine.globalObject().setProperty("KeyDown",QJSValue(keys[Qt::Key_Down] || keys[Qt::Key_S]));
        m.engine.globalObject().setProperty("KeyJump",QJSValue(keys[Qt::Key_Space] || keys[Qt::Key_Shift]));
        m.engine.globalObject().setProperty("KeyRun",QJSValue(keys[Qt::Key_Control] || keys[Qt::Key_Alt] || keys[Qt::Key_AltGr]));
        m.engine.globalObject().setProperty("ResetLevel",QJSValue(false));
        m.update(dt);
        if(m.engine.globalObject().property("ResetLevel").toBool())
            m.load(true);
    }else{
        //Game Editor update
        if(keys[Qt::Key_Right] || keys[Qt::Key_D])
            m.scroll(SCROLL_SPEED*dt,0);
        else if(keys[Qt::Key_Left] || keys[Qt::Key_Q])
            m.scroll(-SCROLL_SPEED*dt,0);
        if(keys[Qt::Key_Up] || keys[Qt::Key_Z])
            m.scroll(0,-SCROLL_SPEED*dt);
        else if(keys[Qt::Key_Down] || keys[Qt::Key_S])
            m.scroll(0,SCROLL_SPEED*dt);

        tileEditor();
        spriteEditor();
    }
}

//Handle tile drawing
void Controller::tileEditor(){
    if(m.spriteModel != NULL)
        return;
    if(clickLeft)
        m.map[(int)(mx+m.sx)/TILE_SIZE][(int)(my+m.sy)/TILE_SIZE] = m.tileLeft;
    else if(clickRight)
        m.map[(int)(mx+m.sx)/TILE_SIZE][(int)(my+m.sy)/TILE_SIZE] = m.tileRight;
}

//Handle sprite drawing
void Controller::spriteEditor(){
    if(m.spriteModel == NULL)
        return;
    if(!clickLeft && !clickRight){//unselect
        this->sprite = NULL;
        return;
    }
    int x = mx+m.sx;
    int y = my+m.sy;
    if(this->sprite != NULL){//move sprite
        this->sprite->x = x + this->sprite->frame->origin.x() - this->sprite->frame->image.center().x();
        this->sprite->y = y + this->sprite->frame->origin.y() - this->sprite->frame->image.center().y();
        return;
    }
    Sprite* sprite = NULL;
    for(Sprite* s = m.sprites.data(); s < m.sprites.data()+m.sprites.size(); s++)
        if(s->imageRect(m.sx,m.sy).contains(mx,my))
            sprite = s;
    if(clickLeft && sprite == NULL && m.spriteModel->states.size() != 0 && m.spriteModel->states[0]->frames.size() != 0){ //create sprite
        m.sprites.push_back(Sprite(m.spriteModel,
            x + m.spriteModel->states[0]->frames[0]->origin.x() - m.spriteModel->states[0]->frames[0]->image.center().x(),
            y + m.spriteModel->states[0]->frames[0]->origin.y() - m.spriteModel->states[0]->frames[0]->image.center().y()));
        this->sprite = m.sprites.data() + m.sprites.size() - 1;
    }else if(clickLeft && sprite != NULL){ //select sprite
        this->sprite = sprite;
    }else if(clickRight && sprite != NULL){ //delete sprite
        *sprite = m.sprites[m.sprites.size()-1];
        m.sprites.pop_back();
    }
}

//Start Drawing
void Controller::mouseButtonPressEvent(QMouseEvent *event){
    float ex = event->x()/SCALE;
    float ey = event->y()/SCALE;
    if(ex > WIDTH || ex < 0 || ey > HEIGHT || ey < 0)
        return;
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton || event->button() == Qt::MiddleButton){
        mx = ex;
        my = ey;
    }
    if(event->button() == Qt::LeftButton)
        clickLeft = true;
    else if(event->button() == Qt::RightButton)
        clickRight = true;
}

//Finish Drawing
void Controller::mouseButtonReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton)
        clickLeft = false;
    else if(event->button() == Qt::RightButton)
        clickRight = false;
}

void Controller::mouseMoveEvent(QMouseEvent *event){
    float ex = event->x()/SCALE;
    float ey = event->y()/SCALE;
    if(ex > WIDTH || ex < 0 || ey > HEIGHT || ey < 0)
        return;
    if(event->buttons().testFlag(Qt::LeftButton) || event->buttons().testFlag(Qt::RightButton)){
        mx = ex;
        my = ey;
    }else if(event->buttons().testFlag(Qt::MiddleButton) && !m.gamemode){
        m.scroll(mx-ex,my-ey);
        mx = ex;
        my = ey;
    }
}

void Controller::keyPressEvent(QKeyEvent *event){
    if(event->isAutoRepeat())
        return;
    //qDebug() << "press" << event->key();
    keys[event->key()] = true;
}

void Controller::keyReleaseEvent(QKeyEvent *event){
    if(event->isAutoRepeat())
        return;
    //qDebug() << "release" << event->key();
    keys[event->key()] = false;
}

#include "sprite.h"
#include "model.h"

//Get collider rect in screen coordinates
QRectF Sprite::colliderRect(float sx, float sy){
    return QRectF(
        x+model->scale*(frame->collider.center().x()-frame->collider.width()/2-frame->origin.x())-sx,
        y+model->scale*(frame->collider.center().y()-frame->collider.height()/2-frame->origin.y())-sy,
        model->scale*frame->collider.width(),
        model->scale*frame->collider.height()
    );
}

//Get image rect in screen coordinates
QRectF Sprite::imageRect(float sx, float sy){
    return QRectF(
        x+model->scale*(frame->image.center().x()-frame->image.width()/2-frame->origin.x())-sx,
        y+model->scale*(frame->image.center().y()-frame->image.height()/2-frame->origin.y())-sy,
        model->scale*frame->image.width(),
        model->scale*frame->image.height()
    );
}

//Get texture in opengl texture coordinates (from 0 to 1)
QRectF Sprite::sourceRect(float w, float h){
    return QRectF(
        frame->image.x()/w,
        frame->image.y()/h,
        frame->image.width()/w,
        frame->image.height()/h
    );
}

unsigned Sprite::currentFrame(){
    unsigned j;
    for(j = 0; j < state->frames.size(); ++j)
        if(frame == state->frames[j].get())
            break;
    return j;
}

//C++ side sprite update for performance porposes
bool Sprite::update(float dt){
    //apply acceleration
    vx += dt*ax;
    vy += dt*ay;
    //clamp velocity
    if(vx > mvx) vx = mvx;
    if(vx <-mvx) vx =-mvx;
    if(vy > mvy) vy = mvy;
    if(vy <-mvy) vy =-mvy;
    //apply velocity
    x += vx*dt;
    y += vy*dt;
    //update animation counter
    t += dt*1000;
    if(t < state->speed)
        return false;
    t = 0;
    if(state->frames.size() <= 1)
        return true;
    //update sprite animation
    if(frame == state->frames.back().get()){
        frame = state->frames.front().get();
        return true;
    }
    frame = state->frames[currentFrame()+1].get();
    return true;
}

#ifndef SPRITE_H
#define SPRITE_H

#include "QRect"
#include "QString"
#include <QObject>
#include <vector>
#include <memory>
#include <algorithm>
#include <QJSEngine>

#define MAX_SPEED 200

//We're using structs here because it's mostly composed of public data.
//Almost no hieritance in this program because I'm managing all the game object dynamically.

struct Frame{
    Frame(){}
    Frame(const Frame& f):image(f.image),collider(f.collider),origin(f.origin){}
    Frame(QRect image, QRect collider, QPoint origin):image(image),collider(collider),origin(origin){}
    QRect image, collider;
    QPoint origin;
};

struct State{
    QString name = QObject::tr("new State");
    QString script;
    std::vector<std::unique_ptr<Frame>> frames;
    unsigned speed = 200;
    bool solid = true;
};

struct SpriteModel{
    QString name = QObject::tr("new Sprite");
    QString script;
    float scale = 1;
    std::vector<std::unique_ptr<State>> states;
};

struct Sprite{
    Sprite(){}
    Sprite(SpriteModel* model,float x,float y,float vx=0,float vy=0,float ax=0,float ay=0,float mvx=100,float mvy=200)
                            :x(x),y(y),vx(vx),vy(vy),ax(ax),ay(ay),mvx(mvx),mvy(mvy),model(model){
        state = (State*) model->states[0].get();
        frame = (Frame*) state->frames[0].get();
    }

    QRectF colliderRect(float sx = 0, float sy = 0);
    QRectF imageRect(float sx = 0, float sy = 0);
    QRectF sourceRect(float w, float h);
    unsigned currentFrame();
    bool update(float dt);

    float x,y,vx,vy,ax,ay;
    float mvx,mvy;//max velocity
    float t = 0;//animation counter
    //last collision state
    unsigned char collideSprite, collideTile;
    //Current model, state and frame
    SpriteModel* model;
    State* state;
    Frame* frame;
};

#endif // SPRITE_H

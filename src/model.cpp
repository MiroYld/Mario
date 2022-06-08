#include "model.h"
#include <QDebug>

//Camera scroll
void Model::scroll(float dx, float dy){
    sx += dx;
    if(sx < 0)//clamp x axis
        sx = 0;
    else if(sx > MAP_WIDTH*TILE_SIZE-WIDTH)
        sx = MAP_WIDTH*TILE_SIZE-WIDTH;

    sy += dy;
    if(sy < 0)//clamp y axis
        sy = 0;
    else if(sy > MAP_HEIGHT*TILE_SIZE-HEIGHT)
        sy = MAP_HEIGHT*TILE_SIZE-HEIGHT;

    ox = sx/TILE_SIZE;
    oy = sy/TILE_SIZE;
    //qDebug() << sx << sy;
}

//Start the Game
void Model::start(){
    if(!gamemode){
        console->setTextColor(Qt::darkGreen);
        console->setText("Simulation start");
        save(true);
        gamemode = true;
        score = 0;
        coins = 0;
        world = 11;
        time = 400;
    }
}

//Reset the Game
void Model::reset(){
    console->setTextColor(Qt::darkGreen);
    console->append("Simulation reset");
    load(true);
    gamemode = false;
}

//Update the Game
void Model::update(float dt){
    time -= dt;
    prepareScript(dt);
    eval("GS:", globalScript);
    QRectF visible_screen(-UPDATE_MARGIN,-UPDATE_MARGIN,WIDTH+UPDATE_MARGIN,HEIGHT+UPDATE_MARGIN);
    std::vector<unsigned> despawnList;
    for(unsigned i = 0; i<sprites.size(); ++i){
        if(!visible_screen.intersects(sprites[i].imageRect(sx,sy)))
            continue;
        bool frame = sprites[i].update(dt);
        bool collide = collisions(sprites.data()+i, dt);
        if(frame || collide){
            prepareScript(sprites[i]);
            if(eval(sprites[i].model->name, sprites[i].model->script))
                return;
            if(eval(sprites[i].model->name+":"+sprites[i].state->name, sprites[i].state->script))
                return;
            if(endScript(sprites[i]))
                despawnList.push_back(i);
        }
    }
    endScript();
    while(despawnList.size() > 0){
        unsigned i = despawnList.back();
        despawnList.pop_back();
        sprites.erase(sprites.begin() + i);
    }
}

//Javascript eval displaying logs
bool Model::eval(QString context, QString script){
    engine.globalObject().setProperty("log","");
    QJSValue result = engine.evaluate(script);
    if(result.isError()){
        console->setTextColor(Qt::red);
        console->append(context+":"+result.property("lineNumber").toString() + ": " + result.toString());
        return true;
    }else{
        QString log = engine.globalObject().property("log").toString();
        if(log.size() > 0){
            console->setTextColor(Qt::black);
            console->append(log);
        }
        return false;
    }
}

//Check tile collisions
bool Model::tileCollision(QRectF collider){
    int minX = static_cast<int>(collider.left()/TILE_SIZE);
    int maxX = static_cast<int>(collider.right()/TILE_SIZE);
    int minY = static_cast<int>(collider.top()/TILE_SIZE);
    int maxY = static_cast<int>(collider.bottom()/TILE_SIZE);
    for(int x = minX; x <= maxX; ++x)
        for(int y = minY; y <= maxY; ++y)
            if(x>=0 && x<MAP_WIDTH && y>=0 && y<MAP_HEIGHT)
                if(tilesolid[map[x][y]] == 1)
                    return true;
    return false;
}

//Check collisions with sprites and tiles; then cancel motion to stop them from overlapping
//Fill also the javascript variables accordingly
//(It might be a good idea to remplace some of this code with Box2D)
//Return true if there is a new collision
bool Model::collisions(Sprite* s, float dt){
    unsigned char testTile = 0, testSprite = 0;
    QJSValue collisions = engine.newObject(), spriteCollisions = engine.newArray();
    QRectF collider = s->colliderRect(0,0);
    //Tile collisions
    bool left = tileCollision(QRectF(collider.left(),collider.top()+1,1,collider.height()-2));
    bool right = tileCollision(QRectF(collider.right()-1,collider.top()+1,1,collider.height()-2));
    bool top = tileCollision(QRectF(collider.left()+1,collider.top(),collider.width()-2,1));
    bool bottom = tileCollision(QRectF(collider.left()+1,collider.bottom()-1,collider.width()-2,1));
    if(left || right || top || bottom){
        //Handle false activations
        if(left && top && bottom && right){
            //Worst case scenario: all activated, recover from last time
            left = s->collideTile&2;
            right = s->collideTile&4;
            top = s->collideTile&8;
            bottom = s->collideTile&16;
            if((left || right) && !(top || bottom)){
                if(s->vy > 0)
                    bottom = true;
                else
                    top = true;
            }else if((top || bottom) && !(left || right)){
                if(s->vx > 0)
                    right = true;
                else
                    left = true;
            }
        }
        //3/4 activated
        if((left && top && bottom && !right) || (!left && top && bottom && right))
            top = bottom = false;
        if((left && !top && bottom && right) || (left && top && !bottom && right))
            left = right = false;
        //Cancel motion
        if(s->state->solid){
            if((left && s->vx < 0) || (right && s->vx > 0)){
                s->x -= s->vx * dt/2;
                s->vx = 0;
            }if((top && s->vy < 0) || (bottom && s->vy > 0)){
                s->y -= s->vy * dt/2;
                s->vy = 0;
            }
        }
    }

    collisions.setProperty("tile",QJSValue(left||right||top||bottom));
    collisions.setProperty("left",QJSValue(left));
    collisions.setProperty("right",QJSValue(right));
    collisions.setProperty("top",QJSValue(top));
    collisions.setProperty("bottom",QJSValue(bottom));
    testTile = (left?3:0) | (right?5:0) | (top?9:0) | (bottom?17:0);
    //Sprite collisions
    for(unsigned i = 0; i < sprites.size(); ++i)
        if(sprites.data()+i != s && collider.intersects(sprites[i].colliderRect(0,0))){
            QJSValue collision = engine.newObject();
            collision.setProperty("model", QJSValue(sprites[i].model->name));
            collision.setProperty("state", QJSValue(sprites[i].state->name));
            spriteCollisions.setProperty(testSprite++,collision);
        }
    collisions.setProperty("sprite",spriteCollisions);
    engine.globalObject().setProperty("collisions",collisions);
    //convert state into trigger
    std::swap(testSprite, s->collideSprite);
    testSprite -= s->collideSprite;
    std::swap(testTile, s->collideTile);
    testTile -= s->collideTile;
    return testSprite != 0 || testTile != 0;
}

//Fills the main javascript property
void Model::prepareScript(float dt){
    engine.globalObject().setProperty("dt",QJSValue(dt));
    engine.globalObject().setProperty("sx",QJSValue(sx));
    engine.globalObject().setProperty("sy",QJSValue(sy));
    engine.globalObject().setProperty("score",QJSValue(score));
    engine.globalObject().setProperty("coins",QJSValue(coins));
    engine.globalObject().setProperty("world",QJSValue(world));
    engine.globalObject().setProperty("time",QJSValue(time));
}

//Recover the main javascript property
void Model::endScript(){
    if(engine.globalObject().property("sx").isNumber()
            && engine.globalObject().property("sy").isNumber()){
        float dx = engine.globalObject().property("sx").toNumber() - sx;
        float dy = engine.globalObject().property("sy").toNumber() - sy;
        if(dx != 0 || dy != 0)
            scroll(dx,dy);
    }
    if(engine.globalObject().property("score").isNumber())
        score = engine.globalObject().property("score").toNumber();
    if(engine.globalObject().property("coins").isNumber())
        coins = engine.globalObject().property("coins").toNumber();
    if(engine.globalObject().property("world").isNumber())
        world = engine.globalObject().property("world").toNumber();
    if(engine.globalObject().property("time").isNumber())
        time = engine.globalObject().property("time").toNumber();
}

//Fills the main javascript property of a specific sprite
void Model::prepareScript(Sprite& s){
    engine.globalObject().setProperty("x",QJSValue(s.x));
    engine.globalObject().setProperty("y",QJSValue(s.y));
    engine.globalObject().setProperty("vx",QJSValue(s.vx));
    engine.globalObject().setProperty("vy",QJSValue(s.vy));
    engine.globalObject().setProperty("mvx",QJSValue(s.mvx));
    engine.globalObject().setProperty("mvy",QJSValue(s.mvy));
    engine.globalObject().setProperty("ax",QJSValue(s.ax));
    engine.globalObject().setProperty("ay",QJSValue(s.ay));
    engine.globalObject().setProperty("model",QJSValue(s.model->name));
    engine.globalObject().setProperty("state",QJSValue(s.state->name));
    engine.globalObject().setProperty("frame",QJSValue(s.currentFrame()));
    engine.globalObject().setProperty("despawn",QJSValue(false));
}

//Recover the main javascript property of a specific sprite
bool Model::endScript(Sprite& s){
    if(engine.globalObject().property("x").isNumber())
        s.x = engine.globalObject().property("x").toNumber();
    if(engine.globalObject().property("y").isNumber())
        s.y = engine.globalObject().property("y").toNumber();
    if(engine.globalObject().property("vx").isNumber())
        s.vx = engine.globalObject().property("vx").toNumber();
    if(engine.globalObject().property("vy").isNumber())
        s.vy = engine.globalObject().property("vy").toNumber();
    if(engine.globalObject().property("mvx").isNumber())
        s.mvx = engine.globalObject().property("mvx").toNumber();
    if(engine.globalObject().property("mvy").isNumber())
        s.mvy = engine.globalObject().property("mvy").toNumber();
    if(engine.globalObject().property("ax").isNumber())
        s.ax = engine.globalObject().property("ax").toNumber();
    if(engine.globalObject().property("ay").isNumber())
        s.ay = engine.globalObject().property("ay").toNumber();
    if(engine.globalObject().property("state").isString()){
        QString name = engine.globalObject().property("state").toString();
        if(name.compare(s.state->name) != 0)
            for(unsigned j = 0; j < s.model->states.size(); ++j)
                if(name.compare(s.model->states[j]->name) == 0){
                    s.state = s.model->states[j].get();
                    s.frame = s.state->frames[0].get();
                    break;
                }
    }
    if(engine.globalObject().property("frame").isNumber()){
        unsigned f = engine.globalObject().property("frame").toUInt();
        if(f < s.state->frames.size())
            s.frame = s.state->frames[f].get();
    }
    if(engine.globalObject().property("despawn").isBool())
        return engine.globalObject().property("despawn").toBool();
    return false;
}

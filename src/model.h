#ifndef MODEL_H
#define MODEL_H

//@clean: use external tools
#include "sprite.h"
#include <QString>
#include <QCoreApplication>
#include <QJSEngine>
#include <QTextEdit>
#include <vector>

//Ressources path
#define ICON_PATH ":/ressources/ressources/icon.png"
#define TILESET_PATH ":/ressources/ressources/tileset.png"
#define SPRITESHEET_PATH ":/ressources/ressources/spritesheet.png"
//Game Data Files path
#define MAP_PATH "map.json"
#define SPRITE_MODELS_PATH "sprites.json"
#define TMP_MAP_PATH "map_tmp.json"
#define TMP_SPRITE_MODELS_PATH "sprites_tmp.json"
//Game mesurement constants
#define SCALE 3               //ratio
#define WIDTH 256             //pixels
#define HEIGHT 240            //pixels
#define TILE_SIZE 16          //pixels
#define MINIATURE_SIZE 128    //pixels
#define UPDATE_MARGIN 64      //pixels
#define SCREEN_WIDTH 16       //tiles
#define SCREEN_HEIGHT 15      //tiles
#define SCREEN_WIDTH_PLUS 17  //tiles
#define SCREEN_HEIGHT_PLUS 16 //tiles
#define MAP_WIDTH 256         //tiles
#define MAP_HEIGHT 64         //tiles
#define TILESET_WIDTH 19      //tiles
#define TILESET_HEIGHT 12     //tiles

class Model {
    /***********/
    /*Game Data*/
    /***********/
public:
    //tilemap data
    unsigned char map[MAP_WIDTH][MAP_HEIGHT];//could cause memory issues
    unsigned char tilesolid[TILESET_WIDTH*TILESET_HEIGHT];
    float sx = 0,sy = 0;
    int ox = 0,oy = 0;
    //sprite models (texture, behaviour)
    std::vector<std::unique_ptr<SpriteModel>> spriteModels;
    QString globalScript;
    //sprite instances (position, speed, animation)
    std::vector<Sprite> sprites;
    //scoreboard
    unsigned score = 0, coins = 0, world = 11; //(11 stands for "World 1-1")
    float time = 400;
    //game state data
    bool gamemode = false;

    /***********************/
    /*Serialization Methods*/
    /***********************/
public:
    void load(bool tmp = false);
    void save(bool tmp = false);
private:
    void loadMap(QString filename);
    void loadSpriteModels(QString filename);
    void saveMap(QString filename);
    void saveSpriteModels(QString filename);

    /*************/
    /*Game Engine*/
    /*************/
public:
    void start();
    void reset();
    void update(float dt);
    void scroll(float dx, float dy);
    QJSEngine engine;
private:
    bool eval(QString context, QString script);
    void prepareScript(float dt);
    void prepareScript(Sprite& s);
    void endScript();
    bool endScript(Sprite& s);
    bool collisions(Sprite* s, float dt);
    bool tileCollision(QRectF collider);

    /*************/
    /*Game Editor*/
    /*************/
public:
    //Drawing selection
    SpriteModel* spriteModel = NULL;
    int tileLeft=0;  //default: block tile
    int tileRight=43;//default: sky tile
    //Dialog flag
    bool focus = true;
    //Debug flag
    bool useGrid = false;
    //Javascript console
    QTextEdit* console;
};

#endif // MODEL_H

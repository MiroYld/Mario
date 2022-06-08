#include "model.h"
#include <QtWidgets>

//Load game data
void Model::load(bool temporary){
    if(temporary){
        loadMap(TMP_MAP_PATH);
    }else{
        loadSpriteModels(SPRITE_MODELS_PATH);
        loadMap(MAP_PATH);
    }
}

//Read a .json file into a QJsonObject
QJsonObject readJSON(QString filename){
    QString val;
    QFile file;
    file.setFileName(filename);
    while(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        //Ask and copy the files when the program is executed for the first time
        QFileDialog dialog;
        QString importFile = dialog.getOpenFileName(NULL,
            QObject::tr("Specify the path of ") + filename, "", QObject::tr("JSON file (*.json)"));
        if(importFile.isNull())
            exit(0);
        file.copy(importFile, filename);
    }
    val = file.readAll();
    file.close();
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    return d.object();
}

//Load sprite structs
void Model::loadSpriteModels(QString filename){
    QJsonObject data = readJSON(filename);
    globalScript = data["script"].toString();
    QJsonArray models = data["models"].toArray();
    spriteModels.clear();
    for(auto mIt : models){
        QJsonObject m = mIt.toObject();
        SpriteModel* model = new SpriteModel;
        model->name = m["name"].toString();
        model->script = m["script"].toString();
        model->scale = m["scale"].toDouble();
        QJsonArray states = m["states"].toArray();
        for(auto sIt : states){
            QJsonObject s = sIt.toObject();
            State* state = new State;
            state->name = s["name"].toString();
            state->script = s["script"].toString();
            state->speed = s["speed"].toInt();
            state->solid = s["solid"].toBool();
            QJsonArray frames = s["frames"].toArray();
            for(auto fIt : frames){
                QJsonObject f = fIt.toObject();
                Frame* frame = new Frame;
                QJsonObject image = f["image"].toObject();
                frame->image.setX(image["x"].toInt());
                frame->image.setY(image["y"].toInt());
                frame->image.setWidth(image["w"].toInt());
                frame->image.setHeight(image["h"].toInt());
                QJsonObject collider = f["collider"].toObject();
                frame->collider.setX(collider["x"].toInt());
                frame->collider.setY(collider["y"].toInt());
                frame->collider.setWidth(collider["w"].toInt());
                frame->collider.setHeight(collider["h"].toInt());
                QJsonObject origin = f["origin"].toObject();
                frame->origin.setX(origin["x"].toInt());
                frame->origin.setY(origin["y"].toInt());
                state->frames.push_back(std::unique_ptr<Frame>(frame));
            }
            model->states.push_back(std::unique_ptr<State>(state));
        }
        spriteModels.push_back(std::unique_ptr<SpriteModel>(model));
    }
}

//Load map
void Model::loadMap(QString filename){
    QJsonObject data = readJSON(filename);
    QJsonArray tilemap = data["tilemap"].toArray();
    for(int x = 0; x < MAP_WIDTH; ++x)
        for(int y = 0; y < MAP_HEIGHT; ++y)
            this->map[x][y] = tilemap[x].toArray()[y].toInt();
    QJsonArray tilesolid = data["tilesolid"].toArray();
    for(int i = 0; i < TILESET_WIDTH*TILESET_HEIGHT; ++i)
        this->tilesolid[i] = tilesolid[i].toInt();
    QJsonArray sprites = data["spritemap"].toArray();
    this->sprites.clear();
    for(auto sIt : sprites){
        QJsonObject s = sIt.toObject();
        Sprite sprite(spriteModels[s["model"].toInt()].get(),
                           s["x"].toInt(),s["y"].toInt());
        this->sprites.push_back(sprite);
    }
}

//Save game data
void Model::save(bool temporary){
    if(temporary){
        saveMap(TMP_MAP_PATH);
    }else{
        saveSpriteModels(SPRITE_MODELS_PATH);
        saveMap(MAP_PATH);
    }
}

//Write a QJsonObject into a .json file
void writeJSON(QString filename, QJsonObject data){
    QJsonDocument d;
    d.setObject(data);
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(d.toJson(QJsonDocument::Compact));
    file.close();
}

//Save sprite structs
void Model::saveSpriteModels(QString filename){
    QJsonArray models;
    for(unsigned i = 0; i < spriteModels.size(); ++i){
        SpriteModel* model = spriteModels[i].get();
        QJsonObject m;
        m["name"] = QJsonValue::fromVariant(model->name);
        m["script"] = QJsonValue::fromVariant(model->script);
        m["scale"] = QJsonValue::fromVariant(model->scale);
        QJsonArray states;
        for(unsigned j = 0; j < model->states.size(); ++j){
            State* state = model->states[j].get();
            QJsonObject s;
            s["name"] = QJsonValue::fromVariant(state->name);
            s["script"] = QJsonValue::fromVariant(state->script);
            s["speed"] = QJsonValue::fromVariant(state->speed);
            s["solid"] = QJsonValue::fromVariant(state->solid);
            QJsonArray frames;
            for(unsigned k = 0; k < state->frames.size(); ++k){
                Frame* frame = state->frames[k].get();
                QJsonObject f;
                QJsonObject image;
                image["x"] = QJsonValue::fromVariant(frame->image.x());
                image["y"] = QJsonValue::fromVariant(frame->image.y());
                image["w"] = QJsonValue::fromVariant(frame->image.width());
                image["h"] = QJsonValue::fromVariant(frame->image.height());
                f["image"] = image;
                QJsonObject collider;
                collider["x"] = QJsonValue::fromVariant(frame->collider.x());
                collider["y"] = QJsonValue::fromVariant(frame->collider.y());
                collider["w"] = QJsonValue::fromVariant(frame->collider.width());
                collider["h"] = QJsonValue::fromVariant(frame->collider.height());
                f["collider"] = collider;
                QJsonObject origin;
                origin["x"] = QJsonValue::fromVariant(frame->origin.x());
                origin["y"] = QJsonValue::fromVariant(frame->origin.y());
                f["origin"] = origin;
                frames << f;
            }
            s["frames"] = frames;
            states << s;
        }
        m["states"] = states;
        models << m;
    }
    QJsonObject data;
    data["models"] = models;
    data["script"] = QJsonValue::fromVariant(globalScript);
    writeJSON(filename, data);
}

//Save map
void Model::saveMap(QString filename){
    QJsonArray tilemap;
    for(int x = 0; x < MAP_WIDTH; ++x){
        QJsonArray sub;
        for(int y = 0; y < MAP_HEIGHT; ++y)
            sub << QJsonValue::fromVariant(this->map[x][y]);
        tilemap << sub;
    }
    QJsonArray tilesolid;
    for(int i = 0; i < TILESET_WIDTH*TILESET_HEIGHT; ++i)
        tilesolid << QJsonValue::fromVariant(this->tilesolid[i]);
    QJsonArray spritemap;
    for(auto sprite : this->sprites){
        QJsonObject s;
        //search the index of the model pointer in the vector<unique_ptr> spriteModels
        for(unsigned i = 0; i < this->spriteModels.size(); ++i)
            if(sprite.model == this->spriteModels[i].get())
                s["model"] = QJsonValue::fromVariant(i);
        s["x"] = QJsonValue::fromVariant(sprite.x);
        s["y"] = QJsonValue::fromVariant(sprite.y);
        spritemap << s;
    }
    QJsonObject data;
    data["tilemap"] = tilemap;
    data["tilesolid"] = tilesolid;
    data["spritemap"] = spritemap;
    writeJSON(filename, data);
}

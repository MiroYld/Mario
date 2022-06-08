#include "spritemodeleditor.h"

SpriteModelEditor::SpriteModelEditor(QWidget *parent, Model& m, QPixmap *spritesheet)
    : QWidget(parent), parent(parent), m(m), spritesheet(spritesheet){
    setMinimumSize(MINIATURE_SIZE,MINIATURE_SIZE);
    QVBoxLayout* vbox = new QVBoxLayout(this);

    //Sprite/State/Frame miniature
    image = new QLabel(this);
    image->setAlignment(Qt::AlignCenter);
    image->setPixmap(spritesheet->copy(default_img).scaled(MINIATURE_SIZE,MINIATURE_SIZE));
    image->setStyleSheet("QLabel{background-color: rgb(200,200,200);}");
    vbox->addWidget(image);
    connect(&timer,SIGNAL(timeout()),this,SLOT(updateAnimation()));

    //Information fields
    QGridLayout* grid = new QGridLayout();
    vbox->addLayout(grid);
    grid->addWidget(new QLabel(tr("Name:")),0,0);
    name = new QLineEdit(this);
    connect(name, &QLineEdit::cursorPositionChanged, [this]() {this->m.focus = false;});
    connect(name, &QLineEdit::editingFinished, [this]() {name->clearFocus();this->updateField();});
    connect(name, &QLineEdit::returnPressed, [this]() {name->clearFocus();this->updateField();});
    name->setEnabled(false);
    name->setFocusPolicy(Qt::ClickFocus);
    grid->addWidget(name,0,1);

    valueLabel = new QLabel(tr("Value :"));
    grid->addWidget(valueLabel,1,0);
    value = new QLineEdit(this);
    connect(value, &QLineEdit::cursorPositionChanged, [this]() {this->m.focus = false;});
    connect(value, &QLineEdit::editingFinished, [this]() {value->clearFocus();this->updateField();});
    connect(value, &QLineEdit::returnPressed, [this]() {value->clearFocus();this->updateField();});
    value->setEnabled(false);
    value->setFocusPolicy(Qt::ClickFocus);
    grid->addWidget(value,1,1);

    solid = new QCheckBox(tr("Solid"),this);
    connect(solid, &QCheckBox::toggled, [this](bool checked){this->getCurrentState()->solid = checked;});
    solid->setEnabled(false);
    solid->setFocusPolicy(Qt::NoFocus);
    grid->addWidget(solid,1,2);

    speedValidator = new QIntValidator(0, 999, this);
    //@todo: find a way to unvalidate "0"
    scaleValidator = new QRegExpValidator(QRegExp("^\\d(\\.\\d)?$", Qt::CaseInsensitive),this);

    //Script button
    scriptButton = new QPushButton("Script", this);
    scriptButton->setFocusPolicy(Qt::NoFocus);
    connect(scriptButton, SIGNAL(clicked()), this, SLOT(editScript()));
    grid->addWidget(scriptButton,0,2);

    //Tree controls
    QGridLayout* controls = new QGridLayout();
    vbox->addLayout(controls);
    QPushButton* addButton = new QPushButton("➕",this);
    addButton->setFocusPolicy(Qt::NoFocus);
    connect(addButton, SIGNAL(clicked()), this, SLOT(addElement()));
    controls->addWidget(addButton,0,0);

    QPushButton* upButton = new QPushButton("⯅",this);
    upButton->setFocusPolicy(Qt::NoFocus);
    connect(upButton, SIGNAL(clicked()), this, SLOT(moveUp()));
    controls->addWidget(upButton,0,1);

    QPushButton* downButton = new QPushButton("⯆",this);
    downButton->setFocusPolicy(Qt::NoFocus);
    connect(downButton, SIGNAL(clicked()), this, SLOT(moveDown()));
    controls->addWidget(downButton,0,2);

    QPushButton* deleteButton = new QPushButton("❌",this);
    deleteButton->setFocusPolicy(Qt::NoFocus);
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteElement()));
    controls->addWidget(deleteButton,0,3);

    //Tree of sprites
    tree = new DeselectableTreeWidget(this);
    //tree->setColumnCount(2);
    tree->setHeaderHidden(true);
    tree->setFocusPolicy(Qt::NoFocus);
    connect(tree, &DeselectableTreeWidget::currentItemChanged,this,&SpriteModelEditor::updateSelection);
    //tree->setRootIsDecorated(false);
    vbox->addWidget(tree);
    loadTree();

    //JS log console
    console = new QTextEdit(this);
    console->setReadOnly(true);
    console->setFocusPolicy(Qt::NoFocus);
    m.console = console;
    vbox->addWidget(console);
}

//Load the sprite tree from the structs defined in sprite.h
void SpriteModelEditor::loadTree(){
    for(unsigned i = 0; i < m.spriteModels.size(); ++i){
        SpriteModel* model = m.spriteModels[i].get();
        QTreeWidgetItem* mItem = new QTreeWidgetItem(tree);
        mItem->setText(0, model->name);
        for(unsigned j = 0; j < model->states.size(); ++j){
            State* state = model->states[j].get();
            QTreeWidgetItem* sItem = new QTreeWidgetItem(mItem);
            sItem->setText(0, state->name);
            for(unsigned k = 0; k < state->frames.size(); ++k){
                Frame* frame = state->frames[k].get();
                QTreeWidgetItem* fItem = new QTreeWidgetItem(sItem);
                QIcon icon(spritesheet->copy(frame->image));
                fItem->setText(0, QString::number(k));
                fItem->setIcon(0, icon);
            }
        }
        if(mItem->childCount() > 0 && mItem->child(0)->childCount() > 0)
            mItem->setIcon(0, mItem->child(0)->child(0)->icon(0));
    }
}

//Shortcut to get the current sprite selected in the tree
SpriteModel* SpriteModelEditor::getCurrentSpriteModel(){
    QTreeWidgetItem* item = tree->currentItem();
    if(item == NULL)
        return NULL;
    while(item->parent() != NULL)
        item = item->parent();
    int i = tree->indexOfTopLevelItem(item);
    return m.spriteModels.at(i).get();
}

//Shortcut to get the current state selected in the tree
State* SpriteModelEditor::getCurrentState(){
    QTreeWidgetItem* item = tree->currentItem();
    if(item == NULL || item->parent() == NULL)
        return NULL;
    while(item->parent()->parent() != NULL)
        item = item->parent();
    int i = tree->indexOfTopLevelItem(item->parent());
    int j = item->parent()->indexOfChild(item);
    return m.spriteModels[i]->states.at(j).get();
}

//Shortcut to get the current frame selected in the tree
Frame* SpriteModelEditor::getCurrentFrame(){
    QTreeWidgetItem* item = tree->currentItem();
    if(item == NULL || item->parent() == NULL || item->parent()->parent() == NULL)
        return NULL;
    while(item->parent()->parent()->parent() != NULL)
        item = item->parent();
    int i = tree->indexOfTopLevelItem(item->parent()->parent());
    int j = item->parent()->parent()->indexOfChild(item->parent());
    int k = item->parent()->indexOfChild(item);
    return m.spriteModels[i]->states[j]->frames.at(k).get();
}

//Update the sprite structs after modification in the information fields
void SpriteModelEditor::updateField(){
    SpriteModel* cModel = getCurrentSpriteModel();
    State* cState = getCurrentState();
    Frame* cFrame = getCurrentFrame();
    m.focus = true;
    if(cModel == NULL)
        return;
    if(cState == NULL){
        cModel->name = name->text();
        tree->currentItem()->setText(0,cModel->name);
        cModel->scale = value->text().toFloat();
    }else if(cFrame == NULL){
        cState->name = name->text();
        tree->currentItem()->setText(0,cState->name);
        cState->speed = value->text().toInt();
        if(cState->frames.size() > 0)
            timer.start(cState->speed);
    }
}

//Update the information fields and miniature if the user select another element in the tree
//@TODO: Add GUI elements to edit further more the sprites
void SpriteModelEditor::updateSelection(){
    SpriteModel* cModel = getCurrentSpriteModel();
    State* cState = getCurrentState();
    Frame* cFrame = getCurrentFrame();
    m.spriteModel = cModel;
    if(cModel == NULL){
        name->setEnabled(false);
        name->clear();
        value->setEnabled(false);
        value->clear();
        valueLabel->setText(tr("Value:"));
        solid->setEnabled(false);
        scriptButton->setEnabled(true);
        image->setPixmap(spritesheet->copy(default_img).scaled(MINIATURE_SIZE,MINIATURE_SIZE));
        timer.stop();
    }else if(cState == NULL){
        name->setEnabled(true);
        name->setText(cModel->name);
        value->setEnabled(true);
        value->setText(QString::number(cModel->scale));
        valueLabel->setText(tr("Scale:"));
        value->setValidator(scaleValidator);
        solid->setEnabled(false);
        scriptButton->setEnabled(true);
        if(cModel->states.size() > 0 && cModel->states[0]->frames.size() > 0)
            image->setPixmap(spritesheet->copy(cModel->states[0]->frames[0]->image).scaledToHeight(MINIATURE_SIZE));
        timer.stop();
    }else if(cFrame == NULL){
        name->setEnabled(true);
        name->setText(cState->name);
        value->setEnabled(true);
        value->setText(QString::number(cState->speed));
        valueLabel->setText(tr("Speed:"));
        value->setValidator(speedValidator);
        solid->setEnabled(true);
        solid->setChecked(cState->solid);
        scriptButton->setEnabled(true);
        if(cState->frames.size() > 0){
            animation.model = cModel;
            animation.state = cState;
            animation.frame = cState->frames[0].get();
            timer.start(cState->speed);
        }
    }else{
        name->setEnabled(false);
        name->clear();
        value->setEnabled(false);
        value->clear();
        valueLabel->setText(tr("Value:"));
        solid->setEnabled(true);
        scriptButton->setEnabled(false);
        image->setPixmap(spritesheet->copy(cFrame->image).scaledToHeight(MINIATURE_SIZE));
        timer.stop();
    }
    this->m.focus = true;
}

//Update the frame shown in the miniature animation preview
void SpriteModelEditor::updateAnimation(){
    unsigned i;
    for(i = 0; i < animation.state->frames.size(); ++i)
        if(animation.state->frames[i].get() == animation.frame)
            break;
    if(i == animation.state->frames.size()-1)
        i = 0;
    else ++i;
    animation.frame = animation.state->frames[i].get();
    update();
}

//Draw the new frame of the miniature animation preview
void SpriteModelEditor::paintEvent(QPaintEvent *){
    if(!timer.isActive() || animation.state->frames.size() == 0)
        return;
    int top = 1000, bottom = -1000, left = 1000, right = -1000;
    for(unsigned i = 0; i < animation.state->frames.size(); ++i){
        Frame* frame = animation.state->frames[i].get();
        top = fmin(frame->image.top()-frame->origin.y(),top);
        bottom = fmax(frame->image.bottom()-frame->origin.y(),bottom);
        left = fmin(frame->image.left()-frame->origin.x(),left);
        right = fmax(frame->image.right()-frame->origin.x(),right);
    }
    int size = fmax(bottom-top, right-left)+1;
    if(bottom-top > right-left)
        left -= abs((bottom-top) - (right-left))/2;
    else
        top -= abs((bottom-top) - (right-left))/2;
    QPixmap result(MINIATURE_SIZE,MINIATURE_SIZE);
    result.fill(QColor(200,200,200));
    QPainter painter(&result);
    painter.scale(1.*MINIATURE_SIZE/size,1.*MINIATURE_SIZE/size);
    painter.drawPixmap(-left+animation.frame->image.left()-animation.frame->origin.x(),
                       -top+animation.frame->image.top()-animation.frame->origin.y(),
                       animation.frame->image.width(),animation.frame->image.height(),
                       spritesheet->copy(animation.frame->image));
    image->setPixmap(result);
}

//Open the Script Editor
void SpriteModelEditor::editScript(){
    SpriteModel* cModel = getCurrentSpriteModel();
    State* cState = getCurrentState();
    Frame* cFrame = getCurrentFrame();
    if(!m.focus)
        return;
    if(cModel == NULL){
        m.focus = false;
        ScriptEditorDialog dialog("Global Script", m.globalScript, parent);
        if(dialog.exec() == 1)
            m.globalScript = dialog.result();
        m.focus = true;
    }else if(cState == NULL){
        m.focus = false;
        ScriptEditorDialog dialog(cModel->name, cModel->script, parent);
        if(dialog.exec() == 1)
            cModel->script = dialog.result();
        m.focus = true;
    }else if(cFrame == NULL){
        m.focus = false;
        ScriptEditorDialog dialog(cModel->name + " : " + cState->name, cState->script, parent);
        if(dialog.exec() == 1)
            cState->script = dialog.result();
        m.focus = true;
    }
}

//Control button pressed: ➕
void SpriteModelEditor::addElement(){
    SpriteModel* cModel = getCurrentSpriteModel();
    State* cState = getCurrentState();
    Frame* cFrame = getCurrentFrame();
    if(cModel == NULL){
        //Create new Sprite Model
        SpriteModel* model = new SpriteModel;
        m.spriteModels.push_back(std::unique_ptr<SpriteModel>(model));
        QTreeWidgetItem* item = new QTreeWidgetItem(tree);
        item->setText(0,model->name);
        tree->setCurrentItem(item);
    }else if(cState == NULL){
        //Create new State
        State* state = new State;
        if(cModel->states.size() == 0)
            state->name = "Idle";
        cModel->states.push_back(std::unique_ptr<State>(state));
        QTreeWidgetItem* item = new QTreeWidgetItem(tree->currentItem());
        item->setText(0,state->name);
        tree->setCurrentItem(item);
    }else if(cFrame == NULL){
        //Create new Frame
        if(!m.focus)
            return;
        m.focus = false;
        SpriteSelectDialog dialog(spritesheet, parent);
        int count = dialog.exec();
        if(count > 0){
            Frame frame = dialog.result();
            QIcon icon(spritesheet->copy(frame.image));
            for(int i = 0; i < count; ++i){
                cState->frames.push_back(std::unique_ptr<Frame>(new Frame(frame)));
                QTreeWidgetItem* item = new QTreeWidgetItem(tree->currentItem());
                item->setText(0,QString::number(cState->frames.size()));
                item->setIcon(0,icon);
            }
            if(cState == cModel->states[0].get()){
                tree->currentItem()->parent()->setIcon(0,icon);
            }
            tree->currentItem()->setExpanded(true);
        }
        m.focus = true;
    }else{
        //Modify existing Frame
        if(!m.focus)
            return;
        m.focus = false;
        SpriteSelectDialog dialog(spritesheet, parent, cFrame);
        if(dialog.exec() > 0){
            Frame frame = dialog.result();
            QIcon icon(spritesheet->copy(frame.image));
            cFrame->collider = frame.collider;
            cFrame->image = frame.image;
            cFrame->origin = frame.origin;
            tree->currentItem()->setIcon(0,icon);
            if(cState == cModel->states[0].get() && cFrame == cState->frames[0].get())
                tree->currentItem()->parent()->parent()->setIcon(0,icon);
        }
        m.focus = true;
    }
}


//Control button pressed: ⯅
void SpriteModelEditor::moveUp(){
    SpriteModel* cModel = getCurrentSpriteModel();
    State* cState = getCurrentState();
    Frame* cFrame = getCurrentFrame();
    unsigned i;
    if(cModel == NULL)
        return;
    bool expanded = tree->currentItem()->isExpanded();
    if(cState == NULL){
        if(cModel == m.spriteModels.front().get())
            return;
        for(i = 0; i < m.spriteModels.size(); ++i)
            if(cModel == m.spriteModels[i].get())
                break;
        std::swap(m.spriteModels[i], m.spriteModels[i-1]);
        tree->insertTopLevelItem(i-1, tree->takeTopLevelItem(i));
        tree->topLevelItem(i-1)->setExpanded(expanded);
        tree->setCurrentItem(tree->topLevelItem(i-1));
        return;
    }else if(cFrame == NULL){
        if(cState == cModel->states.front().get())
            return;
        for(i = 0; i < cModel->states.size(); ++i)
            if(cState == cModel->states[i].get())
                break;
        std::swap(cModel->states[i], cModel->states[i-1]);
    }else{
        if(cFrame == cState->frames.front().get())
            return;
        for(i = 0; i < cState->frames.size(); ++i)
            if(cFrame == cState->frames[i].get())
                break;
        std::swap(cState->frames[i], cState->frames[i-1]);
    }
    QTreeWidgetItem* parent = tree->currentItem()->parent();
    parent->insertChild(i-1, parent->takeChild(i));
    parent->child(i-1)->setExpanded(expanded);
    tree->setCurrentItem(parent->child(i-1));
}

//Control button pressed: ⯆
void SpriteModelEditor::moveDown(){
    SpriteModel* cModel = getCurrentSpriteModel();
    State* cState = getCurrentState();
    Frame* cFrame = getCurrentFrame();
    unsigned i;
    if(cModel == NULL)
        return;
    bool expanded = tree->currentItem()->isExpanded();
    if(cState == NULL){
        if(cModel == m.spriteModels.back().get())
            return;
        for(i = 0; i < m.spriteModels.size(); ++i)
            if(cModel == m.spriteModels[i].get())
                break;
        std::swap(m.spriteModels[i], m.spriteModels[i+1]);
        tree->insertTopLevelItem(i+1, tree->takeTopLevelItem(i));
        tree->topLevelItem(i+1)->setExpanded(expanded);
        tree->setCurrentItem(tree->topLevelItem(i+1));
        return;
    }else if(cFrame == NULL){
        if(cState == cModel->states.back().get())
            return;
        for(i = 0; i < cModel->states.size(); ++i)
            if(cState == cModel->states[i].get())
                break;
        std::swap(cModel->states[i], cModel->states[i+1]);
    }else{
        if(cFrame == cState->frames.back().get())
            return;
        for(i = 0; i < cState->frames.size(); ++i)
            if(cFrame == cState->frames[i].get())
                break;
        std::swap(cState->frames[i], cState->frames[i+1]);
    }
    QTreeWidgetItem* parent = tree->currentItem()->parent();
    parent->insertChild(i+1, parent->takeChild(i));
    parent->child(i+1)->setExpanded(expanded);
    tree->setCurrentItem(parent->child(i+1));
}

//Control button pressed: ❌
void SpriteModelEditor::deleteElement(){
    SpriteModel* cModel = getCurrentSpriteModel();
    State* cState = getCurrentState();
    Frame* cFrame = getCurrentFrame();
    unsigned i;
    if(cModel == NULL)
        return;
    delete tree->currentItem();
    if(cState == NULL){
        for(i = 0; i < m.spriteModels.size(); ++i)
            if(cModel == m.spriteModels[i].get())
                break;
        for(unsigned j = 0; j < m.sprites.size(); ++j)
            if(m.sprites[j].model == cModel)
                m.sprites.erase(m.sprites.begin()+j--);
        m.spriteModels.erase(m.spriteModels.begin()+i);
        return;
    }else if(cFrame == NULL){
        for(i = 0; i < cModel->states.size(); ++i)
            if(cState == cModel->states[i].get())
                break;
        for(unsigned j = 0; j < m.sprites.size(); ++j)
            if(m.sprites[j].model == cModel && m.sprites[j].state == cState){
                if(cModel->states.size() == 1)
                    m.sprites.erase(m.sprites.begin()+j--);
                else if(cModel->states[0].get() == cState){
                    m.sprites[j].state = cModel->states[1].get();
                    m.sprites[j].frame = m.sprites[j].state->frames[0].get();
                }else{
                    m.sprites[j].state = cModel->states[0].get();
                    m.sprites[j].frame = m.sprites[j].state->frames[0].get();
                }
            }
        cModel->states.erase(cModel->states.begin()+i);
    }else{
        for(i = 0; i < cState->frames.size(); ++i)
            if(cFrame == cState->frames[i].get())
                break;
        for(unsigned j = 0; j < m.sprites.size(); ++j)
            if(m.sprites[j].model == cModel && m.sprites[j].state == cState && m.sprites[j].frame == cFrame){
                if(cState->frames.size() == 1)
                    m.sprites.erase(m.sprites.begin()+j--);
                else if(cState->frames[0].get() == cFrame)
                    m.sprites[j].state = cModel->states[1].get();
                else
                    m.sprites[j].state = cModel->states[0].get();
            }
        cState->frames.erase(cState->frames.begin()+i);
    }
}

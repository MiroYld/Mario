#include "spriteselectdialog.h"

SpriteSelectDialog::SpriteSelectDialog(QPixmap* spritesheet, QWidget* parent)
                    :QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),spritesheet(spritesheet){
    QRect s = QApplication::desktop()->screenGeometry();
    setGeometry(s.width()/2-WIDTH*SCALE/2,s.height()/2-HEIGHT*SCALE/2,WIDTH*SCALE,HEIGHT*SCALE);
    setWindowTitle(tr("Frame Editor"));
    QHBoxLayout* hbox = new QHBoxLayout(this);
    setLayout(hbox);

    //Sprite select area
    area = new QScrollArea(this);
    hbox->addWidget(area);
    select = new SpriteSelect(area, spritesheet, SPRITE_DIALOG_SCALE);
    area->setWidget(select);

    //Control panel
    QVBoxLayout* vbox = new QVBoxLayout();
    hbox->addLayout(vbox);

    //Select actions radio buttons
    QRadioButton* radio1 = new QRadioButton(tr("Sprite"),this);
    connect(radio1, &QRadioButton::toggled, [this]() {this->select->type = SPRITE;});
    radio1->setChecked(true);
    vbox->addWidget(radio1);
    QRadioButton* radio2 = new QRadioButton(tr("Collider"),this);
    connect(radio2, &QRadioButton::toggled, [this]() {this->select->type = COLLIDER;});
    vbox->addWidget(radio2);
    QRadioButton* radio3 = new QRadioButton(tr("Origin"),this);
    connect(radio3, &QRadioButton::toggled, [this]() {this->select->type = ORIGIN;});
    vbox->addWidget(radio3);
    vbox->addWidget(new QLabel(tr("Count :"),this));
    QLineEdit* count = new QLineEdit("1",this);
    count->setMaximumWidth(80);
    count->setValidator(new QIntValidator(1,50));
    vbox->addWidget(count);

    //Controls
    QPushButton* confirm = new QPushButton(tr("Confirm"),this);
    connect(confirm,&QPushButton::clicked,[this,count]() {this->done(count->text().toInt());});
    vbox->addWidget(confirm);
    QPushButton* cancel = new QPushButton(tr("Cancel"),this);
    connect(cancel,&QPushButton::clicked,[this]() {this->done(0);});
    vbox->addWidget(cancel);
    vbox->addStretch(1);
}

//Alternative constructor to edit an existing frame
SpriteSelectDialog::SpriteSelectDialog(QPixmap* spritesheet, QWidget* parent, Frame* frame)
    :SpriteSelectDialog(spritesheet, parent){
    select->sprite = frame->image;
    select->collider = frame->collider;
    select->origin = frame->origin;
    area->horizontalScrollBar()->setValue(frame->image.center().x()*SPRITE_DIALOG_SCALE-SPRITE_SELECTION_MARGIN);
    area->verticalScrollBar()->setValue(frame->image.center().y()*SPRITE_DIALOG_SCALE-SPRITE_SELECTION_MARGIN);
}

Frame SpriteSelectDialog::result(){
    return {select->sprite, select->collider, select->origin};
}

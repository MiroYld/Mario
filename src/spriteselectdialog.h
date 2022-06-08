#ifndef SPRITESELECTDIALOG_H
#define SPRITESELECTDIALOG_H

#define SPRITE_DIALOG_SCALE 6
#define SPRITE_SELECTION_MARGIN 320

#include <QtWidgets>
#include "model.h"
#include "spriteselect.h"
#include "sprite.h"

class SpriteSelectDialog : public QDialog{
Q_OBJECT
public:
    explicit SpriteSelectDialog(QPixmap* spritesheet, QWidget* parent=0);
    SpriteSelectDialog(QPixmap* spritesheet, QWidget* parent, Frame* frame);
    Frame result();
private:
    QScrollArea* area;
    QPixmap* spritesheet;
    SpriteSelect* select;
};

#endif // SPRITESELECTDIALOG_H

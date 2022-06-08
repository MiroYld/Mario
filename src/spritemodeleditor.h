#ifndef SPRITEMAPEDITOR_H
#define SPRITEMAPEDITOR_H

#include "QWidget"
#include "QtWidgets"
#include "model.h"
#include "deselectabletreewidget.h"
#include "scripteditordialog.h"
#include "spriteselectdialog.h"
#include "cmath"

const QRect default_img(0,4,16,16);

class SpriteModelEditor : public QWidget{
    Q_OBJECT
public:
    explicit SpriteModelEditor(QWidget *parent, Model& m, QPixmap *spritesheet);
    SpriteModel* getCurrentSpriteModel();
    State* getCurrentState();
    Frame* getCurrentFrame();
    void updateField();
private:
    void loadTree();
    void paintEvent(QPaintEvent *event);
    QWidget* parent;
    Model& m;
    QPixmap *spritesheet;
    QValidator *speedValidator, *scaleValidator;
    QLabel *image, *valueLabel;
    QLineEdit *name, *value;
    QCheckBox *solid;
    QPushButton* scriptButton;
    DeselectableTreeWidget *tree;
    QTextEdit* console;
    QTimer timer;
    Sprite animation;
private slots:
    void updateSelection();
    void updateAnimation();
    void editScript();
    void addElement();
    void moveDown();
    void moveUp();
    void deleteElement();
};

#endif // SPRITEMAPEDITOR_H

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    qApp->installEventFilter(this);

    model.load();
    controller = new Controller(model);

    //Ressources
    tileset = new QPixmap(TILESET_PATH);
    spritesheet = new QPixmap(SPRITESHEET_PATH);
    setWindowIcon(QPixmap(ICON_PATH));

    //Tool Bar
    QToolBar* buttons = new QToolBar(this);
    buttons->setMovable(false);
    QPushButton* resetButton = new QPushButton(tr("Reset"),this);
    resetButton->setFocusPolicy(Qt::NoFocus);
    connect(resetButton, &QPushButton::clicked, this, [this](){model.reset();});
    buttons->addWidget(resetButton);
    QPushButton* startButton = new QPushButton(tr("Start"),this);
    startButton->setFocusPolicy(Qt::NoFocus);
    connect(startButton, &QPushButton::clicked, this, [this](){model.start();});
    buttons->addWidget(startButton);
    QPushButton* gridButton = new QPushButton(tr("Debug"),this);
    gridButton->setFocusPolicy(Qt::NoFocus);
    connect(gridButton, &QPushButton::clicked, [this]() {model.useGrid = !model.useGrid;tilemapEditor->update();});
    buttons->addWidget(gridButton);
    this->setMenuWidget(buttons);

    //Main Widget
    QWidget* main = new QWidget(this);
    hbox = new QHBoxLayout;
    hbox->setMargin(0);
    main->setLayout(hbox);
    setCentralWidget(main);

    //Editor Left
    editorLeft = new QTabWidget(this);
    editorLeft->setVisible(false);
    editorLeft->setFocusPolicy(Qt::NoFocus);
    hbox->addWidget(editorLeft);

    //View in the Middle
    QWidget* viewContainer =  new QWidget(this);
    QVBoxLayout* vbox = new QVBoxLayout(viewContainer);
    vbox->setMargin(0);
    view = new ViewGL(this, model, tileset, spritesheet);
    vbox->addWidget(view);
    vbox->setAlignment(view,Qt::AlignHCenter);
    viewContainer->setLayout(vbox);
    viewContainer->setMinimumHeight(HEIGHT*SCALE*3/4);
    viewContainer->setMaximumHeight(HEIGHT*SCALE);
    hbox->addWidget(viewContainer);
    hbox->setAlignment(viewContainer,Qt::AlignTop);

    //Editor Right
    editorRight = new QTabWidget(this);
    editorRight->setVisible(false);
    editorRight->setFocusPolicy(Qt::NoFocus);
    editorRight->setMinimumWidth(tileset->width());
    hbox->addWidget(editorRight);

    //Tabs for the Editors
    tilemapEditor = new TileMapEditor(this, model, tileset);
    editorRight->addTab(tilemapEditor,"Tilemap");
    spriteModelEditor = new SpriteModelEditor(this, model, spritesheet);
    editorRight->addTab(spriteModelEditor,"Spritemap");

    //Center the window
    QRect s = QApplication::desktop()->screenGeometry();
    setGeometry(s.width()/2-WIDTH*SCALE/2,s.height()/2-HEIGHT*SCALE/2,WIDTH*SCALE,HEIGHT*SCALE);

    //Game/Editor timer
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    //Solution to the random lags and rollbacks: speed up to 200 fps.
    //But Linux caps the framerate to 60 fps, so it's incompatible with Linux and maybe others OS.
    timer->start(5);// 1/0.005s = 200Hz
    time.start();
}

//Display 0,1 or 2 tabs if the window is large enough
void MainWindow::resizeEvent(QResizeEvent *event){
    tilemapEditor->update();
    if(isMaximized()){
        editorLeft->setVisible(false);
        editorRight->setVisible(false);
            editorRight->addTab(spriteModelEditor,"Spritemap");
    }else if(event->size().width() > WIDTH*SCALE + tileset->width() + MINIATURE_SIZE + 30){
        editorLeft->setVisible(true);
        editorRight->setVisible(true);
        if(editorLeft->count() == 0)
            editorLeft->addTab(spriteModelEditor,"Spritemap");
    }else if(event->size().width() > WIDTH*SCALE + tileset->width() + 12){
        editorLeft->setVisible(false);
        editorRight->setVisible(true);
        if(editorRight->count() == 1)
            editorRight->addTab(spriteModelEditor,"Spritemap");
    }else{
        editorLeft->setVisible(false);
        editorRight->setVisible(false);
    }
}

//Update the framerate display
inline void MainWindow::showFPS(){
    ++frameCount;
    if(time.elapsed() > 250){ //update fps every 1/4 sec
        framerate = frameCount * 1000. / time.elapsed();
        setWindowTitle(QString("Mario - ") + QString::number((int)framerate) + " fps");
        time.start();
        frameCount = 0;
    }
}

//Game/Editor timer update
void MainWindow::update(){
    showFPS();
    controller->update(1/framerate);
    view->update();
}

//Redirect events to the controller
bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    //redirect events from tilemap graphic scene
    if(!model.focus)
        return false;
    if (event->type() == QEvent::Close){
        quitDialog(); return true;
    }else if (event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Escape){
            if(model.gamemode)  model.reset();
            else                quitDialog();
            return true;
        }
        controller->keyPressEvent(keyEvent);
    }else if (event->type() == QEvent::KeyRelease){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        controller->keyReleaseEvent(keyEvent);
    }else if (obj != view){
        return QObject::eventFilter(obj, event);
    }else if (event->type() == QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        controller->mouseButtonPressEvent(mouseEvent);
    }else if (event->type() == QEvent::MouseButtonRelease){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        controller->mouseButtonReleaseEvent(mouseEvent);
    }else if (event->type() == QEvent::MouseMove){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        controller->mouseMoveEvent(mouseEvent);
    }
    return QObject::eventFilter(obj, event);
}

//Quit dialog to ask to save the game
void MainWindow::quitDialog(){
    if(!model.focus)
        return;
    model.focus = false;
    QMessageBox msgBox(this);
    msgBox.setText("Do you want to save ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    auto result = msgBox.exec();
    if(result == QMessageBox::Yes)
        model.save();
    if(result != QMessageBox::Cancel)
        QCoreApplication::exit();
    model.focus = true;
}

MainWindow::~MainWindow(){}

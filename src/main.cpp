#include "mainwindow.h"
#include <QApplication>

//Default main entry function in QT
int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include <QtWidgets>

//This code is from https://stackoverflow.com/questions/2443358/how-to-add-lines-numbers-to-qtextedit

class ScriptEditor : public QTextEdit{
    Q_OBJECT

public:
    ScriptEditor(QWidget *parent = 0);

    int getFirstVisibleBlockId();
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void keyReleaseEvent(QKeyEvent *event);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void resizeEvent(QResizeEvent *e);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(QRectF /*rect_f*/);
    void updateLineNumberArea(int /*slider_pos*/);
    void updateLineNumberArea();

private:
    QWidget *lineNumberArea;
    bool ctlKey = false;
};

#endif // SCRIPTEDITOR_H

#include "linenumberarea.h"
#include "scripteditor.h"

//This code is from https://stackoverflow.com/questions/2443358/how-to-add-lines-numbers-to-qtextedit

LineNumberArea::LineNumberArea(QTextEdit *editor) : QWidget(editor) {
    codeEditor = editor;
}

QSize LineNumberArea::sizeHint() const {
    return QSize(((ScriptEditor *)codeEditor)->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    ((ScriptEditor *)codeEditor)->lineNumberAreaPaintEvent(event);
}

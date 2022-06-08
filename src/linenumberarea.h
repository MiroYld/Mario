#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QtWidgets>

//This code is from https://stackoverflow.com/questions/2443358/how-to-add-lines-numbers-to-qtextedit

class LineNumberArea : public QWidget{
    Q_OBJECT

public:
    LineNumberArea(QTextEdit *editor);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    QTextEdit *codeEditor;
};

#endif // LINENUMBERAREA_H

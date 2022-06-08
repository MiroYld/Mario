#ifndef SCRIPTEDITORDIALOG_H
#define SCRIPTEDITORDIALOG_H

#include <QtWidgets>
#include "model.h"
#include "scripteditor.h"
#include "scripthighlighter.h"

#define FONT_FAMILLY "Consolas"
#define FONT_SIZE 11
#define TAB_WIDTH 4

class ScriptEditorDialog : public QDialog{
public:
    ScriptEditorDialog(QWidget* parent = 0);
    ScriptEditorDialog(QString title, QString& script, QWidget* parent = 0);
    QString result();
private:
    QTextEdit* textEdit;
    ScriptHighlighter* highlighter;
public slots:
    void save();
};

#endif // SCRIPTEDITORDIALOG_H

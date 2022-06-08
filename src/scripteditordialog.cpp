#include "scripteditordialog.h"

ScriptEditorDialog::ScriptEditorDialog(QWidget* parent):QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint){
    setWindowTitle(tr("Script Editor"));
    QRect s = QApplication::desktop()->screenGeometry();
    setGeometry(s.width()/4,s.height()/4,s.width()/2,s.height()/2);
    QVBoxLayout* vbox = new QVBoxLayout();
    setLayout(vbox);

    //Script area
    textEdit = new ScriptEditor(this);
    QFont font(FONT_FAMILLY,FONT_SIZE);
    textEdit->setFont(font);
    textEdit->setTabStopWidth(TAB_WIDTH*QFontMetrics(font).width(" "));
    textEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    highlighter = new ScriptHighlighter(textEdit->document());

    //Controls
    vbox->addWidget(textEdit);
    QHBoxLayout* hbox = new QHBoxLayout();
    vbox->addLayout(hbox);
    QPushButton* confirm = new QPushButton(tr("Confirm"),this);
    connect(confirm,&QPushButton::clicked,[this]() {this->done(1);});
    hbox->addWidget(confirm);
    QPushButton* cancel = new QPushButton(tr("Cancel"),this);
    connect(cancel,&QPushButton::clicked,[this]() {this->done(0);});
    hbox->addWidget(cancel);
}

//Alternative constructor to edit an existing script (actually I'm only using this one)
ScriptEditorDialog::ScriptEditorDialog(QString title, QString& script, QWidget* parent):ScriptEditorDialog(parent){
    setWindowTitle(tr("Script Editor - ") + title);
    textEdit->setText(script);
}

void ScriptEditorDialog::save(){
    this->done(1);
}

QString ScriptEditorDialog::result(){
    return textEdit->toPlainText();
}

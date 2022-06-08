#include "scripthighlighter.h"
#include <QtWidgets>

//This code is inspired by this example http://doc.qt.io/qt-5/qtwidgets-richtext-syntaxhighlighter-example.html

ScriptHighlighter::ScriptHighlighter(QTextDocument *parent):QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    EngineKeywordFormat.setForeground(Qt::darkRed);
    EngineKeywordFormat.setFontWeight(QFont::Bold);
    QStringList EngineKeywordPatterns;
    EngineKeywordPatterns << "\\bx\\b" << "\\by\\b" << "\\bvx\\b" << "\\bvy\\b"
                          << "\\bmvx\\b" << "\\bmvy\\b" << "\\bax\\b" << "\\bay\\b"
                          << "\\bsx\\b" << "\\bsy\\b" << "\\bmodel\\b"
                          << "\\bstate\\b" << "\\bframe\\b" << "\\bResetLevel\\b"
                          << "\\bcollisions(\\.((tile)|(left)|(right)|(top)|(bottom)|(sprite)))?\\b"
                          << "\\bdt\\b" << "\\blog\\b" << "\\bspawn\\b" << "\\bdespawn\\b"
                          << "\\bKeyUp\\b" << "\\bKeyDown\\b" << "\\bKeyLeft\\b"
                          << "\\bKeyRight\\b" << "\\bKeyJump\\b" << "\\bKeyRun\\b"
                          << "\\bscore\\b" << "\\bcoins\\b" << "\\bworld\\b" << "\\btime\\b";
    foreach (const QString &pattern, EngineKeywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = EngineKeywordFormat;
        highlightingRules.append(rule);
    }

    JSKeywordFormat.setForeground(Qt::blue);
    JSKeywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bbreak\\b" << "\\bcase\\b" << "\\bcatch\\b"
                    << "\\bcontinue\\b" << "\\bdebugger\\b" << "\\bdefault\\b"
                    << "\\bdelete\\b" << "\\bdo\\b" << "\\belse\\b"
                    << "\\bfinally\\b" << "\\bfor\\b" << "\\bfunction\\b"
                    << "\\bif\\b" << "\\bin\\b" << "\\binstanceof\\b"
                    << "\\bnew\\b" << "\\breturn\\b" << "\\bswitch\\b"
                    << "\\bthis\\b" << "\\bthrow\\b" << "\\btry\\b"
                    << "\\btypeof\\b" << "\\bvar\\b" << "\\bvoid\\b"
                    << "\\bwhile\\b" << "\\bwith\\b" << "\\bclass\\b"
                    << "\\bconst\\b" << "\\benum\\b" << "\\blength\\b"
                    << "\\btrue\\b" << "\\bfalse\\b" << "\\bundefined\\b"
                    << "\\bnull\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = JSKeywordFormat;
        highlightingRules.append(rule);
    }

    symbolFormat.setForeground(Qt::darkBlue);
    rule.pattern = QRegExp("[\\[\\]\\{\\}\\(\\)\\=\\>\\<\\+\\-\\&\\|\\!\\.\\;]");
    rule.format = symbolFormat;
    highlightingRules.append(rule);

    numberFormat.setFontWeight(QFont::Bold);
    numberFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegExp("\\b([0-9]+\\.?[0-9]*)|([0-9]*\\.?[0-9]+)\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    quotationFormat.setForeground(Qt::gray);
    rule.format = quotationFormat;
    //double quotes
    rule.pattern = QRegExp("(\"([^\"]|\"\")*\")");
    highlightingRules.append(rule);
    //single quotes
    rule.pattern = QRegExp("('([^']|'')*')");
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void ScriptHighlighter::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

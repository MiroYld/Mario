#ifndef SCRIPTHIGHLIGHTER_H
#define SCRIPTHIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

//This code is inspired by this example http://doc.qt.io/qt-5/qtwidgets-richtext-syntaxhighlighter-example.html

class ScriptHighlighter : public QSyntaxHighlighter{
    Q_OBJECT
public:
    ScriptHighlighter(QTextDocument *parent);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule{
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat JSKeywordFormat;
    QTextCharFormat EngineKeywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat symbolFormat;
};

#endif // SCRIPTHIGHLIGHTER_H

#include "mysyntaxhighlighter.h"

MySyntaxHighlighter::MySyntaxHighlighter(QTextDocument *parent):QSyntaxHighlighter(parent)
{
    HighlightRule rule;

    keywordFormat.setForeground(Qt::darkBlue); //设定关键词的高亮样式
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns;//关键词集合
    keywordPatterns << "\\bif\\b" << "\\bellse\\b" << "\\bwhile\\b"
                    << "\\bread\\b" << "\\bwrite\\b" << "\\bint\\b"
                    << "\\breal\\b" <<"\\belse\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    multiLineCommentFormat.setForeground(Qt::darkGreen); //多行注释，只设定了样式，具体匹配规则在highlightBlock中指定

    //多行注释符的匹配正则表达式
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void MySyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach(const HighlightRule &rule, highlightingRules){
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);
    //以下是多行注释的匹配

    int startIndex = 0;
    if(previousBlockState() !=1)
        startIndex = text.indexOf(commentStartExpression);
    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if(endIndex == -1){
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }else{
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

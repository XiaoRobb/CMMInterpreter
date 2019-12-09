#ifndef MYSYNTAXHIGHLIGHTER_H
#define MYSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class MySyntaxHighlighter:public QSyntaxHighlighter
{
    Q_OBJECT
public:
    MySyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule//语法规则结构题，包含正则表达式模式串和匹配样式
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightRule> highlightingRules;//规则的集合，可以定义多个高亮规则
    QRegularExpression commentStartExpression;//注释的高亮，使用highliangBlock()匹配
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;//关键词高亮样式
    QTextCharFormat multiLineCommentFormat;//多行注释高亮样式
};

#endif // MYSYNTAXHIGHLIGHTER_H

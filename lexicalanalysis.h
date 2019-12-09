#ifndef LEXICALANALYSIS_H
#define LEXICALANALYSIS_H

#include <QList>
#include "token.h"
class LexicalAnalysis
{
public:
    LexicalAnalysis();
    static QList<const Token*> * lexicalAnalysis(QString text);
    static void readChar();
    static int currentInt;
    static int lineNo;
    static QString currenChar;
    static QString text;
};

#endif // LEXICALANALYSIS_H

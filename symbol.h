#ifndef SYMBOL_H
#define SYMBOL_H

#include <QString>
#include "value.h"


class Symbol
{
public:
    Symbol(QString name, symbolType type, int level);
    Symbol(QString name, symbolType type, int level, int value);
    Symbol(QString name, symbolType type, int level, double value);
    ~Symbol();

    QString name;
    symbolType type;
    Value *value;
    int level;
    Symbol *next;
};

#endif // SYMBOL_H

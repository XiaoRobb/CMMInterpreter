#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <QString>
#include <QList>
#include <QVector>
#include "symbol.h"
#include "value.h"

class SymbolTable
{
public:
    ~SymbolTable();
    static SymbolTable * getSymbolTable(){return symbolTable;}
    void newTable();
    void deleteTable();
    void registSymbol(Symbol *symbol, int lineNo);
    void deregistSymbol(int level);
    void setSymbolValue(QString name, Value *value, int lineNo);
    void setSymbolValue(QString name, int value, int index, int lineNo);
    void setSymbolValue(QString name, double value, int index, int lineNo);
    symbolType getSymbolType(QString name, int lineNo);
    Value * getSymbolValue(QString name, int lineNo);
    Value * getSymbolValue(QString name, int index, int lineNo);
    Symbol * getSymbol(QString name, int lineNo);
    QString toString();

private:
    SymbolTable(){}
    static const QString TEMP_PREFICX;
    static SymbolTable * symbolTable;
    static QVector<Symbol*> *symbolList;

};

#endif // SYMBOLTABLE_H

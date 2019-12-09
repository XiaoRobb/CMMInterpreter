#include "symboltable.h"
#include "interpretexception.h"

const QString SymbolTable::TEMP_PREFICX = "*temp";
SymbolTable * SymbolTable::symbolTable = new SymbolTable();
QVector<Symbol*> *SymbolTable::symbolList = nullptr;
SymbolTable::~SymbolTable()
{
    deleteTable();
    if(symbolTable){
        delete symbolTable;
        symbolTable = nullptr;
    }
}

void SymbolTable::newTable()
{
    symbolList = new QVector<Symbol*>();
}

void SymbolTable::deleteTable()
{
    if(symbolList){
        for(int i=0; i< symbolList->length(); i++){
            delete symbolList->at(i);
            symbolList->removeAt(i);
        }
        symbolList = nullptr;
    }
}

void SymbolTable::registSymbol(Symbol *symbol, int lineNo)
{
    for(int i=0; i<symbolList->length(); i++){
        if(symbolList->at(i)->name == symbol->name){
            if(symbolList->at(i)->level < symbol->level){
                symbol->next = symbolList->at(i);
                symbolList->replace(i,symbol);
                return;
            }else{
                throw new InterpretException("Line " + QString::number(lineNo)+" : " + "变量<"+symbol->name+">重复申明");
            }
        }
    }
    symbolList->append(symbol);
}

void SymbolTable::deregistSymbol(int level)
{
    for(int i=0; i<symbolList->length(); i++){
        if(symbolList->at(i)->level == level){
            Symbol *temp = symbolList->at(i);
            symbolList->replace(i, symbolList->at(i)->next);
            delete temp;
            temp = nullptr;
        }
    }
    for(int i=symbolList->length()-1; i>=0; i--){
        if(symbolList->at(i) == nullptr){
            symbolList->removeAt(i);
        }
    }
}

void SymbolTable::setSymbolValue(QString name, Value *value, int lineNo)
{
    getSymbol(name, lineNo)->value = value;
}

void SymbolTable::setSymbolValue(QString name, int value, int index, int lineNo)
{
    if(getSymbol(name, lineNo)->value->dim > index)
        getSymbol(name, lineNo)->value->mArrayInt[index] = value;
    else
        throw new InterpretException("Line " + QString::number(lineNo)+" : " + "数组<"+name+">下标"+QString::number(index)+"越界");
}

void SymbolTable::setSymbolValue(QString name, double value, int index, int lineNo)
{
    if(getSymbol(name, lineNo)->value->dim > index)
        getSymbol(name, lineNo)->value->mArrayReal[index] = value;
    else
        throw new InterpretException("Line " + QString::number(lineNo)+" : " + "数组<"+name+">下标"+QString::number(index)+"越界");
}

symbolType SymbolTable::getSymbolType(QString name, int lineNo)
{
    return getSymbol(name, lineNo)->type;
}

Value *SymbolTable::getSymbolValue(QString name, int lineNo)
{
    return getSymbolValue(name, -1, lineNo);
}

//index == -1 代表单值，否则表示索引
Value *SymbolTable::getSymbolValue(QString name, int index, int lineNo)
{
    const Symbol *symbol = getSymbol(name, lineNo);
    if(index == -1)
        return symbol->value;
    else{
        if(symbol->value->dim < index + 1)
            throw new InterpretException("Line " + QString::number(lineNo)+" : " + "数组<"+name+">下标"+QString::number(index)+"越界");
        if(symbol->type == symbolType::ARRAY_INT){
            Value *rv = new Value(symbolType::SINGLE_INT);
            rv->mInt = symbol->value->mArrayInt[index];
            return rv;
        }else{
            Value *rv = new Value(symbolType::SINGLE_REAL);
            rv->mReal = symbol->value->mArrayReal[index];
            return rv;
        }
    }
}

Symbol *SymbolTable::getSymbol(QString name, int lineNo)
{
    for(int i=0; i<symbolList->length(); i++){
        Symbol *symbol = symbolList->at(i);
        if(symbol->name == name)
            return symbol;
    }
    throw new InterpretException("Line " + QString::number(lineNo)+" : " + "变量<"+name+">不存在");
}

QString SymbolTable::toString()
{
    QString temp = "";
    for(int i=0;i<symbolList->length();i++){
        Symbol *symbol = symbolList->at(i);
        if(symbol->type == symbolType::ARRAY_INT || symbol->type == symbolType::ARRAY_REAL)
            temp += symbol->name + " : " + symbol->value->toString() + "\n";
        else
            temp += symbol->name + " = " + symbol->value->toString() + "\n";
    }
    return temp;
}

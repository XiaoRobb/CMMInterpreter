#include "symbol.h"


Symbol::Symbol(QString name, symbolType type, int level)
{
    this->name = name;
    this->type = type;
    this->level = level;
    this->next = nullptr;
    this->value = new Value(type);
}

Symbol::Symbol(QString name, symbolType type, int level, int value)
{
    this->name = name;
    this->type = type;
    this->level = level;
    this->next = nullptr;
    this->value = new Value(type);
    this->value->mInt = value;
}

Symbol::Symbol(QString name, symbolType type, int level, double value)
{
    this->name = name;
    this->type = type;
    this->level = level;
    this->next = nullptr;
    this->value = new Value(type);
    this->value->mReal = value;
}

Symbol::~Symbol()
{
    if(value){
        delete value;
        value = nullptr;
    }
}


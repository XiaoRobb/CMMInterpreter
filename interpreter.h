#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "symboltable.h"
#include "treenode.h"
#include <QString>
#include "value.h"

class Interpreter
{
public:
    Interpreter();
    static void start(QString file);

private:
    static int mLevel;
    static void interpret(const TreeNode * node);
    static SymbolTable *symbolTable;
    static void interpretIfStmt(const TreeNode *node);
    static Value * interpretExp(const TreeNode *node);
    static Value * interpretLogicExp(const TreeNode *node);
    static Value * interpretAddictiveExp(const TreeNode *node);
    static Value * interpretTermExp(const TreeNode *node);
    static QString read();
    static void write(Value *value);



};

#endif // INTERPRETER_H

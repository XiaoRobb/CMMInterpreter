#ifndef DEBUGINTERPRETER_H
#define DEBUGINTERPRETER_H

#include "symboltable.h"
#include "treenode.h"
#include <QString>
#include "value.h"
#include <QList>
#include "code.h"
#include <QStack>

class DebugInterpreter
{
public:
    DebugInterpreter();
    ~DebugInterpreter();
    static void start(QString file);
    static SymbolTable *symbolTable;

private:
    static int mLevel;
    static int point;
    static QVector<TreeNode *> *nodes;
    static QStack<TreeNode *> *nodeBeforeBlock;//用于存放语句块前面的判断
    static QList<const Token* > *tokens;


    static Code interpret(TreeNode * node);
    static Value * interpretExp(TreeNode *node);
    static Value * interpretLogicExp(TreeNode *node);
    static Value * interpretAddictiveExp(TreeNode *node);
    static Value * interpretTermExp(TreeNode *node);
    static QString read();
    static void write(Value *value);
    static void waitForNext();
    static TreeNode *getNextNode();
    static void Free();



};

#endif // DEBUGINTERPRETER_H

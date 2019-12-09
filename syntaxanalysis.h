#ifndef SYNTAXANALYSIS_H
#define SYNTAXANALYSIS_H

#include <QList>
#include<QVector>
#include "treenode.h"
#include "token.h"

class SyntaxAnalysis
{
public:
    SyntaxAnalysis();
    static QVector<TreeNode*> * syntaxAnalysis(QList<const Token*> * tokenList);

private:
    static QVector<TreeNode*> * treeNodeList;
    static QList<const Token*> * tokenList;
    static const Token * currentToken;
    static int point;                            //指向tokenList的当前节点，便于回退

    static TreeNode * parseStmt();
    static TreeNode * parseIfStmt();
    static TreeNode * parseWhileStmt();
    static TreeNode * parseReadStmt();
    static TreeNode * parseWriteStmt();
    static TreeNode * parseDeclareStmt();
    static TreeNode * parseAssignStmt();
    static TreeNode * parseStmtBlock();
    static TreeNode * parseExp();
    static TreeNode * addtiveExp();
    static TreeNode * term();
    static TreeNode * factor();
    static TreeNode * litreal();
    static TreeNode * logicalOp();
    static TreeNode * addtiveOp();
    static TreeNode * multiplyOp();
    static TreeNode * variableName();
    static void consumeNextToken(tokenType type);
    static bool checkNextTokenType(tokenType type);
    static bool checkNextTokenType(tokenType types[],int len);
    static tokenType getNextTokenType();
    static int getLastTokenLineNo();
    static int getNextTokenLineNo();
    static QString getLastTokenName();

};

#endif // SYNTAXANALYSIS_H

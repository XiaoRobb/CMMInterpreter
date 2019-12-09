#include "debuginterpreter.h"
#include "lexicalanalysis.h"
#include "syntaxanalysis.h"
#include <QDebug>
#include "parserexception.h"
#include "interpretexception.h"
#include "mainwindow.h"
#include "code.h"

int DebugInterpreter::mLevel = 0;
int DebugInterpreter::point = 0;
SymbolTable * DebugInterpreter::symbolTable = nullptr;
QVector<TreeNode *> *DebugInterpreter::nodes = nullptr;
QList<const Token* > *DebugInterpreter::tokens = nullptr;;
QStack<TreeNode *> *DebugInterpreter::nodeBeforeBlock = nullptr;
DebugInterpreter::DebugInterpreter()
{

}

DebugInterpreter::~DebugInterpreter()
{
    Free();
}

void DebugInterpreter::start(QString file)
{
    symbolTable->deleteTable();
    Free();
    nodeBeforeBlock = new QStack<TreeNode *>();
    mLevel = 0;
    point = 0;
    //词法分析
    tokens = LexicalAnalysis::lexicalAnalysis(file);
    try {
        //语法分析
        nodes = SyntaxAnalysis::syntaxAnalysis(tokens);
        symbolTable = SymbolTable::getSymbolTable();
        symbolTable->newTable();
        //执行
        TreeNode *node = getNextNode();
        while(node){
            if(node->type != treeNodeType::BLOCK){
                if(MainWindow::isInBreak){
                    MainWindow::breakAtLineNo = node->lineNo;
                    MainWindow::waitForNext();
                }else{
                    for(int i=0;i<MainWindow::breakPoints->length();i++){
                        if(node->lineNo == MainWindow::breakPoints->at(i)){
                            MainWindow::breakAtLineNo = node->lineNo;
                            MainWindow::waitForNext();
                            break;
                         }
                    }
                }
            }
            Code nextCode = interpret(node);
            node = nextCode.nextNode;
        }

    } catch (ParserException *e) {
        qDebug()<<e->getMessage();
        MainWindow::writeResult(e->getMessage());
    } catch (InterpretException *e){
        qDebug()<<e->getMessage();
        MainWindow::writeResult(e->getMessage());
    }

}

Code DebugInterpreter::interpret(TreeNode *node)
{
    Code nextCode(nullptr);
    switch (node->type) {
    case treeNodeType::IF_STMT:
        if(interpretExp(node->mLeft)->mType == symbolType::TRUE){
            //nodeBeforeBlock->push(node);
            nextCode.nextNode = node->mMiddle;
            return nextCode;
        }else if(node->mRight){
            //nodeBeforeBlock->push(node);
            nextCode.nextNode = node->mRight;
            return nextCode;
        }
        break;
    case treeNodeType::WHILE_STMT:
        if(interpretExp(node->mLeft)->mType == symbolType::TRUE){
            nodeBeforeBlock->push(node);
            nextCode.nextNode = node->mMiddle;
            return nextCode;
        }
        break;
    case treeNodeType::BLOCK:
        mLevel++;
        if(node->mNext){
            nextCode.nextNode = node->mNext;
            return nextCode;
        }
        break;
    case treeNodeType::READ_STMT:
    {
        if(symbolTable->getSymbolType(node->mLeft->value, node->lineNo) == symbolType::SINGLE_INT){
            Value *value = new Value(symbolType::SINGLE_INT);
            value->mInt = read().toInt();
            symbolTable->setSymbolValue(node->mLeft->value, value, node->lineNo);
        }else if(symbolTable->getSymbolType(node->mLeft->value, node->lineNo) == symbolType::SINGLE_REAL){
            Value *value = new Value(symbolType::SINGLE_REAL);
            value->mReal = read().toDouble();
            symbolTable->setSymbolValue(node->mLeft->value, value, node->lineNo);
        }else if(symbolTable->getSymbolType(node->mLeft->value, node->lineNo) == symbolType::ARRAY_INT){
            int temp = read().toInt();
            symbolTable->setSymbolValue(node->mLeft->value, temp, interpretExp(node->mLeft->mLeft)->mInt, node->lineNo);
        }else if(symbolTable->getSymbolType(node->mLeft->value, node->lineNo) == symbolType::ARRAY_REAL){
            double temp = read().toDouble();
            symbolTable->setSymbolValue(node->mLeft->value, temp, interpretExp(node->mLeft->mLeft)->mInt, node->lineNo);
        }else{
            throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "输入与变量类型不匹配");
        }
        break;
    }
    case treeNodeType::WRITE_STMT:
        write(interpretExp(node->mLeft));
        break;
    case treeNodeType::DECLARE_STMT:
    {
        const TreeNode *var = node->mLeft;
        if(var->mLeft == nullptr){
            //单值
            Value *value = nullptr;
            if(node->mMiddle)
                value = interpretExp(node->mMiddle);
            if(var->mDataType == tokenType::INT){
                int intvalue = 0;
                if(value){
                    if(value->mType == symbolType::SINGLE_INT)
                        intvalue = value->mInt;
                    else
                        throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "表达式与变量类型不匹配");
                }
                Symbol *symbol = new Symbol(var->value, symbolType::SINGLE_INT, mLevel, intvalue);
                symbolTable->registSymbol(symbol, node->lineNo);
            }else if(var->mDataType == tokenType::REAL){
                double realvalue = 0;
                if(value){
                    if(value->mType == symbolType::SINGLE_REAL)
                        realvalue = value->mReal;
                    else
                        realvalue = value->toReal()->mReal;
                }
                Symbol *symbol = new Symbol(var->value, symbolType::SINGLE_REAL, mLevel, realvalue);
                symbolTable->registSymbol(symbol, node->lineNo);
            }
        }else{
            int len = interpretExp(var->mLeft)->mInt;
            Symbol *symbol = nullptr;
            if(var->mDataType == tokenType::INT)
                symbol = new Symbol(var->value, symbolType::ARRAY_INT,mLevel);
            else
                symbol = new Symbol(var->value, symbolType::ARRAY_REAL,mLevel);
            symbol->value->initArray(len);
            symbolTable->registSymbol(symbol, node->lineNo);
        }
        break;
    }
    case treeNodeType::ASSIGN_STMT:
    {
        Value *value = interpretExp(node->mMiddle);
        const TreeNode *var = node->mLeft;
        if(var->mLeft == nullptr){
            //单值
            if(symbolTable->getSymbolValue(var->value, node->lineNo)->mType == symbolType::SINGLE_REAL)
                symbolTable->setSymbolValue(var->value, value->toReal(), node->lineNo);
            else{
                if(value->mType == symbolType::SINGLE_REAL)
                    throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "表达式与变量类型不匹配");
                else
                    symbolTable->setSymbolValue(var->value, value, node->lineNo);
            }
        }else{
            int index  = interpretExp(var->mLeft)->mInt;
            if(symbolTable->getSymbolValue(var->value, index, node->lineNo)->mType == symbolType::SINGLE_REAL)
                symbolTable->setSymbolValue(var->value, value->toReal()->mReal, index, node->lineNo);
            else{
                if(value->mType == symbolType::SINGLE_REAL)
                    throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "表达式与变量类型不匹配");
                else
                    symbolTable->setSymbolValue(var->value, value->mInt, index, node->lineNo);
            }
        }
        break;
    }
    default:
        break;
    }
    if(node->mNext){
        nextCode.nextNode = node->mNext;
        return nextCode;
    }
    if(nodeBeforeBlock->isEmpty())
        nextCode.nextNode = getNextNode();
    else{
        TreeNode * temp = nodeBeforeBlock->pop();
        if(temp->type == treeNodeType::IF_STMT){
            nextCode.nextNode = getNextNode();
        }else if(temp->type == treeNodeType::WHILE_STMT){
            nextCode.nextNode = temp;
        }
        symbolTable->deregistSymbol(mLevel);
        mLevel--;
    }
    return nextCode;
}


Value * DebugInterpreter::interpretExp(TreeNode *node)
{
//    if(node->type == treeNodeType::EXP){
//        switch (node->mDataType) {
//        case tokenType::LOGIC_EXP:
//            return interpretLogicExp(node);
//        case tokenType::ADDTIVE_EXP:
//            return interpretAddictiveExp(node);
//        case tokenType::TERM_EXP:
//            return interpretTermExp(node);
//        default:
//            throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "复合表达式非法");
//        }
//    }else if(node->type == treeNodeType::FACTOR){
//        if(node->mDataType == tokenType::MINUS)
//            return Value::NOT(interpretExp(node->mLeft), node->lineNo);
//        else
//            return interpretExp(node->mLeft);
//    }else if(node->type == treeNodeType::VAR){
//        if(node->mLeft == nullptr){
//            //单值
//            symbolType type =  symbolTable->getSymbolType(node->value, node->lineNo);
//            if(type == symbolType::ARRAY_INT || type == symbolType::ARRAY_REAL)
//                throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "数组名不能参与运算或write");
//            return symbolTable->getSymbolValue(node->value, node->lineNo);
//        }else{
//            Value *index = interpretExp(node->mLeft);
//            if(index->mType != symbolType::SINGLE_INT)
//                throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "数组下标不是整数");
//            return symbolTable->getSymbolValue(node->value, index->mInt);
//        }
//    }else if(node->type == treeNodeType::LITREAL){
//        if(node->mDataType == tokenType::LITERAL_INT){
//            Value *vr = new Value(symbolType::SINGLE_INT);
//            vr->mInt = node->value.toInt();
//            return vr;
//        }else{
//            Value *vr = new Value(symbolType::SINGLE_REAL);
//            vr->mReal = node->value.toDouble();
//            return vr;
//        }
//    }
//    throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "表达式非法");
    if(node->type == treeNodeType::EXP){
        switch (node->mDataType) {
        case tokenType::LOGIC_EXP:
            return interpretLogicExp(node);
        case tokenType::ADDTIVE_EXP:
            return interpretAddictiveExp(node);
        case tokenType::TERM_EXP:
            return interpretTermExp(node);
        default:
            throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "复合表达式非法");
        }
    }else if(node->type == treeNodeType::FACTOR){
        if(node->mDataType == tokenType::MINUS)
            return Value::NOT(interpretExp(node->mLeft), node->lineNo);
        else
            return interpretExp(node->mLeft);
    }else if(node->type == treeNodeType::VAR){
        if(node->mLeft == nullptr){
            //单值
            symbolType type =  symbolTable->getSymbolType(node->value, node->lineNo);
            if(type == symbolType::ARRAY_INT || type == symbolType::ARRAY_REAL)
                throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "数组名不能参与运算或write");
            return symbolTable->getSymbolValue(node->value, node->lineNo);
        }else{
            Value *index = interpretExp(node->mLeft);
            if(index->mType != symbolType::SINGLE_INT)
                throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "数组下标不是整数");
            return symbolTable->getSymbolValue(node->value, index->mInt, node->lineNo);
        }
    }else if(node->type == treeNodeType::LITREAL){
        if(node->mDataType == tokenType::LITERAL_INT){
            Value *vr = new Value(symbolType::SINGLE_INT);
            vr->mInt = node->value.toInt();
            return vr;
        }else{
            Value *vr = new Value(symbolType::SINGLE_REAL);
            vr->mReal = node->value.toDouble();
            return vr;
        }
    }
    throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "表达式非法");
}

Value * DebugInterpreter::interpretLogicExp(TreeNode *node)
{
    switch (node->mMiddle->mDataType) {
    case tokenType::GT:
        return interpretExp(node->mLeft)->GT(interpretExp(node->mRight), node->lineNo);
    case tokenType::GET:
        return interpretExp(node->mLeft)->GET(interpretExp(node->mRight), node->lineNo);
    case tokenType::LT:
        return interpretExp(node->mLeft)->LT(interpretExp(node->mRight), node->lineNo);
    case tokenType::LET:
        return interpretExp(node->mLeft)->LET(interpretExp(node->mRight), node->lineNo);
    case tokenType::EQ:
        return interpretExp(node->mLeft)->EQ(interpretExp(node->mRight), node->lineNo);
    case tokenType::NEQ:
        return interpretExp(node->mLeft)->NEQ(interpretExp(node->mRight), node->lineNo);
    default:
        throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "逻辑运算非法");
    }
}

Value *DebugInterpreter::interpretAddictiveExp(TreeNode *node)
{
    switch (node->mMiddle->mDataType) {
    case tokenType::PLUS:
        return interpretExp(node->mLeft)->PLUS(interpretExp(node->mRight), node->lineNo);
    case tokenType::MINUS:
        return interpretExp(node->mLeft)->MINUS(interpretExp(node->mRight), node->lineNo);
    default:
        throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "算术运算非法");
    }
}

Value * DebugInterpreter::interpretTermExp(TreeNode *node)
{
    if(node->mRight->type == treeNodeType::FACTOR || node->mRight->type == treeNodeType::VAR){
            switch (node->mMiddle->mDataType) {
            case tokenType::MUL:
                return interpretExp(node->mLeft)->MUL(interpretExp(node->mRight), node->lineNo);
            case tokenType::DIV:
                return interpretExp(node->mLeft)->DIV(interpretExp(node->mRight), node->lineNo);
            default:
                throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "算术运算非法");
            }
    }else{
        Value * temp = nullptr;
        switch (node->mMiddle->mDataType) {
        case tokenType::MUL:
            temp = interpretExp(node->mLeft)->MUL(interpretExp(node->mRight->mLeft), node->lineNo);
            break;
        case tokenType::DIV:
            temp = interpretExp(node->mLeft)->DIV(interpretExp(node->mRight->mLeft), node->lineNo);
            break;
        default:
            throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "算术运算非法");
        }
        node = node->mRight;
        while (node->mRight != nullptr && (node->mRight->type!= treeNodeType::FACTOR && node->mRight->type!= treeNodeType::VAR)) {
            switch (node->mMiddle->mDataType) {
            case tokenType::MUL:
                temp = temp->MUL(interpretExp(node->mRight->mLeft), node->lineNo);
                break;
            case tokenType::DIV:
                temp = temp->DIV(interpretExp(node->mRight->mLeft), node->lineNo);
                break;
            default:
                throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "算术运算非法");
            }
            node = node->mRight;
        }
        if(node->mRight != nullptr && (node->mRight->type == treeNodeType::FACTOR || node->mRight->type == treeNodeType::VAR)){
            switch (node->mMiddle->mDataType) {
            case tokenType::MUL:
                return temp->MUL(interpretExp(node->mRight), node->lineNo);
            case tokenType::DIV:
                return temp->DIV(interpretExp(node->mRight), node->lineNo);
            default:
                throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "算术运算非法");
            }
        }
        return temp;
    }
}

QString DebugInterpreter::read()
{
    return MainWindow::readInput();
}

void DebugInterpreter::write(Value *value)
{
    if(value->mType == symbolType::SINGLE_INT){
        qDebug()<<value->mInt;
        MainWindow::writeResult(QString::number(value->mInt));
    }else if(value->mType == symbolType::SINGLE_REAL){
        qDebug()<<value->mReal;
        MainWindow::writeResult(QString::number(value->mReal));
    }else if(value->mType == symbolType::ARRAY_INT){
        qDebug()<<value->mArrayInt[value->mInt];
        MainWindow::writeResult(QString::number(value->mArrayInt[value->mInt]));
    }else if(value->mType == symbolType::ARRAY_REAL){
        qDebug()<<value->mArrayReal[value->mInt];
        MainWindow::writeResult(QString::number(value->mArrayReal[value->mInt]));
    }
}

TreeNode *DebugInterpreter::getNextNode()
{
    if(point >= nodes->length())
        return nullptr;
    TreeNode * temp = nodes->at(point);
    point++;
    return temp;
}

void DebugInterpreter::Free()
{
    if(nodes){
        for(int i=0;i<nodes->length();i++){
            delete nodes->at(i);
        }
        nodes->clear();
        delete nodes;
        nodes = nullptr;
    }
    if(tokens){
        for(int i=0;i<tokens->length();i++){
            delete tokens->at(i);
        }
        tokens->clear();
        delete tokens;
        tokens = nullptr;
    }
    if(nodeBeforeBlock){
        int len = nodeBeforeBlock->length();
        for(int i=0;i<len;i++){
            TreeNode * node = nodeBeforeBlock->pop();
            delete node;
        }
        delete nodeBeforeBlock;
        nodeBeforeBlock = nullptr;
    }
}

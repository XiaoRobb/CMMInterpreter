#include "interpreter.h"
#include "lexicalanalysis.h"
#include "syntaxanalysis.h"
#include <QDebug>
#include "parserexception.h"
#include "interpretexception.h"
#include "mainwindow.h"
int Interpreter::mLevel = 0;
SymbolTable * Interpreter::symbolTable = nullptr;
Interpreter::Interpreter()
{

}

void Interpreter::start(QString file)
{
    mLevel = 0;
    symbolTable->deleteTable();
    QList<const Token* > * tokens = LexicalAnalysis::lexicalAnalysis(file);
    try {
        QVector<TreeNode *> *nodes = SyntaxAnalysis::syntaxAnalysis(tokens);
        symbolTable = SymbolTable::getSymbolTable();
        symbolTable->newTable();
        for(int i=0; i< nodes->length(); i++){
            interpret(nodes->at(i));
        }
        symbolTable->deleteTable();
    } catch (ParserException *e) {
        qDebug()<<e->getMessage();
        MainWindow::writeResult(e->getMessage());
    } catch (InterpretException *e){
        qDebug()<<e->getMessage();
        MainWindow::writeResult(e->getMessage());
    }
}

void Interpreter::interpret(const TreeNode *node)
{
    while(true){
        switch (node->type) {
        case treeNodeType::IF_STMT:
            interpretIfStmt(node);
            break;
        case treeNodeType::WHILE_STMT:
            while(interpretExp(node->mLeft)->mType == symbolType::TRUE){
                mLevel++;
                interpret(node->mMiddle);
                symbolTable->deregistSymbol(mLevel);
                mLevel--;
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
//                        else
//                            throw new InterpretException("Line " + QString::number(node->lineNo)+" : " + "表达式与变量类型不匹配");
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
        if(node->mNext)
            node = node->mNext;
        else
            break;
    }
}

void Interpreter::interpretIfStmt(const TreeNode *node)
{
    if(node->type == treeNodeType::IF_STMT){
        if(interpretExp(node->mLeft)->mType == symbolType::TRUE){
            mLevel++;
            interpret(node->mMiddle);
            symbolTable->deregistSymbol(mLevel);
            mLevel--;
        }else if(node->mRight){
            mLevel++;
            interpret(node->mRight);
            symbolTable->deregistSymbol(mLevel);
            mLevel--;
        }
    }
}

Value * Interpreter::interpretExp(const TreeNode *node)
{
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

Value * Interpreter::interpretLogicExp(const TreeNode *node)
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

Value *Interpreter::interpretAddictiveExp(const TreeNode *node)
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

Value * Interpreter::interpretTermExp(const TreeNode *node)
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

QString Interpreter::read()
{
    return MainWindow::readInput();
}

void Interpreter::write(Value *value)
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

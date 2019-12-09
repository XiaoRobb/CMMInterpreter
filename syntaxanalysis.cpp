#include "syntaxanalysis.h"
#include "parserexception.h"

QVector<TreeNode*> * SyntaxAnalysis::treeNodeList = nullptr;
QList<const Token*> * SyntaxAnalysis::tokenList = nullptr;
const Token * SyntaxAnalysis::currentToken = nullptr;
int SyntaxAnalysis::point = 0;
SyntaxAnalysis::SyntaxAnalysis()
{

}

QVector<TreeNode *> *SyntaxAnalysis::syntaxAnalysis(QList<const Token*> *tokenList)
{
    point = 0;
    treeNodeList =  new QVector<TreeNode*>();
    SyntaxAnalysis::tokenList = tokenList;
    while (point < tokenList->length()) {
        treeNodeList->append(parseStmt());
    }
    return treeNodeList;
}


//根据下一个TokenType分配处理方法
TreeNode *SyntaxAnalysis::parseStmt()
{
    switch (getNextTokenType()) {
    case tokenType::IF: return parseIfStmt();
    case tokenType::WHILE: return parseWhileStmt();
    case tokenType::READ: return parseReadStmt();
    case tokenType::WRITE: return parseWriteStmt();
    case tokenType::INT:
    case tokenType::REAL: return parseDeclareStmt();
    case tokenType::LBRACE: return parseStmtBlock();
    case tokenType::ID: return parseAssignStmt();
    default:
        throw new ParserException(QString("lien ")+QString::number(getLastTokenLineNo())+QString(" : unexpected token"));
    }
}

//if语句
TreeNode *SyntaxAnalysis::parseIfStmt()
{
    TreeNode *node = new TreeNode(treeNodeType::IF_STMT, getNextTokenLineNo());
    consumeNextToken(tokenType::IF);
    consumeNextToken(tokenType::LPARENT);
    node->mLeft = parseExp();
    consumeNextToken(tokenType::RPARENT);
    node->mMiddle = parseStmt();
    if(getNextTokenType() == tokenType::ELSE){
        consumeNextToken(tokenType::ELSE);
        node->mRight = parseStmt();
    }
    return node;
}

//while语句
TreeNode *SyntaxAnalysis::parseWhileStmt()
{
    TreeNode *node = new TreeNode(treeNodeType::WHILE_STMT, getNextTokenLineNo());
    consumeNextToken(tokenType::WHILE);
    consumeNextToken(tokenType::LPARENT);
    node->mLeft = parseExp();
    consumeNextToken(tokenType::RPARENT);
    node->mMiddle = parseStmt();
    return node;
}

//读语句
TreeNode *SyntaxAnalysis::parseReadStmt()
{
    TreeNode *node = new TreeNode(treeNodeType::READ_STMT, getNextTokenLineNo());
    consumeNextToken(tokenType::READ);
    node->mLeft = variableName();
    consumeNextToken(tokenType::SEMI);
    return node;
}

//write语句
TreeNode *SyntaxAnalysis::parseWriteStmt()
{
    TreeNode *node = new TreeNode(treeNodeType::WRITE_STMT, getNextTokenLineNo());
    consumeNextToken(tokenType::WRITE);
    node->mLeft = parseExp();
    consumeNextToken(tokenType::SEMI);
    return node;
}

//声明语句
TreeNode *SyntaxAnalysis::parseDeclareStmt()
{
    TreeNode * node = new TreeNode(treeNodeType::DECLARE_STMT, getNextTokenLineNo());
    TreeNode * varNode = new TreeNode(treeNodeType::VAR, getNextTokenLineNo());
    tokenType types[2] = {tokenType::INT, tokenType::REAL};
    int len = sizeof (types)/sizeof(int);
    if(checkNextTokenType(types, len)){
        currentToken = tokenList->at(point);
        point++;
        tokenType type = currentToken->type;
        if(type == tokenType::INT){
            varNode->mDataType = tokenType::INT;
        }else{
            varNode->mDataType = tokenType::REAL;
        }
    }else{
        delete node;
        delete varNode;
        node = nullptr;
        varNode = nullptr;
        throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> variable type"));
    }
    if(checkNextTokenType(tokenType::ID)){
        currentToken = tokenList->at(point);
        point++;
        varNode->value = currentToken->value;
    }else{
        delete node;
        delete varNode;
        node = nullptr;
        varNode = nullptr;
        throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> ID"));
    }
    if(getNextTokenType() == tokenType::ASSIGN){
        consumeNextToken(tokenType::ASSIGN);
        node->mMiddle = parseExp();
    }else if(getNextTokenType() == tokenType::LBRACKET){
        consumeNextToken(tokenType::LBRACKET);
        varNode->mLeft = parseExp();
        consumeNextToken(tokenType::RBRACKET);
    }
    consumeNextToken(tokenType::SEMI);
    node->mLeft = varNode;
    return node;

}

//赋值语句
TreeNode *SyntaxAnalysis::parseAssignStmt()
{
    TreeNode *node = new TreeNode(treeNodeType::ASSIGN_STMT, getNextTokenLineNo());
    node->mLeft = variableName();
    consumeNextToken(tokenType::ASSIGN);
    node->mMiddle = parseExp();
    consumeNextToken(tokenType::SEMI);
    return node;
}

//语句块
TreeNode *SyntaxAnalysis::parseStmtBlock()
{
    TreeNode *node = new TreeNode(treeNodeType::BLOCK, getNextTokenLineNo());
    TreeNode *header = node;
    TreeNode *temp = nullptr;
    consumeNextToken(tokenType::LBRACE);
    while(getNextTokenType() != tokenType::RBRACE){
        if(getNextTokenType() == tokenType::NULLTOKEN)
            break;
        //允许语句块中没有语句
        temp = parseStmt();
        node->mNext = temp;
        node = temp;
    }
    consumeNextToken(tokenType::RBRACE);
    return header;
}

//表达式
TreeNode *SyntaxAnalysis::parseExp()
{
    TreeNode * node = new TreeNode(treeNodeType::EXP, getNextTokenLineNo());
    node->mDataType = tokenType::LOGIC_EXP;
    TreeNode * leftNode = addtiveExp();
    tokenType types[6] = {tokenType::EQ, tokenType::NEQ, tokenType::GT,
                         tokenType::GET, tokenType::LT, tokenType::LET};
    int len = sizeof (types)/sizeof(int);
    if(checkNextTokenType(types, len)){
        node->mLeft = leftNode;
        node->mMiddle = logicalOp();
        try {
            node->mRight = addtiveExp();
        } catch (ParserException *e) {
            delete e;
            e = nullptr;
            throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> addtiveExp"));
        }

    }else{
        return leftNode;
    }
    return node;
}

//多项式
TreeNode *SyntaxAnalysis::addtiveExp()
{
    TreeNode *node = new TreeNode(treeNodeType::EXP, getNextTokenLineNo());
    node->mDataType = tokenType::ADDTIVE_EXP;
    TreeNode *leftNode = term();
    if(checkNextTokenType(tokenType::PLUS)){
        node->mLeft = leftNode;
        node->mMiddle = addtiveOp();
        node->mRight = addtiveExp();
    }else if(checkNextTokenType(tokenType::MINUS)){
        node->mLeft = leftNode;
        TreeNode * opNode = new TreeNode(treeNodeType::OP);
        opNode->mDataType = tokenType::PLUS;
        node->mMiddle = opNode;
        node->mRight = addtiveExp();
    }else{
        delete node;
        node = nullptr;
        return leftNode;
    }
    return node;
}


//项
TreeNode *SyntaxAnalysis::term()
{
    TreeNode * node = new TreeNode(treeNodeType::EXP, getNextTokenLineNo());
    node->mDataType = tokenType::TERM_EXP;
    TreeNode * leftNode = factor();
    tokenType types[2] = {tokenType::MUL, tokenType::DIV};
    int len = sizeof (types)/sizeof(int);
    if(checkNextTokenType(types, len)){
        node->mLeft = leftNode;
        node->mMiddle = multiplyOp();
        node->mRight = term();
    }else{
        delete node;
        node = nullptr;
        return  leftNode;
    }
    return node;
}

//因子
TreeNode *SyntaxAnalysis::factor()
{
    if(point < tokenList->length()){
        TreeNode * expNode = new TreeNode(treeNodeType::FACTOR, getNextTokenLineNo());
        switch (getNextTokenType()) {
        case tokenType::LITERAL_INT:
        case tokenType::LITERAL_REAL:
                expNode->mLeft = litreal();
                return expNode;
        case tokenType::LPARENT:{
                consumeNextToken(tokenType::LPARENT);
                TreeNode * temp = parseExp();
                consumeNextToken(tokenType::RPARENT);
                delete expNode;
                expNode = nullptr;
                return temp;
        }
        case tokenType::MINUS:
                expNode->mDataType = tokenType::MINUS;
                currentToken = tokenList->at(point);
                point++;
                expNode->mLeft = term();
                return expNode;
        case tokenType::PLUS:
                currentToken = tokenList->at(point);
                point++;
                expNode->mLeft = term();
                return expNode;
         default:
                return variableName();
        }
    }
    throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> factor"));
}

TreeNode *SyntaxAnalysis::litreal()
{
    if(point < tokenList->length()){
        currentToken = tokenList->at(point);
        point++;
        tokenType type = currentToken->type;
        TreeNode * node = new TreeNode(treeNodeType::LITREAL, getNextTokenLineNo());
        node->mDataType = type;
        node->value = currentToken->value;
        if(type == tokenType::LITERAL_INT || type == tokenType::LITERAL_REAL){
            return node;
        }
    }
    throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> litreal value"));
}

//逻辑运算符
TreeNode *SyntaxAnalysis::logicalOp()
{
    if(point < tokenList->length()){
        currentToken = tokenList->at(point);
        point++;
        tokenType type = currentToken->type;
        if(type == tokenType::EQ||
                type == tokenType::GT||
                type == tokenType::GET||
                type == tokenType::LT||
                type == tokenType::LET||
                type == tokenType::NEQ){
            TreeNode *node = new TreeNode(treeNodeType::OP, getNextTokenLineNo());
            node->mDataType = type;
            return node;
        }
    }
    throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> logical operateor"));
}

//加减运算符
TreeNode *SyntaxAnalysis::addtiveOp()
{
    if(point < tokenList->length()){
        currentToken = tokenList->at(point);
        tokenType type = currentToken->type;
        if(type == tokenType::PLUS || type == tokenType::MINUS ){
            TreeNode *node = new TreeNode(treeNodeType::OP, getNextTokenLineNo());
            node->mDataType = type;
            return node;
        }
    }
    throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> addttive operateor"));
}

//乘除运算符
TreeNode *SyntaxAnalysis::multiplyOp()
{
    if(point < tokenList->length()){
        currentToken = tokenList->at(point);
        point++;
        tokenType type = currentToken->type;
        if(type == tokenType::MUL || type == tokenType::DIV){
            TreeNode *node = new TreeNode(treeNodeType::OP, getNextTokenLineNo());
            node->mDataType = type;
            return node;
        }
    }
    throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> multiple operateor"));
}

//变量名，可能是单个的变量，也可能是数组的一个元素
TreeNode *SyntaxAnalysis::variableName()
{
    TreeNode *node = new TreeNode(treeNodeType::VAR, getNextTokenLineNo());
    if(checkNextTokenType(tokenType::ID)){
        currentToken = tokenList->at(point);
        point++;
        node->value = currentToken->value;
    }else{
        delete node;
        node = nullptr;
        throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> ID"));
    }
    if(getNextTokenType() == tokenType::LBRACKET){
        consumeNextToken(tokenType::LBRACKET);
        node->mLeft = parseExp();
        consumeNextToken(tokenType::RBRACKET);
    }
    return node;
}

//消耗掉下一个token,要求必须是type类型，消耗后，currentTokenjiang 停在最后消耗的token上
//消耗失败返回
void SyntaxAnalysis::consumeNextToken(tokenType type)
{
    if(point < tokenList->length()){
        currentToken = tokenList->at(point);
        if(currentToken->type == type){
            point++;
            return;
        }
    }
    throw new ParserException("line " + QString::number(getLastTokenLineNo()) + " : after " + getLastTokenName() + QString(" should be -> ") + Token::getTokenTypeName(type));
}

//检查下一个token的类型是否和type相同，调用此函数currentToken位置不变
//相同true，不同false
bool SyntaxAnalysis::checkNextTokenType(tokenType type)
{
    if(point < tokenList->length()){
        tokenType nextType = tokenList->at(point)->type;
        if(nextType == type)
            return true;
    }
    return false;
}

//检查下一个token的类型是否和types中的每一个元素相同，调用此函数currentToken位置不变
//相同true，不同false
bool SyntaxAnalysis::checkNextTokenType(tokenType types[],int len)
{
    if(point < tokenList->length()){
        tokenType nextType = tokenList->at(point)->type;
        for(int i=0; i < len; i++)
        if(nextType == types[i])
            return true;
    }
    return false;
}

//获取下一个token的type,如果没有返回下一个token，返回tokenType.NULLToken
tokenType SyntaxAnalysis::getNextTokenType()
{
    if(point < tokenList->length()){//有下一个
        return tokenList->at(point)->type;
    }
    return tokenType::NULLTOKEN;
}

//获取上一个token的lineNo
int SyntaxAnalysis::getLastTokenLineNo()
{
    if(point == 0)
        return tokenList->at(0)->lineNo;
    return tokenList->at(point-1)->lineNo;
}

int SyntaxAnalysis::getNextTokenLineNo()
{
    if(point < tokenList->length())
        return tokenList->at(point)->lineNo;
    else
        return 0;
}


QString SyntaxAnalysis::getLastTokenName()
{
    if(point == 0)
        return "";
    return tokenList->at(point-1)->toString();
}





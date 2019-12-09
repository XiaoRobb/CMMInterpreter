#ifndef TOKEN_H
#define TOKEN_H

#include <QString>

enum tokenType
{
    ERROR = -1, //错误
    NULLTOKEN,  //不是任何类型，初始化Token时的类型
    IF,       // if 1
    ELSE,       // else 2
    WHILE,      // while 3
    READ,       // read 4
    WRITE,      // write 5
    INT,        // int 6
    REAL,       // real 7
    PLUS,       // + 8
    MINUS,      // - 9
    MUL,        // * 10
    DIV,        // / 11
    ASSIGN,     // = 12
    LT,         // < 13
    EQ,         // == 14
    NEQ,        // <> 15
    LPARENT,    // ( 16
    RPARENT,    // ) 17
    SEMI,       // ; 18
    LBRACE,     // { 19
    RBRACE,     // } 20
    //LCOM,       // /* 21
    //RCOM,       // */ 22
    //SCOM,       // // 23
    LBRACKET,   // [ 24
    RBRACKET,   // ] 25
    LET,        // <= 26
    GT,         // > 27
    GET,        // >= 28
    ID,         // 标识符 29（由数字，字母或下划线组成，第一个字符不是数字）
    LITERAL_INT,//int型字面值
    LITERAL_REAL,//real型字面值
    LOGIC_EXP,//逻辑表达式
    ADDTIVE_EXP,//多项式
    TERM_EXP//项
};

class Token
{
public:
    Token(int lineNo);
    Token(tokenType type, int lineNo);
    Token(tokenType type, QString value, int lineNo);
    QString toStringWithLine() const;
    QString toString() const;
    static QString getTokenTypeName(tokenType type);

    tokenType type;//类型
    QString value;//如果token需要值，则使用这个存储，比如ID，LITERAL_INT,LITERAL_REAL
    int lineNo;//行号

};

#endif // TOKEN_H

#ifndef VALUE_H
#define VALUE_H

#include <QString>
#include <QList>

enum symbolType{
    TEMP=-1,
    SINGLE_INT,
    SINGLE_REAL,
    ARRAY_INT,
    ARRAY_REAL,
    //仅供value使用
    TRUE,
    FALSE,
};

// 用于存储符号表中符号对应的值，包括int real 以及数组符号对应的值
class Value
{
public:
    Value(symbolType type);
    Value(bool boolean);//存储bool值用Value对象
    ~Value();
    void initArray(int dim);
    Value * PLUS(Value *value, int lineNo);
    Value * MINUS(Value *value, int lineNo);
    Value * MUL(Value * value, int lineNo);
    Value * DIV(Value * value, int lineNo);
    Value * GT(Value * value, int lineNo);
    Value * EQ(Value * value, int lineNo);
    Value * OR(Value * value);
    Value * GET(Value * value, int lineNo);
    Value * LT(Value * value, int lineNo);
    Value * LET(Value * value, int lineNo);
    Value * NEQ(Value * value, int lineNo);
    static Value * NOT(Value * value, int lineNo);
    QString toString();
    Value * toReal();
    bool isZero();

    symbolType mType;
    int mInt;
    double mReal;
    int dim;
    int *mArrayInt;
    double *mArrayReal;




};

#endif // VALUE_H

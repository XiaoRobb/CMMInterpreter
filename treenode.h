#ifndef TREENODE_H
#define TREENODE_H

#include <QString>
#include "token.h"

enum treeNodeType{
    //NULL类型的TreeNode作为头部，注意NULL节点不存储信息，仅仅使用next指向下一个节点
    BLOCK,

    //if语句：left存exp，midle存if条件正确时的TreeNode，right存else的TreeNode（如果有）
    IF_STMT,

    //while语句：left存exp，midle存循环体
    WHILE_STMT,

    //left存一个var
    READ_STMT,

    //left存一个exp
    WRITE_STMT,

    //声明语句：left存var，middle存exp（若有赋值）
    DECLARE_STMT,

    //赋值语句：left存var，middle存exp
    ASSIGN_STMT,

    //复合表达式：datatype可能为LOGIC_EXP ADDTIVE_EXP TERM_EXP
    //value == null
    EXP,

    //变量：datatype存变量类型Token.INT 和 REAL
    //value存变量名
    //left：在声明语句中变量的left值代表变量长度exp，在其他的调用中变量left表示变量索引值exp
    VAR,

    //运算符：在datatype中存储操作符类型
    OP,

    //因子：datatype存Token.PLUS/MINUS, 默认为Token.PLUS
    //left存TreeNode
    //若TreeNode是var，代表一个变量/数组元素
    //若TreeNode是exp，则是一个表达式因子
    //若是LITERAL,则该LITERAL的value中存放字面的字符形式
    //EXP为因子时，mDataType存储符号PLUS/MINUS
    FACTOR,

    //字面值：value存字面值，无符号，datatyoe存放类型
    LITREAL,
};

class TreeNode
{
public:
    TreeNode(treeNodeType type);
    TreeNode(treeNodeType type, int lineNo);
//    TreeNode(TreeNode *var);
//    void Free();

    treeNodeType type;
    tokenType mDataType;
    TreeNode *mLeft;
    TreeNode *mMiddle;
    TreeNode *mRight;

    //如果是代码块中的代码，则mNext指向其后面的一条语句，普通的顶级代码都是存在list中，不需要这个参数
    TreeNode *mNext;
    QString value;
    QString toString() const;
    int lineNo;    //此语句的开始行号数，如while语句可能是 10-15行，lineNo = 10;
};

#endif // TREENODE_H

#include "treenode.h"

TreeNode::TreeNode(treeNodeType type)
{
    this->type = type;
    switch (type) {
    case treeNodeType::FACTOR :
    case treeNodeType::LITREAL :
        this->mDataType = tokenType::PLUS;
        break;
    default:
        this->mDataType = tokenType::NULLTOKEN;
        break;
    }

    mLeft = nullptr;
    mMiddle = nullptr;
    mRight = nullptr;
    mNext = nullptr;
    lineNo = 0;
}

TreeNode::TreeNode(treeNodeType type, int lineNo)
{
    this->type = type;
    switch (type) {
    case treeNodeType::FACTOR :
    case treeNodeType::LITREAL :
        this->mDataType = tokenType::PLUS;
        break;
    default:
        this->mDataType = tokenType::NULLTOKEN;
        break;
    }

    mLeft = nullptr;
    mMiddle = nullptr;
    mRight = nullptr;
    mNext = nullptr;
    this->lineNo = lineNo;
}

QString TreeNode::toString() const
{
    switch (this->type) {
        case treeNodeType::IF_STMT: return QString("IF_STMT");
        case treeNodeType::WHILE_STMT: return QString("WHILE_STMT");
        case treeNodeType::READ_STMT: return QString("READ_STMT");
        case treeNodeType::WRITE_STMT: return QString("WRITE_STMT");
        case treeNodeType::DECLARE_STMT: return QString("DECLARE_STMT");
        case treeNodeType::ASSIGN_STMT: return QString("ASSIGN_STMT");
        case treeNodeType::EXP:
        case treeNodeType::VAR: return value;
        case treeNodeType::OP: return Token::getTokenTypeName(this->mDataType);
        case treeNodeType::BLOCK: return QString("");
        default: return QString("UNKNOWN");
    }
}


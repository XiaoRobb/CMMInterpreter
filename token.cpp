#include "token.h"


Token::Token(int lineNo)
{
    type = tokenType::NULLTOKEN;
    this->lineNo = lineNo;
    this->value = "";
}

Token::Token(tokenType type, int lineNo)
{
    this->type = type;
    this->lineNo = lineNo;
    this->value = "";

}

Token::Token(tokenType type, QString value, int lineNo)
{
    this->type = type;
    this->value = value;
    this->lineNo = lineNo;
}

QString Token::toStringWithLine() const
{
    switch (this->type)
    {
        case tokenType::IF : return QString("Line ")+QString::number(lineNo)+QString(": IF");
        case tokenType::ELSE : return QString("Line ")+QString::number(lineNo)+QString(": ELSE");
        case tokenType::WHILE : return QString("Line ")+QString::number(lineNo)+QString(": WHILE");
        case tokenType::READ : return QString("Line ")+QString::number(lineNo)+QString(": READ");
        case tokenType::WRITE : return QString("Line ")+QString::number(lineNo)+QString(": WRITE");
        case tokenType::INT : return QString("Line ")+QString::number(lineNo)+QString(": INT");
        case tokenType::REAL : return QString("Line ")+QString::number(lineNo)+QString(": REAL");
        case tokenType::PLUS : return QString("Line ")+QString::number(lineNo)+QString(": +");
        case tokenType::MINUS : return QString("Line ")+QString::number(lineNo)+QString(": -");
        case tokenType::MUL : return QString("Line ")+QString::number(lineNo)+QString(": *");
        case tokenType::DIV : return QString("Line ")+QString::number(lineNo)+QString(": /");
        case tokenType::ASSIGN : return QString("Line ")+QString::number(lineNo)+QString(": =");
        case tokenType::LT : return QString("Line ")+QString::number(lineNo)+QString(": <");
        case tokenType::LET : return QString("Line ")+QString::number(lineNo)+QString(": <=");
        case tokenType::EQ : return QString("Line ")+QString::number(lineNo)+QString(": ==");
        case tokenType::NEQ : return QString("Line ")+QString::number(lineNo)+QString(": <>");
        case tokenType::LPARENT : return QString("Line ")+QString::number(lineNo)+QString(": (");
        case tokenType::RPARENT : return QString("Line ")+QString::number(lineNo)+QString(": )");
        case tokenType::SEMI : return QString("Line ")+QString::number(lineNo)+QString(": ;");
        case tokenType::LBRACE : return QString("Line ")+QString::number(lineNo)+QString(": {");
        case tokenType::RBRACE : return QString("Line ")+QString::number(lineNo)+QString(": }");
        case tokenType::LBRACKET : return QString("Line ")+QString::number(lineNo)+QString(": [");
        case tokenType::RBRACKET : return QString("Line ")+QString::number(lineNo)+QString(": ]");
        case tokenType::GT : return QString("Line ")+QString::number(lineNo)+QString(": >");
        case tokenType::GET : return QString("Line ")+QString::number(lineNo)+QString(": >=");
        case tokenType::ID :
        case tokenType::LITERAL_INT :
        case tokenType::LITERAL_REAL : return QString("Line ")+QString::number(lineNo)+QString(": ")+QString(value);
        default:return QString("LINE ")+QString::number(lineNo)+QString(": UNKNOWN");
    }
}

QString Token::toString() const
{
    switch (type)
    {
        case tokenType::IF : return QString("IF");
        case tokenType::ELSE : return QString("ELSE");
        case tokenType::WHILE : return QString("WHILE");
        case tokenType::READ : return QString("READ");
        case tokenType::WRITE : return QString("WRITE");
        case tokenType::INT : return QString("INT");
        case tokenType::REAL : return QString("REAL");
        case tokenType::PLUS : return QString("+");
        case tokenType::MINUS : return QString("-");
        case tokenType::MUL : return QString(":*");
        case tokenType::DIV : return QString("/");
        case tokenType::ASSIGN : return QString("=");
        case tokenType::LT : return QString("<");
        case tokenType::LET : return QString("<=");
        case tokenType::EQ : return QString("==");
        case tokenType::NEQ : return QString("<>");
        case tokenType::LPARENT : return QString("(");
        case tokenType::RPARENT : return QString(")");
        case tokenType::SEMI : return QString(";");
        case tokenType::LBRACE : return QString("{");
        case tokenType::RBRACE : return QString("}");
        case tokenType::LBRACKET : return QString("[");
        case tokenType::RBRACKET : return QString("]");
        case tokenType::GT : return QString(">");
        case tokenType::GET : return QString(">=");
        case tokenType::ID :
        case tokenType::LITERAL_INT :
        case tokenType::LITERAL_REAL : return QString(this->value);
        default:return QString("UNKNOWN");
    }
}

QString Token::getTokenTypeName(tokenType type)
{
    switch (type)
    {
        case tokenType::IF : return QString("IF");
        case tokenType::ELSE : return QString("ELSE");
        case tokenType::WHILE : return QString("WHILE");
        case tokenType::READ : return QString("READ");
        case tokenType::WRITE : return QString("WRITE");
        case tokenType::INT : return QString("INT");
        case tokenType::REAL : return QString("REAL");
        case tokenType::PLUS : return QString("+");
        case tokenType::MINUS : return QString("-");
        case tokenType::MUL : return QString(":*");
        case tokenType::DIV : return QString("/");
        case tokenType::ASSIGN : return QString("=");
        case tokenType::LT : return QString("<");
        case tokenType::LET : return QString("<=");
        case tokenType::EQ : return QString("==");
        case tokenType::NEQ : return QString("<>");
        case tokenType::LPARENT : return QString("(");
        case tokenType::RPARENT : return QString(")");
        case tokenType::SEMI : return QString(";");
        case tokenType::LBRACE : return QString("{");
        case tokenType::RBRACE : return QString("}");
        case tokenType::LBRACKET : return QString("[");
        case tokenType::RBRACKET : return QString("]");
        case tokenType::GT : return QString(">");
        case tokenType::GET : return QString(">=");
        case tokenType::ID : return QString("ID");
        case tokenType::LITERAL_INT : return QString("INT");
        case tokenType::LITERAL_REAL : return QString("REAL");
        default:return QString("UNKNOWN");
    }
}

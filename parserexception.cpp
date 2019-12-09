#include "parserexception.h"

ParserException::ParserException(QString message)
{
    this->message = message;
}

void ParserException::raise() const
{
    throw *this;
}


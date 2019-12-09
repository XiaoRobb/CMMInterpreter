#include "interpretexception.h"

InterpretException::InterpretException(QString message)
{
    this->message = message;
}

void InterpretException::raise() const
{
    throw *this;
}

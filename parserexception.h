#ifndef PARSEREXCEPTION_H
#define PARSEREXCEPTION_H

#include <QException>
#include <QString>

class ParserException:public QException
{
public:
    ParserException(QString message);
    void raise() const override;
    ParserException *clone() const override { return new ParserException(*this); }
    QString getMessage() { return this->message; }
private:
    QString message;
};

#endif // PARSEREXCEPTION_H

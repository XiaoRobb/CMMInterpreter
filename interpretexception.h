#ifndef INTERPRETEXCEPTION_H
#define INTERPRETEXCEPTION_H

#include <QException>
#include <QString>

class InterpretException : public QException
{
public:
    InterpretException(QString message);
    void raise() const override;
    InterpretException *clone() const override { return new InterpretException(*this); }
    QString getMessage() { return this->message; }
private:
    QString message;
};

#endif // INTERPRETEXCEPTION_H

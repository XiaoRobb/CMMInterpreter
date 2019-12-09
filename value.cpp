#include "value.h"
#include "interpretexception.h"

Value::Value(symbolType type)
{
    this->mType = type;
    this->mInt = 0;
    this->mReal = 0.0;
    this->mArrayInt = nullptr;
    this->mArrayReal = nullptr;
    this->dim = 0;
}

Value::Value(bool boolean)
{
    if(boolean)
        this->mType = symbolType::TRUE;
    else
        this->mType = symbolType::FALSE;
    this->mInt = 0;
    this->mReal = 0.0;
    this->mArrayInt = nullptr;
    this->mArrayReal = nullptr;
    this->dim = 0;
}

Value::~Value()
{
    if(this->mArrayInt){
        delete[] mArrayInt;
        mArrayInt = nullptr;
    }
    if(this->mArrayReal){
        delete[] mArrayReal;
        mArrayReal = nullptr;
    }
}

void Value::initArray(int dim)
{
    this->dim = dim;
    if(mType == symbolType::ARRAY_INT)
        mArrayInt = new int[dim] ();
    else
        mArrayReal = new double[dim] ();
}

Value *Value::PLUS(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_REAL){
        Value *rv = new Value(symbolType::SINGLE_REAL);
        if(value->mType == symbolType::SINGLE_INT){
            rv->mReal = this->mReal + value->mInt;
            return rv;
        }else if(value->mType == symbolType::SINGLE_REAL){
            rv->mReal = this->mReal + value->mReal;
            return rv;
        }
    }else if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            Value *rv = new Value(symbolType::SINGLE_INT);
            rv->mInt = this->mInt + value->mInt;
            return rv;
        }else if(value->mType == symbolType::SINGLE_REAL){
            Value *rv = new Value(symbolType::SINGLE_REAL);
            rv->mReal = this->mInt + value->mReal;
            return rv;
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "算术运算非法");
}

Value *Value::MINUS(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_REAL){
        Value *rv = new Value(symbolType::SINGLE_REAL);
        if(value->mType == symbolType::SINGLE_INT){
            rv->mReal = this->mReal - value->mInt;
            return rv;
        }else if(value->mType == symbolType::SINGLE_REAL){
            rv->mReal = this->mReal - value->mReal;
            return rv;
        }
    }else if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            Value *rv = new Value(symbolType::SINGLE_INT);
            rv->mInt = this->mInt - value->mInt;
            return rv;
        }else if(value->mType == symbolType::SINGLE_REAL){
            Value *rv = new Value(symbolType::SINGLE_REAL);
            rv->mReal = this->mInt - value->mReal;
            return rv;
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "算术运算非法");
}

Value *Value::MUL(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_REAL){
        Value *rv = new Value(symbolType::SINGLE_REAL);
        if(value->mType == symbolType::SINGLE_INT){
            rv->mReal = this->mReal * value->mInt;
            return rv;
        }else if(value->mType == symbolType::SINGLE_REAL){
            rv->mReal = this->mReal * value->mReal;
            return rv;
        }
    }else if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            Value *rv = new Value(symbolType::SINGLE_INT);
            rv->mInt = this->mInt * value->mInt;
            return rv;
        }else if(value->mType == symbolType::SINGLE_REAL){
            Value *rv = new Value(symbolType::SINGLE_REAL);
            rv->mReal = this->mInt * value->mReal;
            return rv;
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "算术运算非法");
}

Value *Value::DIV(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_REAL){
        Value *rv = new Value(symbolType::SINGLE_REAL);
        if(value->mType == symbolType::SINGLE_INT){
            if(value->mInt == 0)
                throw new InterpretException("Line " + QString::number(lineNo) + " : " + "不能除0");
            rv->mReal = this->mReal / value->mInt;
            return rv;
        }else if(value->mType == symbolType::SINGLE_REAL){
            if(value->mReal == 0.0)
                throw new InterpretException("Line " + QString::number(lineNo) + " : " + "不能除0");
            rv->mReal = this->mReal / value->mReal;
            return rv;
        }
    }else if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            if(value->mInt == 0)
                throw new InterpretException("Line " + QString::number(lineNo) + " : " + "不能除0");
            Value *rv = new Value(symbolType::SINGLE_INT);
            rv->mInt = this->mInt / value->mInt;
            return rv;
        }else if(value->mType == symbolType::SINGLE_REAL){
            Value *rv = new Value(symbolType::SINGLE_REAL);
            if(value->mReal == 0.0)
                throw new InterpretException("Line " + QString::number(lineNo) + " : " + "不能除0");
            rv->mReal = this->mInt / value->mReal;
            return rv;
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "算术运算非法");
}

Value *Value::GT(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mInt > value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            return new Value(this->mInt > value->mReal);
        }
    }else if(this->mType == symbolType::SINGLE_REAL){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mReal > value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            return new Value(this->mReal > value->mReal);
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "逻辑比较非法");
}

Value *Value::EQ(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mInt == value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            double result = value->mReal-this->mInt;
            bool flag = (result < 0.0000001 && result >= 0)  || (result > -0.0000001 && result <0);
            return new Value(flag);
        }
    }else if(this->mType == symbolType::SINGLE_REAL){
        if(value->mType == symbolType::SINGLE_INT){
            double result = this->mReal-value->mInt;
            bool flag = (result < 0.0000001 && result >= 0)  || (result > -0.0000001 && result <0);
            return new Value(flag);
        }else if(value->mType == symbolType::SINGLE_REAL){
            double result = this->mReal-value->mReal;
            bool flag = (result < 0.0000001 && result >= 0)  || (result > -0.0000001 && result <0);
            return new Value(flag);
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "逻辑比较非法");
}

Value *Value::OR(Value *value)
{
    if(this->mType == symbolType::TRUE || value->mType == symbolType::TRUE)
        return new Value(symbolType::TRUE);
    else
        return new Value(symbolType::FALSE);
}

Value *Value::GET(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mInt >= value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            return new Value(this->mInt >= value->mReal);
        }
    }else if(this->mType == symbolType::SINGLE_REAL){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mReal >= value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            return new Value(this->mReal >= value->mReal);
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "逻辑比较非法");
}

Value *Value::LT(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mInt < value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            return new Value(this->mInt < value->mReal);
        }
    }else if(this->mType == symbolType::SINGLE_REAL){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mReal < value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            return new Value(this->mReal < value->mReal);
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "逻辑比较非法");
}

Value *Value::LET(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mInt <= value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            return new Value(this->mInt <= value->mReal);
        }
    }else if(this->mType == symbolType::SINGLE_REAL){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mReal <= value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            return new Value(this->mReal <= value->mReal);
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "逻辑比较非法");
}

Value *Value::NEQ(Value *value, int lineNo)
{
    if(this->mType == symbolType::SINGLE_INT){
        if(value->mType == symbolType::SINGLE_INT){
            return new Value(this->mInt != value->mInt);
        }else if(value->mType == symbolType::SINGLE_REAL){
            double result = value->mReal-this->mInt;
            bool flag = (result < 0.0000001 && result >= 0)  || (result > -0.0000001 && result <0);
            return new Value(!flag);
        }
    }else if(this->mType == symbolType::SINGLE_REAL){
        if(value->mType == symbolType::SINGLE_INT){
            double result = this->mReal-value->mInt;
            bool flag = (result < 0.0000001 && result >= 0)  || (result > -0.0000001 && result <0);
            return new Value(!flag);
        }else if(value->mType == symbolType::SINGLE_REAL){
            double result = this->mReal-value->mReal;
            bool flag = (result < 0.0000001 && result >= 0)  || (result > -0.0000001 && result <0);
            return new Value(!flag);
        }
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "逻辑比较非法");
}

Value *Value::NOT(Value *value, int lineNo)
{
    if(value->mType == symbolType::TRUE){
        return new Value(symbolType::FALSE);
    }else if(value->mType == symbolType::FALSE){
        return new Value(symbolType::TRUE);
    }else if(value->mType == symbolType::SINGLE_INT){
        Value *rv = new Value(symbolType::SINGLE_INT);
        rv->mInt = value->mInt * -1;
        return rv;
    }else if(value->mType == symbolType::SINGLE_REAL){
        Value *rv = new Value(symbolType::SINGLE_REAL);
        rv->mReal = value->mReal * -1;
        return rv;
    }
    throw new InterpretException("Line " + QString::number(lineNo) + " : " + "负号使用非法");
}

QString Value::toString()
{
    switch (mType) {
    case symbolType::SINGLE_INT:
        return QString::number(mInt);
    case symbolType::SINGLE_REAL:
        return QString::number(mReal);
    case symbolType::TRUE:
        return "true";
    case symbolType::FALSE:
        return "false";
    default:
        return "array";
    }
}

Value *Value::toReal()
{
    if(mType == symbolType::SINGLE_REAL)
        return this;
    else{
        mType = symbolType::SINGLE_REAL;
        mReal = mInt;
        mInt = 0;
        return this;
    }
}

bool Value::isZero()
{
    if((mType == symbolType::SINGLE_INT && mInt == 0) ||
            (mType == symbolType::SINGLE_REAL && mReal == 0.0)||
            (mType == symbolType::ARRAY_INT && mArrayInt[mInt] == 0)||
            (mType == symbolType::ARRAY_REAL && mArrayReal[mInt] == 0.0))
        return true;
    else
        return false;
}



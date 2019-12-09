#include "lexicalanalysis.h"
#include <QDebug>

int LexicalAnalysis::lineNo = 1;
int LexicalAnalysis::currentInt = 0;
QString LexicalAnalysis::currenChar = "";
QString LexicalAnalysis::text = "";
LexicalAnalysis::LexicalAnalysis()
{

}

QList<const Token*> * LexicalAnalysis::lexicalAnalysis(QString text)
{
    if(text == "")
        return nullptr;
    lineNo = 1;
    currentInt = 0;
    currenChar = "";
    QList<const Token*> *tokens = new QList<const Token*>;
    LexicalAnalysis::text = text;
    QString valueBuff = "";
    readChar();
    while(currenChar != ""){

        //消耗空白字符
        if(currenChar == "\n"
                ||currenChar == "\r"
                ||currenChar == "\t"
                ||currenChar == "\f"
                ||currenChar == " "){
            readChar();
            continue;
        }

        //简单特殊符号
        if(currenChar == ";"){
            tokens->append(new const Token(tokenType::SEMI, lineNo));
            readChar();
            continue;
        }else if (currenChar == "+"){
            tokens->append(new const Token(tokenType::PLUS, lineNo));
            readChar();
            continue;
        }else if (currenChar == "-"){
            tokens->append(new const Token(tokenType::MINUS, lineNo));
            readChar();
            continue;
        }else if (currenChar == "*"){
            tokens->append(new const Token(tokenType::MUL, lineNo));
            readChar();
            continue;
        }else if (currenChar == "("){
            tokens->append(new const Token(tokenType::LPARENT, lineNo));
            readChar();
            continue;
        }else if (currenChar == ")"){
            tokens->append(new const Token(tokenType::RPARENT, lineNo));
            readChar();
            continue;
        }else if (currenChar == "["){
            tokens->append(new const Token(tokenType::LBRACKET, lineNo));
            readChar();
            continue;
        }else if (currenChar == "]"){
            tokens->append(new const Token(tokenType::RBRACKET, lineNo));
            readChar();
            continue;
        }else if (currenChar == "{"){
            tokens->append(new const Token(tokenType::LBRACE, lineNo));
            readChar();
            continue;
        }else if (currenChar == "}"){
            tokens->append(new const Token(tokenType::RBRACE, lineNo));
            readChar();
            continue;
        }

        //复合特殊符号
        if(currenChar == "/"){
            readChar();
            if(currenChar == "*"){//多行注释
                while(1){//使用死循环消耗多行注释的内容
                    readChar();
                    if(currenChar == "*"){//如果是"*"，那么有可能是多行注释结尾
                        readChar();
                        if(currenChar == '/'){//多行注释结束符号
                            readChar();
                            break;
                        }else{//如果不是*就继续读下一个相当于忽略了这个字符
                            readChar();
                        }
                    }
                }
                continue;
            }else{//除号
                tokens->append(new const Token(tokenType::DIV, lineNo));
                continue;
            }
        }else if (currenChar == "="){
            readChar();
            if(currenChar == "="){
                tokens->append(new const Token(tokenType::EQ, lineNo));
                readChar();
            }else{
                tokens->append(new const Token(tokenType::ASSIGN, lineNo));
            }
            continue;
        }else if (currenChar == ">"){
            readChar();
            if(currenChar == "="){
                tokens->append(new const Token(tokenType::GET, lineNo));
                readChar();
            }else{
                tokens->append(new const Token(tokenType::GT, lineNo));
            }
            continue;
        }else if(currenChar == "<"){
            readChar();
            if(currenChar == "="){
                tokens->append(new const Token(tokenType::LET, lineNo));
                readChar();
            }else if(currenChar == ">"){
                tokens->append(new const Token(tokenType::NEQ, lineNo));
                readChar();
            }else{
                tokens->append(new const Token(tokenType::LT, lineNo));
            }
            continue;
        }

        //数字
        if(currenChar.compare("0") >=0 && currenChar.compare("9") <= 0){
            bool isReal = false;//是否是小数
            while((currenChar.compare("0") >=0 && currenChar.compare("9") <= 0)
                  || currenChar == "."){
                if(currenChar == '.'){
                    if(isReal)
                        break;
                    else
                        isReal = true;
                }
                valueBuff += currenChar;
                readChar();
            }
            if(currenChar != "\n"){
                if(isReal)
                    tokens->append(new const Token(tokenType::LITERAL_REAL, valueBuff, lineNo));
                else
                    tokens->append(new const Token(tokenType::LITERAL_INT, valueBuff, lineNo));
            }else{
                if(isReal)
                    tokens->append(new const Token(tokenType::LITERAL_REAL, valueBuff, lineNo-1));
                else
                    tokens->append(new const Token(tokenType::LITERAL_INT, valueBuff, lineNo-1));
            }
            valueBuff = "";
            continue;
        }

        //字符组成的标识符包括保留字和ID
        if((currenChar.compare("a") >= 0 && currenChar.compare("z") <=0)
                || currenChar == "_" ||(currenChar.compare("A") >= 0 && currenChar.compare("Z") <=0)){
            //取剩下的可能是的字符
            while((currenChar.compare("a") >= 0 && currenChar.compare("z") <= 0)
                  ||(currenChar.compare("A") >= 0 && currenChar.compare("Z") <= 0)
                  ||currenChar == "_"
                  ||(currenChar.compare("0") >=0 && currenChar.compare("9") <= 0)){
                valueBuff += currenChar;
                readChar();
            }
            if(currenChar != "\n"){
                if(valueBuff == "if"){
                    tokens->append(new const Token(tokenType::IF, lineNo));
                }else if(valueBuff == "else"){
                    tokens->append(new const Token(tokenType::ELSE, lineNo));
                }else if(valueBuff == "while"){
                    tokens->append(new const Token(tokenType::WHILE, lineNo));
                }else if(valueBuff == "read"){
                    tokens->append(new const Token(tokenType::READ, lineNo));
                }else if(valueBuff == "write"){
                    tokens->append(new const Token(tokenType::WRITE, lineNo));
                }else if(valueBuff == "int"){
                    tokens->append(new const Token(tokenType::INT, lineNo));
                }else if(valueBuff == "real"){
                    tokens->append(new const Token(tokenType::REAL, lineNo));
                }else{
                    tokens->append(new const Token(tokenType::ID, valueBuff, lineNo));
                }
            }else{
                if(valueBuff == "if"){
                    tokens->append(new const Token(tokenType::IF, lineNo-1));
                }else if(valueBuff == "else"){
                    tokens->append(new const Token(tokenType::ELSE, lineNo-1));
                }else if(valueBuff == "while"){
                    tokens->append(new const Token(tokenType::WHILE, lineNo-1));
                }else if(valueBuff == "read"){
                    tokens->append(new const Token(tokenType::READ, lineNo-1));
                }else if(valueBuff == "write"){
                    tokens->append(new const Token(tokenType::WRITE, lineNo-1));
                }else if(valueBuff == "int"){
                    tokens->append(new const Token(tokenType::INT, lineNo-1));
                }else if(valueBuff == "real"){
                    tokens->append(new const Token(tokenType::REAL, lineNo-1));
                }else{
                    tokens->append(new const Token(tokenType::ID, valueBuff, lineNo-1));
                }
            }

            valueBuff = "";
            continue;
        }
        readChar();
    }
    return tokens;
}

void LexicalAnalysis::readChar()
{
    if(currentInt >= text.length()){
        currenChar = "";
        return;
    }

    currenChar = text.at(currentInt);
    currentInt++;
    if(currenChar == "\n")
        lineNo++;
}

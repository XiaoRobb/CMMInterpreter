#include "linebutton.h"
#include <QFont>

int LineButton::height = 23;
int LineButton::width = 50;
LineButton::LineButton(QWidget * parent, int line):QPushButton(parent)
{
    this->line = line;
    QFont font;
    font.setPixelSize(20);
    setFont(font);
    setText(QString::number(line));
    resize(width,height);
    setFlat(true);
    move(0, 23*(line-1));
    setCursor(Qt::PointingHandCursor);
    clicked = false;
}

void LineButton::changeText()
{
    if(!clicked){
        setIcon(QIcon(":/images/images/point.png"));
        clicked = true;
    }else{
        setIcon(QIcon());
        clicked = false;
    }
}

bool LineButton::isClicked()
{
    return clicked;
}

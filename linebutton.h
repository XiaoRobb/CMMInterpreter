#ifndef LINEBUTTON_H
#define LINEBUTTON_H

#include <QPushButton>

class LineButton :public QPushButton
{
public:
    LineButton(QWidget *parent, int line);
    void changeText();
    bool isClicked();
    static int height;
    static int width;
    int line;
private:
    bool clicked;

};

#endif // LINEBUTTON_H

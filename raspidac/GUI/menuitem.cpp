#include "menuitem.h"

MenuItem::MenuItem(QString text, QWidget *parent) : QLabel(text, parent)
{
    setIndent(15);
    m_selected = false;
}

MenuItem::~MenuItem()
{

}

bool MenuItem::selected()
{
    return m_selected;
}

void MenuItem::select()
{
    QFont font = this->font();
    font.setBold(true);
    font.setPixelSize(15);
    setFont(font);
    m_selected = true;
}

void MenuItem::unselect()
{
    QFont font = this->font();
    font.setBold(false);
    font.setPixelSize(13);
    setFont(font);
    m_selected = false;
}


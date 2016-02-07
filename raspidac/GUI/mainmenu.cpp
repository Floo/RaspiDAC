#include "mainmenu.h"
#include "menuitem.h"

MainMenu::MainMenu(QWidget *parent) : QWidget(parent)
{
    m_maxItem = 7;
    QFont font;
    font.setPointSize(13);
    setFont(font);
    setAutoFillBackground(true);
    setStyleSheet(QLatin1String("QWidget{\n"
                                "background-color:#4d4d4d;\n"
                                "}\n"
                                "\n"
                                "QLabel{\n"
                                "color:#fff;\n"
                                "}"));
    m_vlayout = new QVBoxLayout();
}

MainMenu::~MainMenu()
{

}

void  MainMenu::showMenu(int row)
{
    int i = 0;
    QList<MenuItem*> lst;
    lst = this->findChildren<MenuItem*>();
    foreach (MenuItem *lbl, lst)
    {
        lbl->unselect();
        if (i <= m_maxItem)
            lbl->setVisible(true);
        else
            lbl->setVisible(false);
        i++;
    }
    MenuItem *item = this->findChild<MenuItem*>(QString("lbl_%1").arg(row));
    item->select();
    show();
}

void MainMenu::selectNextRow()
{
    int i = -1;
    int lastVisibleItem = 0;
    int selectedItem = 0;
    QList<MenuItem*> lst;
    bool select_next = false;
    lst = this->findChildren<MenuItem*>();
    foreach (MenuItem *lbl, lst)
    {
        i++;
        if (lbl->isVisible())
        {
            lastVisibleItem = i;
        }
        if (lbl->selected()) {
            select_next = true;
            lbl->unselect();
            continue;
        }
        if (select_next) {
            lbl->select();
            select_next = false;
            selectedItem = i;
        }
    }
    if (select_next) {
        hideMenu();
    }
    if (m_scroll && lastVisibleItem < lst.size() - 1 &&
            selectedItem >= lastVisibleItem)
    {
        i = 0;
        foreach (MenuItem *lbl, lst)
        {
            if ((i > lastVisibleItem - m_maxItem) && (i <= lastVisibleItem + 1))
            {
                lbl->setVisible(true);
            }
            else {
                lbl->setVisible(false);
            }
            i++;
        }
    }
}


void MainMenu::clear()
{
    QList<MenuItem*> lst;
    lst = this->findChildren<MenuItem*>();
    foreach (MenuItem *lbl, lst)
    {
        m_vlayout->removeWidget(lbl);
        delete lbl;
    }
}

void MainMenu::fill(const QStringList &lst)
{
    int i = 0;
    m_scroll = false;

    m_scroll = (lst.size() > m_maxItem);

    foreach (QString str, lst)
    {
        MenuItem *lbl = new MenuItem(str, this);
        lbl->setObjectName(QString("lbl_%1").arg(i));
        m_vlayout->addWidget(lbl);
        i++;
    }
    if (m_scroll)
        resize(m_width, 40 * m_maxItem + 1);
    else
        resize(m_width, 40 * i);

    setLayout(m_vlayout);
}

int MainMenu::getActivatedRow()
{
    QList<MenuItem*> lst;
    int i = 0;
    lst = this->findChildren<MenuItem*>();
    foreach (MenuItem *lbl, lst)
    {
        if (lbl->selected()) {
            return i;
        }
        i++;
    }
}

void MainMenu::hideMenu()
{
    emit onHide();
    hide();
}

void MainMenu::setWidth(int w)
{
    m_width = w;
}

#include "menu.h"
#include "mainmenu.h"

Menu::Menu(QWidget *parent) :
    QObject(parent),  m_radioLevel(0), m_inputLevel(0),
    m_setupLevel(0), m_parent(parent)
{
    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_timer->setInterval(5000);

    m_firstLevel = new MainMenu(parent);
    m_firstLevel->move(20,10);
    m_firstLevel->setWidth(160);
    hideall();

    QStringList stringlist;
    stringlist << "..." << "Radio" << "Digital Input" << "Musicplayer" << "Setup";
    m_firstLevel->fill(stringlist);

    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(hideall()));

}

Menu::~Menu()
{

}

void Menu::hideall()
{
    m_firstLevel->hide();
    if (m_radioLevel)
        m_radioLevel->hide();
    if (m_inputLevel)
        m_inputLevel->hide();
    if (m_setupLevel)
        m_setupLevel->hide();
    m_timer->stop();
}

void Menu::setRadioList(const QStringList& lst)
{
    if (!m_radioLevel) {
        m_radioLevel = new MainMenu(m_parent);
        m_radioLevel->hide();
        m_radioLevel->move(170,30);
        m_radioLevel->setWidth(260);
    }
    else {
        m_radioLevel->clear();
    }
    m_radioLevel->fill(lst);
}

void Menu::setInputList(QStringList &lst)
{
    if (!m_inputLevel) {
        m_inputLevel = new MainMenu(m_parent);
        m_inputLevel->hide();
        m_inputLevel->move(170,30);
        m_inputLevel->setWidth(140);
    }
    else {
        m_inputLevel->clear();
    }
    m_inputLevel->fill(lst);
}

void Menu::setInputList()
{
    QStringList lst;
    lst << "Input 1" << "Input 2" << "Input 3" << "Input 4";
    setInputList(lst);
}

void Menu::btnMenuPressed()
{
    m_timer->start();
    if (m_firstLevel->isHidden()) {
        m_firstLevel->showMenu(1);
    }
    else if (((m_radioLevel && m_radioLevel->isHidden()) &&
             (m_inputLevel && m_inputLevel->isHidden())) ||
             (!m_radioLevel && m_inputLevel && m_inputLevel->isHidden()) ||
             (!m_inputLevel && m_radioLevel && m_radioLevel->isHidden()) ||
             (!m_radioLevel && !m_inputLevel)) {
        m_firstLevel->selectNextRow();
    }
    else if (m_radioLevel && !m_radioLevel->isHidden()) {
        m_radioLevel->selectNextRow();
    }
    else if (m_inputLevel && !m_inputLevel->isHidden()) {
        m_inputLevel->selectNextRow();
    }

}

void Menu::btnSelectPressed()
{
    m_timer->start();
    if (m_radioLevel && !m_radioLevel->isHidden()) {
        int row = m_radioLevel->getActivatedRow();
        hideall();
        emit radioRowSelected(row);
    }
    else if (m_inputLevel && !m_inputLevel->isHidden()) {
        int row = m_inputLevel->getActivatedRow();
        hideall();
        emit inputSelected(row);
    }
    else if (!m_firstLevel->isHidden()) {
        int row = m_firstLevel->getActivatedRow();
        switch (row)
        {
        case 1:
            // Radio
            if (m_radioLevel)
                m_radioLevel->showMenu();
            break;
        case 2:
            // Input
            if (m_inputLevel)
                m_inputLevel->showMenu();
            break;
        case 3:
            //UPNP
            hideall();
            emit guiSelected(RaspiDAC::RPI_Upnp);
            break;
        }
    }
}

bool Menu::isHidden()
{
    return m_firstLevel->isHidden();
}

void Menu::hideMenu(){
    hideall();
}

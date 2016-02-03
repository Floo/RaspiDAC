#ifndef MENU_H
#define MENU_H

#include <QObject>
#include <QStringList>
#include <QTimer>
#include "../raspidac.h"

class MainMenu;

class Menu : public QObject
{
    Q_OBJECT
public:
    explicit Menu(QWidget *parent = 0);
    ~Menu();

    void setInputList();
    bool isHidden();

signals:
    void radioRowSelected(int);
    void inputSelected(int);
    void guiSelected(RaspiDAC::GUIMode);


public slots:
    void btnMenuPressed();
    void btnSelectPressed();
    void setRadioList(const QStringList& lst);
    void setInputList(QStringList& lst);

private:
    MainMenu *m_firstLevel;
    MainMenu *m_radioLevel;
    MainMenu *m_inputLevel;
    MainMenu *m_setupLevel;
    QTimer *m_timer;
    QWidget *m_parent;

private slots:
    void hideall();
};

#endif // MENU_H

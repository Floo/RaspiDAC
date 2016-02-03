#ifndef SECONDMENU_H
#define SECONDMENU_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>

#include "../raspidac.h"

class MainMenu : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = 0);
    ~MainMenu();
    void clear();
    void fill(const QStringList&);
    int getActivatedRow();
    void setWidth(int);

signals:
    void menuUpnp(RaspiDAC::GUIMode);
    void menuRadio(int);
    void menuSpdif(int);
    void onHide();

public slots:
    void selectNextRow();
    void showMenu(int row = 0);
    void hideMenu();

private:
    QVBoxLayout *m_vlayout;
    bool m_scroll;
    int m_maxItem;
    int m_width;
};

#endif // SECONDMENU_H

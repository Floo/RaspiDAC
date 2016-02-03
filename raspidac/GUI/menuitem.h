#ifndef MENUITEM_H
#define MENUITEM_H

#include <QWidget>
#include <QLabel>
#include <QFont>

class MenuItem : public QLabel
{
    Q_OBJECT
public:
    explicit MenuItem(QString text, QWidget *parent = 0);
    ~MenuItem();

    bool selected();
    void select();
    void unselect();

signals:

public slots:

private:
    bool m_selected;
};

#endif // MENUITEM_H

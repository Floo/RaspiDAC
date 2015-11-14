#ifndef STANDBYWINDOW_H
#define STANDBYWINDOW_H

#include <QWidget>

namespace Ui {
class StandbyWindow;
}

class StandbyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StandbyWindow(QWidget *parent = 0);
    ~StandbyWindow();

private:
    Ui::StandbyWindow *ui;
};

#endif // STANDBYWINDOW_H

#ifndef STANDBYWINDOW_H
#define STANDBYWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QTime>

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

public slots:
    void setCurrentTime();

};

#endif // STANDBYWINDOW_H

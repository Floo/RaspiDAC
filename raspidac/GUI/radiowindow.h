#ifndef RADIOWINDOW_H
#define RADIOWINDOW_H

#include <QWidget>

namespace Ui {
class RadioWindow;
}

class RadioWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RadioWindow(QWidget *parent = 0);
    ~RadioWindow();

public slots:
    void newStationName(QString);
    void newRadioState(QString);

private:
    Ui::RadioWindow *ui;
};

#endif // RADIOWINDOW_H

#ifndef RADIOWINDOW_H
#define RADIOWINDOW_H

#include <QWidget>
#include "HelperStructs/MetaData.h"

namespace Ui {
class RadioWindow;
}

class RadioWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RadioWindow(QWidget *parent = 0);
    ~RadioWindow();

    void update_track(const MetaData& metadata);
    void clear_track();

public slots:
    void newStationName(QString);
    void newRadioState(QString);

private:
    Ui::RadioWindow *ui;
};

#endif // RADIOWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "upnpwindow.h"
#include "mainwindow.h"
#include "radiowindow.h"
#include "standbywindow.h"
#include "spdifwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int currentIndex();
    int getIndexByName(QString key);
    void upnp_updateTrack(const MetaData &in);
    void upnp_setCurrentPosition(quint32 sec);
    void playing();
    void paused();
    void stopped();

signals:


public slots:
    void setCurrentIndex(int index);

private:
    Ui::MainWindow *ui;
    StandbyWindow *m_standbyWindow;
    RadioWindow *m_radioWindow;
    UpnpWindow *m_upnpWindow;
    SpdifWindow *m_spdifWindow;
    int addWidget(QWidget *widget);
    QMap<QString, int> windowMap;

    QFile *m_res;
    QFont *m_font;


};

#endif // MAINWINDOW_H

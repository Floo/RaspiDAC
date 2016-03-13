#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "../raspidac.h"
#include "upplay/HelperStructs/CSettingsStorage.h"

class StandbyWindow;
class RadioWindow;
class UpnpWindow;
class SpdifWindow;
class MessageWindow;

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
    void updateTrack(const MetaData &in);
    void forceUpdateTrackUpnp(const MetaData &in);
    void clearTrack();
    void upnp_setCurrentPosition(quint32 sec);
    void playing();
    void paused();
    void stopped();
    void input(QString);
    void spdifInput(int);
    void showMessage(QString &msg, int msec = 5000);

signals:
    void messageWindowClosed();

public slots:
    void setCurrentIndex(int index);
    void hideMessage();

private:
    Ui::MainWindow *ui;
    StandbyWindow *m_standbyWindow;
    RadioWindow *m_radioWindow;
    UpnpWindow *m_upnpWindow;
    SpdifWindow *m_spdifWindow;
    MessageWindow *m_messageWindow;
    int addWidget(QWidget *widget);
    int m_lastWindow;
    int m_msgwID;

    QTimer *m_msgTimer;

    QFile *m_res;
    QFont *m_font;

};

#endif // MAINWINDOW_H

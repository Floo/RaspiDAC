#ifndef UPNPWINDOW_H
#define UPNPWINDOW_H

#include <string>
#include <iostream>

#include <QWidget>
#include <QLabel>
#include <QFontDatabase>

#include "HelperStructs/Helper.h"



namespace Ui {

class UpnpWindow;
}

class ProgressBar;
class Ticker;
class AlbumArtLoader;

class UpnpWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UpnpWindow(QWidget *parent = 0);
    ~UpnpWindow();

signals:


public slots:
    void stopped();
    void paused();
    void playing();
    void setCurrentPosition(quint32);
    void update_track(const MetaData& md);
    void clear_track();
    void new_transport_state(int tps, const char *);

private slots:


private:
    Ui::UpnpWindow *ui;

    QLabel *m_label;
    Ticker *m_ticker;
    ProgressBar *m_progressbar;

    bool m_playing;
    quint32 m_completeLength_ms;
    bool m_metadata_available;

    MetaData m_metadata;
    AlbumArtLoader *m_albumartloader;
};

#endif // UPNPWINDOW_H

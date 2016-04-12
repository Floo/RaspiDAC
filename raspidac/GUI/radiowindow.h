#ifndef RADIOWINDOW_H
#define RADIOWINDOW_H

#include <QWidget>
#include <QTime>
#include "HelperStructs/MetaData.h"

namespace Ui {
class RadioWindow;
}

class AlbumArtLoader;
class Ticker;

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
    AlbumArtLoader *m_albumartloader;
    Ticker *m_stationText;
    Ticker *m_stationName;
    Ticker *m_nowPlaying;
};

#endif // RADIOWINDOW_H

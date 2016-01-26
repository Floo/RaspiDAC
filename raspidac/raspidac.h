#ifndef RASPIDAC_H
#define RASPIDAC_H

#include <QObject>
#include <QWidget>
#include "application.h"
#include "GUI/mainwindow.h"
#include "rpicontrol/netapiserver.h"
#ifdef __rpi__
    #include "rpicontrol/rpigpio.h"
#endif

class NetAPIServer;

class RaspiDAC : public QWidget
{
    Q_OBJECT
public:
    explicit RaspiDAC(Application *upapp, QWidget *parent = 0);
    ~RaspiDAC();

    //-----Interface zu upplay
    void setPlaylist(Rpi_Playlist* playlist);
    QWidget* getParentOfPlaylist();
    QWidget* getParentOfLibrary();
    void setLibraryWidget(QWidget* );
    void setPlaylistWidget(QWidget* );

    void ui_loaded();
    void setStyle(int);
    //-----Ende Interface

    void show();

signals:
    //-----Interface zu upplay
    /* Player*/
    void play();
    void pause();
    void stop();
    void backward();
    void forward();
    void sig_mute(bool ison);
    void sig_volume_changed (int);
    void sig_seek(int secs);

    /* File */
    void sig_choose_renderer();
    void sig_choose_source(QString);
    void sig_save_playlist();
    void sig_load_playlist();
    void sig_open_songcast();

    /* Preferences / View */
    void show_playlists();
    void show_small_playlist_items(bool);
    void sig_skin_changed(bool);
    void showSearchPanel(bool);
    void sig_sortprefs();
    void sig_preferences();
    //-----Ende Interface

public slots:
    //-----Interface zu upplay
    void really_close(bool=false);

    // For restoring from settings
    void setVolume(int vol);

    // Reflect externally triggered audio changes in ui
    void update_track(const MetaData& in);
    void setCurrentPosition(quint32 pos_sec);
    void stopped();
    void playing();
    void paused();
    void setVolumeUi(int volume_percent);
    void setMuteUi(bool);

    void enableSourceSelect(bool);
    //-----Ende Interface
    void onPlay();
    void onPaused();
    void onStopped();
    void onNext();
    void onPrevious();
    void setDACInput(int);
    void setSPDIFInput(int);
    void setMode(int mode);
    void setBacklight(int);
private:
    Rpi_Playlist *m_playlist;
    QWidget *m_playlistwidget;
    QWidget *m_librarywidget;
    Application *m_upapp;
    MainWindow *m_window;
#ifdef __rpi__
    RPiGPIO *rpiGPIO;
#endif
    NetAPIServer *netAPIServer;

private slots:


};

#endif // RASPIDAC_H

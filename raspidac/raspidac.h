#ifndef RASPIDAC_H
#define RASPIDAC_H

#include <QObject>
#include <QWidget>
#include <QTime>

#include "../upplay/HelperStructs/MetaData.h"
#include "rpicontrol/helper.h"
#include "../upplay/upqo/ohproduct_qo.h"
#include "rpicontrol/lirccontrol.h"
#ifdef __rpi__
    #include "rpicontrol/rpigpio.h"
#endif

class NetAPIServer;
class MainWindow;
class Rpi_Playlist;
class Application;
class Menu;
class LircControl;
class LircTest;

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
    void setPlaylistWidget(Rpi_Playlist*);

    void ui_loaded();
    void setStyle(int);
    //-----Ende Interface

    void show();
    enum GUIMode{RPI_Standby, RPI_Upnp, RPI_Radio, RPI_Spdif};
    Q_ENUMS(GUIMode)
    enum PlayMode{RPI_Play, RPI_Pause, RPI_Stop};
    Q_ENUMS(PlayMode)
    GUIMode getGUIMode();
    MetaData &getMetaData();
    OHProductQO::SourceType getSourceType();
    QString getRendererByName();
    QStringList* getRadioList();
    void prepareDatagram(bool metadatahaschanged = false, bool radiolisthaschanged = false);

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

    void datagramm(UDPDatagram&);
    void sendIRKey(LircControl::commandCode);

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

    void setDACInput(int);
    void setSPDIFInput(int);
    void setGUIMode(RaspiDAC::GUIMode mode);
    void setBacklight(int);
    void onTaster(int);
    void onTasterZweitbelegung(int);
    void setRadio(int);

private:
    Application *m_upapp;
    Rpi_Playlist *m_playlist;
    QWidget *m_playlistwidget;
    QWidget *m_librarywidget;
    MainWindow *m_window;
    Menu *m_menu;
    PlayMode m_playmode;
    MetaData m_MetaData;
    GUIMode m_lastMode;
    int m_spdifInput;
    NetAPIServer *netAPIServer;
    QString m_rendererName;
    bool m_shutdownPending;
    bool m_initialized;
    quint16 m_port;
    LircControl *m_lirc;
    LircTest *m_lirctest;

    void applySavedMetaData();
    void shutdownDevice();
    void restartDevice();

#ifdef __rpi__
    RPiGPIO *rpiGPIO;
#endif


private slots:
    void onMsgWinClosed();

};

Q_DECLARE_METATYPE(RaspiDAC::GUIMode)

#endif // RASPIDAC_H

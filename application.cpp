#include "application.h"

Application::Application(QApplication* qapp, int n_files, QObject *parent)
    :QObject(parent), m_app(qapp), m_initialized(false), m_settings(0), m_avto(0),
      m_rdco(0), m_hasOHRenderer(false), m_mpdradio(0)
{
    m_gui = new MainWindow();
    m_gui->show();

//    m_initialized = true;
//    return;

    m_settings = CSettingsStorage::getInstance();
    QString version = getVersion();
    m_settings->setVersion(version);

//    string uid = "8e4c4350c8b0411af4768317f48a4eb6";
//    string name = "Raspberry";
//    string ohname = "Raspberry (OpenHome)";
    string name = "RaspiDAC";
    string ohname = "RaspiDAC (OpenHome)";
//    string name = "SoundBridge";
//    string ohname = "SoundBridge (OpenHome)";
    m_initialized = true;
    superdir = UPnPDeviceDirectory::getTheDir();
    if (superdir == 0) {
        cerr << "Can't create UPnP discovery object" << endl;
        exit(1);
    }

    m_mpdradio = new Mpdradio();

    list_renderer();

    cerr << "!!! Alle Renderer aufgelistet." << endl;

    UPnPDeviceDesc ohddesc;
    UPnPDeviceDesc ddesc;
    MRDH ohrenderer;
    MRDH renderer;

    if (superdir->getDevByFName(name, ddesc)) {
        renderer = MRDH(new MediaRenderer(ddesc));
    } else {
        cerr << "getDevByFname failed for " << name << endl;
        renderer = MRDH();
    }

    if (superdir->getDevByFName(ohname, ohddesc)) {
        ohrenderer = MRDH(new MediaRenderer(ohddesc));
    } else {
        cerr << "getDevByFname failed for " << ohname << endl;
        ohrenderer = MRDH();
    }

    cerr << "!!! Alle MRDH erzeugt." << endl;

//    if (superdir->getDevByUDN(uid, ddesc)) {
//        renderer = MRDH(new MediaRenderer(ddesc));
//    } else {
//        cerr << "getDevByUDN failed" << endl;
//        renderer = MRDH();
//    }
    if (!renderer){
        cerr << "Renderer " << name << " not found" << endl;
        m_initialized = false;
        return;
    }

    if (!ohrenderer){
        cerr << "Renderer " << ohname << " not found" << endl;
//        m_initialized = false;
//        return;
    }

    cerr << "!!! Start RenderingControlQO." << endl;

    if (renderer->rdc()) {
        m_rdco = new RenderingControlQO(renderer->rdc());
    } else {
        cerr << "Device has no RenderingControl" << endl;
        m_initialized = false;
        return;
    }

    cerr << "!!! RenderingControlQO erzeugt." << endl;

    if (ohrenderer->ohvl()) {
        m_ohvlo =  new OHVolumeQO(ohrenderer->ohvl());
    } else {
        cerr << "Device has no OHVolumeControl" << endl;
        m_initialized = false;
        return;
    }

    cerr << "!!! OHVilumeControl erzeugt." << endl;

    OHPLH ohpl = ohrenderer->ohpl();
    if (ohpl) {
        m_ohplo = new OHPlayer(ohpl);
        OHTMH ohtm = ohrenderer->ohtm();
        if (ohtm) {
            m_ohtmo = new OHTimeQO(ohtm);
            m_hasOHRenderer = true;
        }
    }

    cerr << "!!! OHPlayer erzeugt." << endl;

//    m_playlist = new PlaylistAVT(renderer->desc()->UDN);
    if (renderer->avt()) {
        m_avto = new AVTPlayer(renderer->avt());
    } else {
        cerr << "Renderer: AVTransport missing but we need it" << endl;
        m_initialized = false;
        return;
    }
    //QString fn = QString::fromUtf8(renderer->desc()->friendlyName.c_str());
    cerr << "Renderer " << renderer->desc()->friendlyName <<  " initialisiert." << endl;
    if (m_hasOHRenderer) {
        cerr << "OHRenderer " << ohrenderer->desc()->friendlyName <<  " initialisiert." << endl;
    } else {
        cerr << "Could not initialize OHRenderer." << endl;
    }

    renderer_connections();
}

Application::~Application()
{

}

bool Application::is_initialized()
{
    return m_initialized;
}

QString Application::getVersion()
{
    return UPPLAY_VERSION;
}

void Application::list_renderer()
{
    vector<UPnPDeviceDesc> devices;
    if (!MediaRenderer::getDeviceDescs(devices) || devices.empty()) {
        cerr << "No Media Renderers found." << endl;
        return;
    }
    for (auto it = devices.begin(); it != devices.end(); it++) {
        cerr << it->friendlyName << endl;
        cerr << it->UDN << endl;
    }
}

void Application::renderer_connections()
{
//    CONNECT(m_playlist, sig_play_now(const MetaData&, int, bool),
//            m_avto, changeTrack(const MetaData&, int, bool));
//    CONNECT(m_playlist, sig_next_track_to_play(const MetaData&),
//            m_avto, infoNextTrack(const MetaData&));
//    CONNECT(m_avto, endOfTrackIsNear(),
//            m_playlist, psl_prepare_for_end_of_track());
//    CONNECT(m_avto, newTrackPlaying(const QString&),
//            m_playlist, psl_ext_track_change(const QString&));
//    CONNECT(m_avto, sig_currentMetadata(const MetaData&),
//            m_playlist, psl_onCurrentMetadata(const MetaData&));
    // the search (actually seek) param is in percent
    //CONNECT(m_player, search(int), m_avto, seekPC(int));
    if (m_hasOHRenderer) {
        cerr << "Connect OHRenderer." << endl;
        CONNECT(m_ohplo, audioStateChanged(int, const char*),
                m_gui, new_transport_state(int, const char *));
        CONNECT(m_ohtmo, secondsChanged(quint32),
                m_gui, setCurrentPosition(quint32));
        CONNECT(m_gui, sig_play(), m_ohplo, play());
        CONNECT(m_gui, sig_stop(), m_ohplo, stop());
        CONNECT(m_gui, sig_pause(), m_ohplo, pause());
        CONNECT(m_gui, sig_forward(), m_ohplo, next());
        CONNECT(m_gui, sig_backward(), m_ohplo, previous());
        //Radio
//        CONNECT(m_gui, sig_clear_playlist(),
//                m_ohplo, clear());
//        CONNECT(m_gui, sig_insert_tracks(const MetaDataList&, int),
//                m_ohplo, insertTracks(const MetaDataList&, int));
        CONNECT(m_gui, sig_playradio(),
                m_mpdradio, play_radio());
//        m_app->connect(m_gui, SIGNAL(sig_insert_tracks(const MetaDataList&,int)),
//                       m_ohplo, SLOT(insertTracks(const MetaDataList&,int)));
    } else {
        CONNECT(m_avto, audioStateChanged(int, const char*),
                m_gui, new_transport_state(int, const char *));
        CONNECT(m_gui, sig_play(), m_avto, play());
        CONNECT(m_gui, sig_stop(), m_avto, stop());
        CONNECT(m_gui, sig_pause(), m_avto, pause());
        CONNECT(m_avto, secsInSongChanged(quint32),
                m_gui, setCurrentPosition(quint32));
        CONNECT(m_gui, sig_radio(const MetaData&, int, bool),
                m_avto, changeTrack(const MetaData&, int, bool));
    }




    //CONNECT(m_avto, stoppedAtEOT(), m_playlist, psl_forward());

//    CONNECT(m_playlist, sig_stop(),  m_avto, stop());
//    CONNECT(m_playlist, sig_resume_play(), m_avto, play());
//    CONNECT(m_playlist, sig_pause(), m_avto, pause());

//TODO: Wird Volume und Mute benötigt?
//    CONNECT(m_rdco, volumeChanged(int), m_gui, setVolumeUi(int));
//    CONNECT(m_rdco, muteChanged(bool), m_gui, setMuteUi(bool));
    // Set up the initial volume from the renderer value
//    m_gui->setVolumeUi(m_rdco->volume());

    //TODO: Klasse RC5 und HWButton erzeugen und mit slots verknüpfen
//    CONNECT(m_gui, play(), m_playlist, psl_play());
//    CONNECT(m_gui, pause(), m_playlist, psl_pause());
//    CONNECT(m_gui, stop(), m_playlist, psl_stop());
//    CONNECT(m_gui, forward(), m_playlist, psl_forward());
//    CONNECT(m_gui, backward(), m_playlist, psl_backward());
//    CONNECT(m_gui, sig_load_playlist(), m_playlist, psl_load_playlist());
//    CONNECT(m_gui, sig_save_playlist(), m_playlist, psl_save_playlist());
//    CONNECT(m_gui, sig_volume_changed(int), m_rdco, setVolume(int));
//    CONNECT(m_gui, sig_mute(bool), m_rdco, setMute(bool));

    CONNECT(m_avto, sig_currentMetadata(const MetaData&),
            m_gui, update_track(const MetaData&));

//    CONNECT(m_playlist, sig_track_metadata(const MetaData&),
//            m_gui, update_track(const MetaData&));
//    CONNECT(m_playlist, sig_stopped(),  m_gui, stopped());
//    CONNECT(m_playlist, sig_paused(),  m_gui, paused());
//    CONNECT(m_playlist, sig_playing(),  m_gui, playing());

//    CONNECT(m_playlist, sig_playing_track_changed(int),
//            m_ui_playlist, track_changed(int));
//    CONNECT(m_playlist, sig_playlist_updated(MetaDataList&, int, int),
//            m_ui_playlist, fillPlaylist(MetaDataList&, int, int));
}


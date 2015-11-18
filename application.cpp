#include "application.h"

//#define __rpi__

class Application::Internal
{
public:
    int idStandbyWindow;
    int idUpnpWindow;
    int idRadioWindow;
    int idSpdifWindow;
    int idMenuWindow;
};

Application::Application(QApplication* qapp, int n_files, QObject *parent)
    :QObject(parent), m_initialized(false), m_hasOHRenderer(false), m_app(qapp),  m_settings(0), m_avto(0),
      m_rdco(0), m_mpdradio(0)
{
    m = new Internal;
#ifdef __rpi__
    rpiGPIO = new RPiGPIO();
#endif
    if(QFontDatabase::addApplicationFont(":/fonts/resources/fonts/LiberationSans-Regular.ttf") == -1)
    {
        qDebug() << "Fehler beim Laden von LiberationSans";
    }
    if(QFontDatabase::addApplicationFont(":/fonts/resources/fonts/NotoSans-Regular.ttf") == -1)
    {
        qDebug() << "Fehler beim Laden von NotoSans";
    }
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/LiberationSans-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/NotoSans-Bold.ttf");

    m_settings = new Settings();

    m_mainWindow = new MainWindow();
    m_standbyWindow = new StandbyWindow();
    m_upnpWindow = new UpnpWindow();
    m_radioWindow = new RadioWindow();
    m_spdifWindow = new SpdifWindow();

    m->idStandbyWindow = m_mainWindow->addWidget(m_standbyWindow);
    m->idUpnpWindow = m_mainWindow->addWidget(m_upnpWindow);
    m->idRadioWindow = m_mainWindow->addWidget(m_radioWindow);
    m->idSpdifWindow = m_mainWindow->addWidget(m_spdifWindow);

    setStandby();
    m_mainWindow->show();  

    netAPIServer = new NetAPIServer(this);

    if (!netAPIServer->listen(QHostAddress::Any, 8000)) {
        qDebug() << "Unable to start the server: " << netAPIServer->errorString();
    }

    //nur GUI
//    m_initialized = true;
//    return;

    m_settings = CSettingsStorage::getInstance();
    QString version = getVersion();
    m_settings->setVersion(version);

//    string name = "LinuxUpMpd";
//    string ohname = "LinuxUpMpd (OpenHome)";
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

//    if (RDCH foo = renderer->rdc()) {;
    if (renderer->rdc()) {
//        long foo1 = foo.use_count();
//        cerr << "!!! std::shared_ptr.use_count() = " << foo1 << endl;
//        cerr << "!!! std::shared_ptr = " << foo << endl;
//        foo.reset();
        m_rdco = new RenderingControlQO(renderer->rdc());
    } else {
        cerr << "Device has no RenderingControl" << endl;
        m_initialized = false;
        return;
    }

//    m_rdco = new RenderingControlQO(renderer->rdc());

    cerr << "!!! RenderingControlQO erzeugt." << endl;

    if (ohrenderer->ohvl()) {
        m_ohvlo =  new OHVolumeQO(ohrenderer->ohvl());
    } else {
        cerr << "Device has no OHVolumeControl" << endl;
        m_initialized = false;
        return;
    }

//    m_ohvlo =  new OHVolumeQO(ohrenderer->ohvl());

    cerr << "!!! OHVolumeControl erzeugt." << endl;

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

//    m_avto = new AVTPlayer(renderer->avt());

    cerr << "!!! PlaylistAVT erzeugt." << endl;

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
        connect(m_ohplo, SIGNAL(audioStateChanged(int,const char*)), m_upnpWindow, SLOT(new_transport_state(int,const char*)));
        connect(m_ohplo, SIGNAL(audioStateChanged(int,const char*)), this, SLOT(new_transport_state(int,const char*)));
        connect(m_ohtmo, SIGNAL(secondsChanged(quint32)), m_upnpWindow, SLOT(setCurrentPosition(quint32)));
        connect(netAPIServer, SIGNAL(play()), m_ohplo, SLOT(play()));
        connect(netAPIServer, SIGNAL(pause()), m_ohplo, SLOT(pause()));
        connect(netAPIServer, SIGNAL(next()), m_ohplo, SLOT(next()));
        connect(netAPIServer, SIGNAL(previous()), m_ohplo, SLOT(previous()));
        connect(netAPIServer, SIGNAL(stop()), m_ohplo, SLOT(stop()));
        //        CONNECT(m_upnpWindow, sig_clear_playlist(),
//                m_ohplo, clear());
//        CONNECT(m_upnpWindow, sig_insert_tracks(const MetaDataList&, int),
//                m_ohplo, insertTracks(const MetaDataList&, int));
        connect(m_upnpWindow, SIGNAL(sig_playradio()), m_mpdradio, SLOT(play_radio()));
//        m_app->connect(m_upnpWindow, SIGNAL(sig_insert_tracks(const MetaDataList&,int)),
//                       m_ohplo, SLOT(insertTracks(const MetaDataList&,int)));
    } else {
        connect(m_avto, SIGNAL(audioStateChanged(int,const char*)), m_upnpWindow, SLOT(new_transport_state(int,const char*)));
        connect(m_avto, SIGNAL(audioStateChanged(int, const char*)), this, SLOT(new_transport_state(int, const char *)));
        connect(netAPIServer, SIGNAL(play()), m_avto, SLOT(play()));
        connect(netAPIServer, SIGNAL(stop()), m_avto, SLOT(stop()));
        connect(netAPIServer, SIGNAL(pause()), m_avto, SLOT(pause()));
        connect(m_avto, SIGNAL(secsInSongChanged(quint32)), m_upnpWindow, SLOT(setCurrentPosition(quint32)));
        connect(m_upnpWindow, SIGNAL(sig_radio(const MetaData&, int, bool)), m_avto, SLOT(changeTrack(const MetaData&, int, bool)));
    }

    connect(m_avto, SIGNAL(sig_currentMetadata(const MetaData&)), m_upnpWindow, SLOT(update_track(const MetaData&)));
    //TODO: Wird Volume und Mute benötigt?
    //    CONNECT(m_rdco, volumeChanged(int), m_upnpWindow, setVolumeUi(int));
    //    CONNECT(m_rdco, muteChanged(bool), m_upnpWindow, setMuteUi(bool));
        // Set up the initial volume from the renderer value
    //    m_upnpWindow->setVolumeUi(m_rdco->volume());



    //CONNECT(m_avto, stoppedAtEOT(), m_playlist, psl_forward());

//    CONNECT(m_playlist, sig_stop(),  m_avto, stop());
//    CONNECT(m_playlist, sig_resume_play(), m_avto, play());
//    CONNECT(m_playlist, sig_pause(), m_avto, pause());



    //TODO: Klasse RC5 und HWButton erzeugen und mit slots verknüpfen
//    CONNECT(m_upnpWindow, play(), m_playlist, psl_play());
//    CONNECT(m_upnpWindow, pause(), m_playlist, psl_pause());
//    CONNECT(m_upnpWindow, stop(), m_playlist, psl_stop());
//    CONNECT(m_upnpWindow, forward(), m_playlist, psl_forward());
//    CONNECT(m_upnpWindow, backward(), m_playlist, psl_backward());
//    CONNECT(m_upnpWindow, sig_load_playlist(), m_playlist, psl_load_playlist());
//    CONNECT(m_upnpWindow, sig_save_playlist(), m_playlist, psl_save_playlist());
//    CONNECT(m_upnpWindow, sig_volume_changed(int), m_rdco, setVolume(int));
//    CONNECT(m_upnpWindow, sig_mute(bool), m_rdco, setMute(bool));



//    CONNECT(m_playlist, sig_track_metadata(const MetaData&),
//            m_upnpWindow, update_track(const MetaData&));
//    CONNECT(m_playlist, sig_stopped(),  m_upnpWindow, stopped());
//    CONNECT(m_playlist, sig_paused(),  m_upnpWindow, paused());
//    CONNECT(m_playlist, sig_playing(),  m_upnpWindow, playing());

//    CONNECT(m_playlist, sig_playing_track_changed(int),
//            m_ui_playlist, track_changed(int));
//    CONNECT(m_playlist, sig_playlist_updated(MetaDataList&, int, int),
//            m_ui_playlist, fillPlaylist(MetaDataList&, int, int));
}

void Application::setMode(int mode)
{
    m_mainWindow->setCurrentIndex(mode);
#ifdef __rpi__
    if(mode == m->idSpdifWindow)
        rpiGPIO->setInputSelect(INPUT_DAC);
    else
        rpiGPIO->setInputSelect(INPUT_UPNP);
    rpiGPIO->setRelais(REL_ON);
    rpiGPIO->setLED(LED_ON);
    rpiGPIO->setBacklight(BACKLIGHT_MAX);
#endif
}

void Application::setDACInput(int value)
{
    m_mainWindow->setCurrentIndex(m->idSpdifWindow);
#ifdef __rpi__
    rpiGPIO->setInputSelect(value);
#endif
}

void Application::setSPDIFInput(int value)
{
    m_mainWindow->setCurrentIndex(0);
#ifdef __rpi__
    rpiGPIO->setCS8416InputSelect(value);
#endif
}

void Application::setStandby()
{
    m_mainWindow->setCurrentIndex(0);
#ifdef __rpi__
    rpiGPIO->setRelais(REL_OFF);
    rpiGPIO->setBacklight(BACKLIGHT_DIMM);
    rpiGPIO->setLED(LED_OFF);
#endif
}

void Application::setBacklight(int value)
{
    if(value <= 255 && value >= 0)
    {
#ifdef __rpi__
        rpiGPIO->setBacklight(value);
#endif
    }
}

void Application::new_transport_state(int tps, const char *)
{
    //m_tpstate = tps;
    switch (tps) {
    case AUDIO_UNKNOWN:
    case AUDIO_STOPPED:
    default:
        break;
    case AUDIO_PLAYING:
        setMode(m->idUpnpWindow);
        break;
    case AUDIO_PAUSED:
        break;
    }
}

void Application::setRadioStation(QString name, QString file, int id)
{
    m_settings->setRadioStation(id, name, file);
}



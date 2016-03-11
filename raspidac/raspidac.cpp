#include "raspidac.h"
#include "GUI/mainwindow.h"
#include "upplay/application.h"
#include "rpicontrol/netapiserver.h"
#include "rpi_playlist.h"
#include "GUI/menu.h"

RaspiDAC::RaspiDAC(Application *upapp, QWidget *parent) :
    QWidget(parent), m_upapp(upapp), m_playlist(0), m_initialized(false),
    m_sendIRKey(true)
{
    m_port = CSettingsStorage::getInstance()->getTcpPort();
#ifdef __rpi__
    rpiGPIO = new RPiGPIO();
#endif
    m_rendererName = RENDERER_NAME;
    m_playmode = RPI_Stop;
    m_window = new MainWindow();
    m_window->setCurrentIndex(RPI_Standby);
    m_window->clearTrack();

    m_lastMode = RPI_Radio;

    m_menu = new Menu(m_window);
    m_menu->setInputList();

    m_lirc = new LircControl();

    m_spdifInput = 0;
    m_shutdownPending = false;

    netAPIServer = new NetAPIServer(this);

    if (!netAPIServer->listen(QHostAddress::Any, m_port)) {
        qDebug() << "Unable to start the server: " << netAPIServer->errorString();
    }
    connect(m_menu, SIGNAL(guiSelected(RaspiDAC::GUIMode)),
            this, SLOT(setGUIMode(RaspiDAC::GUIMode)));
    connect(m_menu, SIGNAL(inputSelected(int)),
            this, SLOT(setSPDIFInput(int)));
    connect(m_menu, SIGNAL(radioRowSelected(int)),
            this, SLOT(setRadio(int)));
    connect(this, SIGNAL(datagramm(UDPDatagram&)),
            netAPIServer, SLOT(sendDatagramm(UDPDatagram&)));
    connect(m_window, SIGNAL(messageWindowClosed()),
            this, SLOT(onMsgWinClosed()));
#ifdef __rpi__
    connect(rpiGPIO, SIGNAL(taster(int)), this, SLOT(onTaster(int)));
    connect(rpiGPIO, SIGNAL(tasterZweitbelegung(int)),
            this, SLOT(onTasterZweitbelegung(int)));
#endif
    connect(m_lirc, &LircControl::play, this, &RaspiDAC::play);
    connect(m_lirc, &LircControl::pause, this, &RaspiDAC::pause);
    connect(m_lirc, &LircControl::stop, this, &RaspiDAC::stop);
    connect(m_lirc, &LircControl::key, this, &RaspiDAC::setRadio);
    connect(m_lirc, &LircControl::next, this, &RaspiDAC::forward);
    connect(m_lirc, &LircControl::previous, this, &RaspiDAC::backward);
    connect(m_lirc, &LircControl::mode, this, &RaspiDAC::toggleSPDIFInput);
    connect(m_lirc, &LircControl::powerOn, this, &RaspiDAC::powerOn);
    connect(m_lirc, &LircControl::powerOff, this, &RaspiDAC::powerOff);
    connect(this, &RaspiDAC::sendIRKey, m_lirc, &LircControl::sendCode);
}

RaspiDAC::~RaspiDAC()
{

}

void RaspiDAC::setDACInput(int value)
{
#ifdef __rpi__
    rpiGPIO->setInputSelect(value);
#endif
}

void RaspiDAC::setSPDIFInput(int value)
{
    m_spdifInput = value;
    setGUIMode(RPI_Spdif);

#ifdef __rpi__
    rpiGPIO->setCS8416InputSelect(value);
#endif
    m_window->input(QString("Input %1").arg(m_spdifInput + 1));
    prepareDatagram();
}

void RaspiDAC::toggleSPDIFInput()
{
    if (getGUIMode() == RPI_Spdif)
    {
        if (m_spdifInput == 3)
        {
            setSPDIFInput(0);
        } else {
            setSPDIFInput(m_spdifInput + 1);
        }
    }
    else
    {
        setSPDIFInput(m_spdifInput);
    }
}

void RaspiDAC::setBacklight(int value)
{
    if(value <= 255 && value >= 0)
    {
#ifdef __rpi__
        rpiGPIO->setBacklight(value);
#endif
    }
}

void RaspiDAC::setRadio(int row)
{
    qDebug() << "RaspiDAC::setRadio: SourceType" << getSourceType();

    if (getSourceType() == OHProductQO::OHPR_SourceRadio) {
        qDebug() << "***RaspiDAC::setRadio: emit m_playlist->row_activated(row)";
        emit m_playlist->row_activated(row);
    }
    else {
        m_playlist->setPlayRowPending(row);        
        if (getGUIMode() == RaspiDAC::RPI_Radio)
        {
            qDebug() << "***RaspiDAC::setRadio: emit sig_choose_source(QString(\"Radio\"))";
            emit sig_choose_source(QString("Radio"));
        }
    }
    setGUIMode(RPI_Radio);
}

void RaspiDAC::onTaster(int taster)
{
    if ((taster == 1) && m_shutdownPending)
    {
        shutdownDevice();
        return;
    }
    else if ((taster == 2) && m_shutdownPending)
    {
        restartDevice();
        return;
    }
    else if (taster == 2 && getGUIMode() != RPI_Standby) //MENU
    {
        m_menu->btnMenuPressed();
    }
    else if (taster == 1 && getGUIMode() != RPI_Standby) //PLAY/PAUSE/SELECT
    {
        if (m_menu->isHidden()) {
            if (m_playmode == RPI_Play)
            {
                qDebug() << "***RaspiDAC::onTaster: emit pause()";
                emit pause();
            }
            else
            {
                qDebug() << "***RaspiDAC::onTaster: emit play()";
                emit play();
            }
        }
        else {
            m_menu->btnSelectPressed();
        }
    }
    else if (taster == 3) //POWER
    {
        if (!m_initialized)
        {
            QString msg = QString("Initialisierung fehlgeschlagen.\n\n"
                                  "Keine Verbindung zum Renderer: %1.")
                    .arg(m_rendererName);
            m_window->showMessage(msg);
            return;
        }
        if (getGUIMode() == RPI_Standby)
        {
            setGUIMode(m_lastMode);
        }
        else
        {
            m_lastMode = (GUIMode)m_window->currentIndex();
            setGUIMode(RPI_Standby);
        }
    }
}

void RaspiDAC::powerOn()
{
    if (!m_initialized)
    {
        QString msg = QString("Initialisierung fehlgeschlagen.\n\n"
                              "Keine Verbindung zum Renderer: %1.")
                .arg(m_rendererName);
        m_window->showMessage(msg);
        return;
    }
    if (getGUIMode() == RPI_Standby)
    {
        m_sendIRKey = false;
        setGUIMode(m_lastMode);
    }
}

void RaspiDAC::powerOff()
{
    if (getGUIMode() != RPI_Standby)
    {
        m_sendIRKey = false;
        m_lastMode = (GUIMode)m_window->currentIndex();
        setGUIMode(RPI_Standby);
    }
}

void RaspiDAC::onTasterZweitbelegung(int taster)
{
    if (taster == 1) //PLAY/PAUSE/SELECT
    {

    }
    else if (taster == 2) //MENU
    {
        m_menu->hideMenu();
        QString msg = QString("Soll die Software beendet\n"
                              "und das Gerät runtergefahren werden?\n\n"
                              "Für Shutdown bitte PLAY/PAUSE/SEL drücken!\n\n"
                              "Für Neustart bitte MENU drücken!");
        m_window->showMessage(msg);
        m_shutdownPending = true;
    }
}


void RaspiDAC::setGUIMode(RaspiDAC::GUIMode mode)
{
    qDebug() << "***RaspiDAC::setGUIMode: Set to Mode " << mode;
    if(mode == m_window->currentIndex())
    {
        return;
    }

    if((mode != RPI_Standby) &&
            (m_window->currentIndex() == RPI_Standby))
    {
        // Geräte einschalten
        if (m_sendIRKey)
            emit sendIRKey(LircControl::cmd_sysPowerOn);
#ifdef __rpi__
        rpiGPIO->setRelais(REL_ON);
        rpiGPIO->setLED(LED_ON);
        rpiGPIO->setBacklight(BACKLIGHT_MAX);
        rpiGPIO->setCS8416InputSelect(m_spdifInput);
#endif
    }

    if(m_window->currentIndex() == RPI_Radio)
    {
//        qDebug() << "***RaspiDAC::setGUIMode: emit pause()";
//        emit pause();
    }

    if(m_window->currentIndex() == RPI_Upnp)
    {
//        qDebug() << "***RaspiDAC::setGUIMode: emit pause()";
//        emit pause();
    }

    if(mode == RPI_Radio)
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_UPNP);
#endif
        qDebug() << "***RaspiDAC::setGUIMode: emit sig_choose_source(QString(\"Radio\"))";
        emit sig_choose_source(QString("Radio"));
        //m_window->clearTrack();
    }

    if(mode == RPI_Upnp)
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_UPNP);
#endif
        qDebug() << "***RaspiDAC::setGUIMode: emit sig_choose_source(QString(\"Playlist\"))";
        emit sig_choose_source(QString("Playlist"));
        //m_window->clearTrack();
        //m_window->forceUpdateTrackUpnp(m_MetaData);
    }

    if(mode == RPI_Spdif)
    {
        qDebug() << "***RaspiDAC::setGUIMode: emit pause()";
        emit pause();
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_DAC);
#endif
        m_window->input(QString("Input %1").arg(m_spdifInput + 1));
    }

    if(mode == RPI_Standby)
    {
        // Geräte einschalten
        qDebug() << "***RaspiDAC::setGUIMode: emit pause()";
        emit pause();
#ifdef __rpi__
        rpiGPIO->setRelais(REL_OFF);
        rpiGPIO->setBacklight(BACKLIGHT_DIMM);
        rpiGPIO->setLED(LED_OFF);
#endif
        if (m_sendIRKey)
            emit sendIRKey(LircControl::cmd_sysPowerOff);
    }
    m_window->setCurrentIndex(mode);
    prepareDatagram();
    m_sendIRKey = true;
}

void RaspiDAC::really_close(bool)
{

}

void RaspiDAC::setVolume(int)
{

}

void RaspiDAC::update_track(const MetaData &in)
{
    qDebug() << "RaspiDAC::update_track: MetaData geändert: " << in.title << " " << in.artist;
    m_MetaData = in;
    if ((getSourceType() == OHProductQO::OHPR_SourceRadio && m_playmode == RPI_Play) ||
            getSourceType() == OHProductQO::OHPR_SourcePlaylist)
    {
        m_window->updateTrack(in);
        prepareDatagram(true);
    }
}

void RaspiDAC::applySavedMetaData()
{
    m_window->updateTrack(m_MetaData);
}

void RaspiDAC::setCurrentPosition(quint32 pos_sec)
{
    m_window->upnp_setCurrentPosition(pos_sec);
}

void RaspiDAC::stopped()
{
    qDebug() << "+++RaspiDAC::stopped()";
    m_playmode = RPI_Stop;
    m_window->stopped();
    prepareDatagram();
}

void RaspiDAC::playing()
{
    qDebug() << "+++RaspiDAC::playing()";
    if (getSourceType() == OHProductQO::OHPR_SourceRadio)
        setGUIMode(RPI_Radio);
    if (getSourceType() == OHProductQO::OHPR_SourcePlaylist)
        setGUIMode(RPI_Upnp);

    m_playmode = RPI_Play;
    applySavedMetaData();
    m_window->playing();
    prepareDatagram();
}

void RaspiDAC::paused()
{
    qDebug() << "+++RaspiDAC::paused()";
    m_playmode = RPI_Pause;
    m_window->paused();
    prepareDatagram();
}

void RaspiDAC::setVolumeUi(int)
{

}

void RaspiDAC::setMuteUi(bool)
{

}

void RaspiDAC::enableSourceSelect(bool)
{
    qDebug() << "RaspiDAC::enableSourceSelect: Connected to " << getRendererByName();
    if (getRendererByName().contains(m_rendererName))
    {
        m_initialized = true;
    }
}

void RaspiDAC::setPlaylist(Rpi_Playlist *)
{
    MetaData md;
    m_upapp->getIdleMeta(&md);

    //qDebug() << "RaspiDAC::setPlaylist :" << md.artist;
    qDebug() << "***RaspiDAC::setPlaylist: emit stop()";
    emit stop();
    qDebug() << "***RaspiDAC::setPlaylist: emit sig_choose_source(QString(\"Radio\"))";
    emit sig_choose_source(QString("Radio"));
}

OHProductQO::SourceType RaspiDAC::getSourceType()
{
    MetaData md;
    m_upapp->getIdleMeta(&md);
    if (md.artist.contains("(Radio)"))
        return OHProductQO::OHPR_SourceRadio;
    if (md.artist.contains("(Playlist)"))
        return OHProductQO::OHPR_SourcePlaylist;

    return OHProductQO::OHPR_SourceUnknown;
}

QString RaspiDAC::getRendererByName()
{
    MetaData md;
    m_upapp->getIdleMeta(&md);
    if (md.artist.contains("No renderer connected"))
    {
        return QString("");
    }
    else
    {
        QString name = md.artist;
        name.remove("Renderer: ");
       int end = name.indexOf("(");
       if (end == -1)
       {
           return name;
       }
       else
       {
           name.chop(name.size() - end);
           return name;
       }
    }
}

void RaspiDAC::setLibraryWidget(QWidget *w)
{
     m_librarywidget = w;
}

void RaspiDAC::setPlaylistWidget(Rpi_Playlist *playlist)
{
    m_playlist = playlist;
    connect(m_playlist, SIGNAL(radioListChanged(const QStringList&)),
            m_menu, SLOT(setRadioList(const QStringList&)));
}

QWidget* RaspiDAC::getParentOfLibrary()
{
    return m_window;
}

QWidget* RaspiDAC::getParentOfPlaylist()
{
    return this;
}

void RaspiDAC::ui_loaded()
{
    if (!getRendererByName().contains(m_rendererName))
    {
        qDebug() << "***RaspiDAC::ui_loaded: chooseRenderer";
        m_upapp->chooseRenderer();
    }
}

void RaspiDAC::setStyle(int)
{

}

void RaspiDAC::show()
{
    m_window->show();
}

void RaspiDAC::prepareDatagram(bool metadatahaschanged, bool radiolisthaschanged)
{
    UDPDatagram dtg;
    dtg.playMode = m_playmode;
    dtg.guiMode = getGUIMode();
    dtg.metaDataHasChanged = metadatahaschanged;
    dtg.spdifInput = m_spdifInput;
    dtg.radioListHasChanged = radiolisthaschanged;
    dtg.port = (int)m_port;

    emit datagramm(dtg);
}

RaspiDAC::GUIMode RaspiDAC::getGUIMode()
{
    return (GUIMode)m_window->currentIndex();
}

MetaData& RaspiDAC::getMetaData()
{
    return m_MetaData;
}

QStringList* RaspiDAC::getRadioList()
{
    return m_playlist->getRadioList();
}

void RaspiDAC::onMsgWinClosed()
{
    m_shutdownPending = false;
}

void RaspiDAC::shutdownDevice()
{
    setGUIMode(RaspiDAC::RPI_Standby);
    QString msg = QString("System shutdown!\n\n"
                          "Bitte den Netzstecker erst ziehen,\n"
                          "wenn POWER-LED wieder leuchtet.");
    m_window->showMessage(msg, 0);
    Delay::msleep(3000);
#ifdef __rpi__
    system("shutdown -h now");
#endif
}

void RaspiDAC::restartDevice()
{
    setGUIMode(RaspiDAC::RPI_Standby);
    QString msg = QString("Systemstart wird ausgeführt!\n");
    m_window->showMessage(msg, 0);
    Delay::msleep(3000);
#ifdef __rpi__
    system("shutdown -r now");
#endif
}




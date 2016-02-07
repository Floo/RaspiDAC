#include "raspidac.h"
#include "GUI/mainwindow.h"
#include "application.h"
#include "rpicontrol/netapiserver.h"
#include "rpi_playlist.h"
#include "GUI/menu.h"

RaspiDAC::RaspiDAC(Application *upapp, QWidget *parent) :
    QWidget(parent), m_upapp(upapp), m_playlist(0)
{
#ifdef __rpi__
    rpiGPIO = new RPiGPIO();
#endif
    m_playmode = RPI_Stop;
    m_window = new MainWindow();
    m_window->setCurrentIndex(RPI_Standby);
    m_window->clearTrack();

    m_lastMode = RPI_Upnp;

    m_menu = new Menu(m_window);
    m_menu->setInputList();

    m_spdifInput = 0;

    netAPIServer = new NetAPIServer(this);

    if (!netAPIServer->listen(QHostAddress::Any, 8000)) {
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
    prepareDatagram();
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
    setGUIMode(RPI_Radio);
    if (m_playlist->sourceType() == OHProductQO::OHPR_SourceRadio) {
        emit m_playlist->row_activated(row);
    }
    else {
        m_playlist->setPlayRowPending(row);
    }
}

void RaspiDAC::onTaster(int taster)
{
    if (taster == 2 && m_window->currentIndex() != RPI_Standby) //MENU
    {
        m_menu->btnMenuPressed();
    }
    else if (taster == 1 && m_window->currentIndex() != RPI_Standby) //PLAY/PAUSE/SELECT
    {
        if (m_menu->isHidden()) {
            if (m_playmode == RPI_Play)
                emit pause();
            else
                emit play();
        }
        else {
            m_menu->btnSelectPressed();
        }
    } else if (taster == 3) //POWER
    {
        if (m_window->currentIndex() == RPI_Standby)
        {
            setGUIMode(m_lastMode);
        }
        else
        {
            emit stop();
            m_lastMode = (GUIMode)m_window->currentIndex();
            setGUIMode(RPI_Standby);
        }
    }
}


void RaspiDAC::setGUIMode(RaspiDAC::GUIMode mode)
{
    qDebug() << "RaspiDAC::setGUIMode: Set to Mode " << mode;
    if(mode == m_window->currentIndex())
    {
        return;
    }

    if((mode != RPI_Standby) &&
            (m_window->currentIndex() == RPI_Standby))
    {
#ifdef __rpi__
        rpiGPIO->setRelais(REL_ON);
        rpiGPIO->setLED(LED_ON);
        rpiGPIO->setBacklight(BACKLIGHT_MAX);
        rpiGPIO->setCS8416InputSelect(m_spdifInput);
#endif
    }

    if(m_window->currentIndex() == RPI_Radio)
    {

    }

    if(m_window->currentIndex() == RPI_Upnp)
    {
        emit pause();
    }

    if(mode == RPI_Radio)
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_UPNP);
#endif
        emit sig_choose_source(QString("Radio"));
        //m_window->clearTrack();
    }

    if(mode == RPI_Upnp)
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_UPNP);
#endif
        emit sig_choose_source(QString("Playlist"));
        //m_window->clearTrack();
    }

    if(mode == RPI_Spdif)
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_DAC);
#endif
        m_window->input(QString("Input %1").arg(m_spdifInput + 1));
    }

    if(mode == RPI_Standby)
    {
#ifdef __rpi__
        rpiGPIO->setRelais(REL_OFF);
        rpiGPIO->setBacklight(BACKLIGHT_DIMM);
        rpiGPIO->setLED(LED_OFF);
#endif
    }
    m_window->setCurrentIndex(mode);
}

void RaspiDAC::really_close(bool value)
{

}

void RaspiDAC::setVolume(int vol)
{

}

void RaspiDAC::update_track(const MetaData &in)
{
    qDebug() << "RaspiDAC::update_track: MetaData geändert: " << in.title;
    m_window->updateTrack(in);
    m_MetaData = in;
    prepareDatagram(true);
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
    m_playmode = RPI_Stop;
    m_window->stopped();
    prepareDatagram();
}

void RaspiDAC::playing()
{
    if (getGUIMode() == RPI_Standby)
    {
        if (m_playlist) //Playlist bereits initialisiert
        {
        if (m_playlist->sourceType() == OHProductQO::OHPR_SourceRadio)
            setGUIMode(RPI_Radio);
        if (m_playlist->sourceType() == OHProductQO::OHPR_SourcePlaylist)
            setGUIMode(RPI_Upnp);
        }
        else //versuche aus MetaData zu erkennen, ob Radio oder Playlist
        {
            if (m_MetaData.length_ms > 0)
                setGUIMode(RPI_Upnp);
            else
                setGUIMode(RPI_Radio);
        }
        applySavedMetaData();
    }
    m_playmode = RPI_Play;
    m_window->playing();
    prepareDatagram();
}

void RaspiDAC::paused()
{
    m_playmode = RPI_Pause;
    m_window->paused();
    prepareDatagram();
}

void RaspiDAC::setVolumeUi(int volume_percent)
{

}

void RaspiDAC::setMuteUi(bool value)
{

}

void RaspiDAC::enableSourceSelect(bool value)
{

}

void RaspiDAC::setPlaylist(Rpi_Playlist *playlist)
{
    m_playlist = playlist;
    connect(m_playlist, SIGNAL(radioListChanged(const QStringList&)),
            netAPIServer, SLOT(radioList(const QStringList&)));
    connect(m_playlist, SIGNAL(radioListChanged(const QStringList&)),
            m_menu, SLOT(setRadioList(const QStringList&)));
    connect(m_playlist, SIGNAL(sig_source_type_changed(OHProductQO::SourceType)),
            this, SLOT(onChangedSourceType(OHProductQO::SourceType)));
    qDebug() << "RaspiDAC::setPlaylist";
}

void RaspiDAC::setLibraryWidget(QWidget *w)
{
     m_librarywidget = w;
}

void RaspiDAC::setPlaylistWidget(QWidget *w)
{
    m_playlistwidget = w;
}

QWidget* RaspiDAC::getParentOfLibrary()
{
    return m_window;
}

QWidget* RaspiDAC::getParentOfPlaylist()
{
    return m_window;
}

void RaspiDAC::ui_loaded()
{

}

void RaspiDAC::setStyle(int value)
{

}

void RaspiDAC::show()
{
    m_window->show();
}

void RaspiDAC::prepareDatagram(bool metadatahaschanged)
{
    UDPDatagram dtg;
    dtg.playMode = m_playmode;
    dtg.guiMode = getGUIMode();
    dtg.metaDataHasChanged = metadatahaschanged;
    dtg.spdifInput = m_spdifInput;

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

void RaspiDAC::onChangedSourceType(OHProductQO::SourceType st)
{
    if (st == OHProductQO::OHPR_SourcePlaylist)
        setGUIMode(RaspiDAC::RPI_Upnp);
    if (st == OHProductQO::OHPR_SourceRadio)
        setGUIMode(RaspiDAC::RPI_Radio);
}


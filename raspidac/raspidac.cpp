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
    m_rendererName = RENDERER_NAME;
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
#ifdef __rpi__
    connect(rpiGPIO, SIGNAL(taster(int)), this, SLOT(onTaster(int)));
#endif
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
    if (getSourceType() == OHProductQO::OHPR_SourceRadio) {
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
    prepareDatagram();
}

void RaspiDAC::really_close(bool)
{

}

void RaspiDAC::setVolume(int)
{

}

void RaspiDAC::update_track(const MetaData &in)
{
    qDebug() << "RaspiDAC::update_track: MetaData geändert: " << in.title;
    if ((getSourceType() == OHProductQO::OHPR_SourceRadio && m_playmode == RPI_Play) ||
            getSourceType() == OHProductQO::OHPR_SourcePlaylist)
    {
        m_window->updateTrack(in);
        m_MetaData = in;
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
    m_playmode = RPI_Stop;
    m_window->stopped();
    prepareDatagram();
}

void RaspiDAC::playing()
{
    if (getSourceType() == OHProductQO::OHPR_SourceRadio)
        setGUIMode(RPI_Radio);
    if (getSourceType() == OHProductQO::OHPR_SourcePlaylist)
        setGUIMode(RPI_Upnp);
//    if (getGUIMode() == RPI_Standby)
//    {
//        applySavedMetaData();
//    }
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

void RaspiDAC::setVolumeUi(int)
{

}

void RaspiDAC::setMuteUi(bool)
{

}

void RaspiDAC::enableSourceSelect(bool)
{
    qDebug() << "RaspiDAC::enableSourceSelect";
}

void RaspiDAC::setPlaylist(Rpi_Playlist *playlist)
{
    m_playlist = playlist;
    connect(m_playlist, SIGNAL(radioListChanged(const QStringList&)),
            m_menu, SLOT(setRadioList(const QStringList&)));

    MetaData md;
    m_upapp->getIdleMeta(&md);

    qDebug() << "RaspiDAC::setPlaylist :" << md.artist;

    emit stop();
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
    return this;
}

void RaspiDAC::ui_loaded()
{
    if (!getRendererByName().contains(m_rendererName))
    {
        qDebug() << "RaspiDAC::ui_loaded: chooseRenderer";
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


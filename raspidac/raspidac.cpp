#include "raspidac.h"
#include "GUI/mainwindow.h"
#include "application.h"
#include "rpicontrol/netapiserver.h"
#include "rpi_playlist.h"
#include "GUI/menu.h"

RaspiDAC::RaspiDAC(Application *upapp, QWidget *parent) :
    QWidget(parent), m_upapp(upapp)
{
#ifdef __rpi__
    rpiGPIO = new RPiGPIO();
#endif
    m_playmode = RPI_Stop;
    m_window = new MainWindow();
    m_window->setCurrentIndex(RPI_Standby);

    m_lastMode = RPI_Upnp;

    m_menu = new Menu(m_window);
    m_menu->setInputList();

    emit GUIModeChanged(RPI_Standby);
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
    setGUIMode(RPI_Spdif);
    m_window->input(QString("Input %1").arg(value + 1));
#ifdef __rpi__
    rpiGPIO->setCS8416InputSelect(value);
#endif
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
    }

    if(mode == RPI_Upnp)
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_UPNP);
#endif
        emit sig_choose_source(QString("Playlist"));
    }

    if(mode == RPI_Spdif)
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_DAC);
#endif
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
    emit GUIModeChanged(mode);
}

void RaspiDAC::really_close(bool value)
{

}

void RaspiDAC::setVolume(int vol)
{

}

void RaspiDAC::update_track(const MetaData &in)
{
    m_window->updateTrack(in);
}

void RaspiDAC::setCurrentPosition(quint32 pos_sec)
{
    m_window->upnp_setCurrentPosition(pos_sec);
}

void RaspiDAC::stopped()
{
    m_playmode = RPI_Stop;
    m_window->stopped();
}

void RaspiDAC::playing()
{
    m_playmode = RPI_Play;
    m_window->playing();
}

void RaspiDAC::paused()
{
    m_playmode = RPI_Pause;
    m_window->paused();
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


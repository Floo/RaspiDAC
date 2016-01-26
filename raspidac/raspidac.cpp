#include "raspidac.h"

RaspiDAC::RaspiDAC(Application *upapp, QWidget *parent) :
    QWidget(parent), m_upapp(upapp)
{
#ifdef __rpi__
    rpiGPIO = new RPiGPIO();
#endif
    m_window = new MainWindow();
    netAPIServer = new NetAPIServer(this);

    if (!netAPIServer->listen(QHostAddress::Any, 8000)) {
        qDebug() << "Unable to start the server: " << netAPIServer->errorString();
    }
}

RaspiDAC::~RaspiDAC()
{

}

void RaspiDAC::onPlay()
{
    emit play();
}

void RaspiDAC::onPaused()
{
    emit pause();
}

void RaspiDAC::onStopped()
{
    emit stopped();
}
void RaspiDAC::onNext()
{
    emit forward();
}

void RaspiDAC::onPrevious()
{
    emit backward();
}

void RaspiDAC::setDACInput(int value)
{
    setMode(m_window->getIndexByName("spdif"));
#ifdef __rpi__
    rpiGPIO->setInputSelect(value);
#endif
}

void RaspiDAC::setSPDIFInput(int value)
{
    setDACInput(0);
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

void RaspiDAC::setMode(int mode)
{
    if(mode == m_window->currentIndex())
    {
        return;
    }

    if((mode != m_window->getIndexByName("standby")) &&
            (m_window->currentIndex() == m_window->getIndexByName("standby")))
    {
#ifdef __rpi__
        rpiGPIO->setRelais(REL_ON);
        rpiGPIO->setLED(LED_ON);
        rpiGPIO->setBacklight(BACKLIGHT_MAX);
#endif
    }

    if(m_window->currentIndex() == m_window->getIndexByName("radio"))
    {

    }

    if(m_window->currentIndex() == m_window->getIndexByName("upnp"))
    {
        emit pause();
    }

    if(mode == m_window->getIndexByName("radio"))
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_UPNP);
#endif
    }

    if(mode == m_window->getIndexByName("upnp"))
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_UPNP);
#endif
    }

    if(mode == m_window->getIndexByName("spdif"))
    {
#ifdef __rpi__
        rpiGPIO->setInputSelect(INPUT_DAC);
#endif
    }

    if(mode == m_window->getIndexByName("standby"))
    {
#ifdef __rpi__
        rpiGPIO->setRelais(REL_OFF);
        rpiGPIO->setBacklight(BACKLIGHT_DIMM);
        rpiGPIO->setLED(LED_OFF);
#endif
    }

    if(mode == m_window->getIndexByName("menu"))
    {

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
    m_window->upnp_updateTrack(in);
}

void RaspiDAC::setCurrentPosition(quint32 pos_sec)
{
    m_window->upnp_setCurrentPosition(pos_sec);
}

void RaspiDAC::stopped()
{
    m_window->stopped();
}

void RaspiDAC::playing()
{
    setMode(m_window->getIndexByName("upnp"));
    m_window->playing();
}

void RaspiDAC::paused()
{
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

#include "netapiserver.h"
#include "netapithread.h"


NetAPIServer::NetAPIServer(RaspiDAC *rpi_h, QObject *parent)
    : QTcpServer(parent), m_rpi(rpi_h)
{
    qRegisterMetaType<RaspiDAC::GUIMode>("RaspiDAC::GUIMode");
    connect(m_rpi, SIGNAL(GUIModeChanged(RaspiDAC::GUIMode)),
            this, SLOT(GUIMode(RaspiDAC::GUIMode)));
    connect(m_rpi, SIGNAL(radioListChanged(QString)), this, SLOT(radioList(QString)));

}

NetAPIServer::~NetAPIServer()
{
}

void NetAPIServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "Eingehende Verbindung";
    NetAPIThread *thread = new NetAPIThread(socketDescriptor, this); 
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(setDACinput(int)), m_rpi , SLOT(setDACInput(int)));
    connect(thread, SIGNAL(setSPDIFInput(int)), m_rpi, SLOT(setSPDIFInput(int)));
    connect(thread, SIGNAL(setMode(RaspiDAC::GUIMode)),
            m_rpi, SLOT(setGUIMode(RaspiDAC::GUIMode)));
    connect(thread, SIGNAL(setBacklight(int)), m_rpi, SLOT(setBacklight(int)));
    connect(thread, SIGNAL(setPlay()), m_rpi, SLOT(onPlay()));
    connect(thread, SIGNAL(setPause()), m_rpi, SLOT(onPaused()));
    connect(thread, SIGNAL(setNext()), m_rpi, SLOT(onNext()));
    connect(thread, SIGNAL(setPrevious()), m_rpi, SLOT(onPrevious()));
    connect(thread, SIGNAL(setStop()), m_rpi, SLOT(onStopped()));
    connect(thread, SIGNAL(setRadio(int)), m_rpi, SLOT(setRadio(int)));
    connect(thread, SIGNAL(taster(int)), m_rpi, SLOT(onTaster(int)));

    thread->start();
}

void NetAPIServer::GUIMode(RaspiDAC::GUIMode h_mode)
{
    m_GUIMode = h_mode;
}

void NetAPIServer::radioList(const QString &h_list)
{
    m_RadioList = h_list;
}

QString NetAPIServer::getGUIMode()
{
    switch (m_GUIMode) {
    case RaspiDAC::RPI_Standby:
        return "standby";
        break;
    case RaspiDAC::RPI_Upnp:
        return "upnp";
        break;
    case RaspiDAC::RPI_Spdif:
        return "spdif";
        break;
    case RaspiDAC::RPI_Radio:
        return "radio";
        break;
    }
}

QString NetAPIServer::getRadioList()
{
    return m_RadioList;
}



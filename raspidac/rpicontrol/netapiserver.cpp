#include "netapiserver.h"
#include "netapithread.h"


NetAPIServer::NetAPIServer(RaspiDAC *rpi_h, QObject *parent)
    : QTcpServer(parent), m_rpi(rpi_h)
{
    qRegisterMetaType<RaspiDAC::GUIMode>("RaspiDAC::GUIMode");
}

NetAPIServer::~NetAPIServer()
{
}

void NetAPIServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "NetAPIServer::incomingConnection: Eingehende Verbindung";
    NetAPIThread *thread = new NetAPIThread(socketDescriptor, this); 
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(setDACinput(int)), m_rpi , SLOT(setDACInput(int)));
    connect(thread, SIGNAL(setSPDIFInput(int)), m_rpi, SLOT(setSPDIFInput(int)));
    connect(thread, SIGNAL(setMode(RaspiDAC::GUIMode)),
            m_rpi, SLOT(setGUIMode(RaspiDAC::GUIMode)));
    connect(thread, SIGNAL(setBacklight(int)), m_rpi, SLOT(setBacklight(int)));
    connect(thread, SIGNAL(setPlay()), m_rpi, SIGNAL(play()));
    connect(thread, SIGNAL(setPause()), m_rpi, SIGNAL(pause()));
    connect(thread, SIGNAL(setNext()), m_rpi, SIGNAL(forward()));
    connect(thread, SIGNAL(setPrevious()), m_rpi, SIGNAL(backward()));
    connect(thread, SIGNAL(setStop()), m_rpi, SIGNAL(stop()));
    connect(thread, SIGNAL(setRadio(int)), m_rpi, SLOT(setRadio(int)));
    connect(thread, SIGNAL(taster(int)), m_rpi, SLOT(onTaster(int)));

    thread->start();
}


QString NetAPIServer::getGUIMode()
{
    RaspiDAC::GUIMode guimode = m_rpi->getGUIMode();

    switch (guimode) {
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
    QStringList *strlist = 0;
    strlist = m_rpi->getRadioList();
    return strlist->join(";");
}

void NetAPIServer::sendDatagramm(UDPDatagram &dtg)
{
    qDebug() << "NetAPIServer::sendDatagramm";
    m_datagram = dtg;
    QUdpSocket *socket = new QUdpSocket();
    QByteArray bytearray = QString("[RaspiDAC]").toUtf8();
    bytearray.append(dtg.toDatagram().toUtf8());
    socket->writeDatagram(bytearray, QHostAddress::Broadcast, 8001);
    delete socket;
}

QString NetAPIServer::getMetaData()
{
    MetaData& md = m_rpi->getMetaData();
    QString mdStr = md.album + ";"
            + md.artist + ";"
            + md.title + ";"
            + md.albumArtURI + ";";
    return mdStr;
}

QString NetAPIServer::getDatagram()
{
    return m_datagram.toDatagram();
}





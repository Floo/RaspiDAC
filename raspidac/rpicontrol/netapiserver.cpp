#include "netapiserver.h"
#include "netapithread.h"
#include "upplay/HelperStructs/CSettingsStorage.h"


NetAPIServer::NetAPIServer(RaspiDAC *rpi_h, QObject *parent)
    : QTcpServer(parent), m_rpi(rpi_h)
{
    int port = CSettingsStorage::getInstance()->getUdpDiscoveryPort();
    qRegisterMetaType<RaspiDAC::GUIMode>("RaspiDAC::GUIMode");
    qRegisterMetaType<LircControl::sysCommandCode>("LircControl::sysCommandCode");
    qRegisterMetaType<LircControl::commandCode>("LircControl::commandCode");
    m_udpSocket = new QUdpSocket();
    m_udpSocket->bind(port);

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(pendingUDPDatagram()));
}

NetAPIServer::~NetAPIServer()
{
    delete m_udpSocket;
}

void NetAPIServer::incomingConnection(qintptr socketDescriptor)
{
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
    connect(thread, SIGNAL(tasterZweitbelegung(int)), m_rpi, SLOT(onTasterZweitbelegung(int)));
    connect(thread, &NetAPIThread::setPM8000, m_rpi, &RaspiDAC::sendIRKey);

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
    return "";
}

QString NetAPIServer::getRadioList()
{
    QStringList *strlist = 0;
    strlist = m_rpi->getRadioList();
    return strlist->join(";");
}

QString NetAPIServer::getInputList()
{
    QStringList lst = CSettingsStorage::getInstance()->getSpdifInputNames();
    return lst.join(";");
}

void NetAPIServer::sendDatagramm(UDPDatagram &dtg)
{
    int port = CSettingsStorage::getInstance()->getUdpDatagramPort();
    //qDebug() << "NetAPIServer::sendDatagramm";
    m_datagram = dtg;
    QByteArray bytearray = QString("[RaspiDAC]").toUtf8();
    bytearray.append(dtg.toDatagram().toUtf8());
    m_udpSocket->writeDatagram(bytearray, QHostAddress::Broadcast, port);
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

void NetAPIServer::pendingUDPDatagram()
{
    while (m_udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress hostSender;
        quint16 portSender;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size(), &hostSender, &portSender);
        QString str = QString(datagram);
        if (str.contains("[RendererBitteMelden!]"))
        {
            m_datagram.radioListHasChanged = true;
            m_datagram.metaDataHasChanged = true;
            sendDatagramm(m_datagram);
        }

    }
}





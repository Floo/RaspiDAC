#include "netapiserver.h"


NetAPIServer::NetAPIServer(RaspiDAC *rpi_h, QObject *parent)
    : QTcpServer(parent), m_rpi(rpi_h)
{

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
    connect(thread, SIGNAL(setMode(int)), m_rpi, SLOT(setMode(int)));
    connect(thread, SIGNAL(setBacklight(int)), m_rpi, SLOT(setBacklight(int)));
    connect(thread, SIGNAL(setPlay()), m_rpi, SLOT(onPlay()));
    connect(thread, SIGNAL(setPause()), m_rpi, SLOT(onPause()));
    connect(thread, SIGNAL(setNext()), m_rpi, SLOT(onNext()));
    connect(thread, SIGNAL(setPrevious()), m_rpi, SLOT(onPrevious()));
    connect(thread, SIGNAL(setStop()), m_rpi, SLOT(onStop()));
    connect(thread, SIGNAL(setRadioStation(QString,QString,int)), m_rpi, SLOT(setRadioStation(QString,QString,int)));
    connect(thread, SIGNAL(setRadio(int)), this, SLOT(setRadio(int)));
    connect(thread, SIGNAL(taster(int)), this, SLOT(setTaster(int)));

    thread->start();
}


void NetAPIServer::setRadio(int id)
{
    emit radio(id);
}

void NetAPIServer::setTaster(int id)
{
    emit taster(id);
}

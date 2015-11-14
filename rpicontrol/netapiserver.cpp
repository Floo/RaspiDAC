#include "netapiserver.h"


NetAPIServer::NetAPIServer(Application *app_h, QObject *parent)
    : QTcpServer(parent)
{
    app = app_h;
}

NetAPIServer::~NetAPIServer()
{
}

void NetAPIServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "Eingehende Verbindung";
    NetAPIThread *thread = new NetAPIThread(socketDescriptor, this); 
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(setDACinput(int)), app, SLOT(setDACInput(int)));
    connect(thread, SIGNAL(setSPDIFInput(int)), app, SLOT(setSPDIFInput(int)));
    connect(thread, SIGNAL(setMode(int)), app, SLOT(setMode(int)));
    connect(thread, SIGNAL(setBacklight(int)), app, SLOT(setBacklight(int)));
    connect(thread, SIGNAL(setStandby()), app, SLOT(setStandby()));
    connect(thread, SIGNAL(setPlay()), this, SLOT(setPlay()));
    connect(thread, SIGNAL(setPause()), this, SLOT(setPause()));
    connect(thread, SIGNAL(setNext()), this, SLOT(setNext()));
    connect(thread, SIGNAL(setPrevious()), this, SLOT(setPrevious()));
    connect(thread, SIGNAL(setStop()), this, SLOT(setStop()));

    thread->start();
}

void NetAPIServer::setPlay()
{
    emit play();
}

void NetAPIServer::setPause()
{
    emit pause();
}

void NetAPIServer::setNext()
{
    emit next();
}

void NetAPIServer::setPrevious()
{
    emit previous();
}

void NetAPIServer::setStop()
{
    emit stop();
}

#include "lirccontrol.h"
#include "lircreceiver.h"
#include "lircsender.h"

LircControl::LircControl(QObject *parent) : QObject(parent)
{
    m_lircreceiver = new LircReceiver();
    m_lircsender = new LircSender();
    connect(m_lircreceiver, SIGNAL(buttonPressed(char*)), this, SLOT(cmdParser(char*)));
}

LircControl::~LircControl()
{
    delete m_lircreceiver;
}

void LircControl::powerOnPM8000()
{
    m_lircsender->sendKey("KEY_POWER");
}

void LircControl::powerOffPM8000()
{
    m_lircsender->sendKey("KEY_POWER2");
}

void LircControl::cmdParser(char *cmd)
{
    QString str_cmd = QString(cmd);
    if (!str_cmd.compare("PLAY"))
    {
        emit play();
    }
    else if (!str_cmd.compare("PAUSE"))
    {
        emit pause();
    }
    else if (!str_cmd.compare("STOP"))
    {
        emit stop();
    }
}


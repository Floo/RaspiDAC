#include "lirccontrol.h"
#include "lircreceiver.h"
#include "lircsender.h"

LircControl::LircControl(QObject *parent) : QObject(parent)
{
    m_recCmdTable["KEY_1"] =       cmd_key_1;
    m_recCmdTable["KEY_2"] =       cmd_key_2;
    m_recCmdTable["KEY_3"] =       cmd_key_3;
    m_recCmdTable["KEY_4"] =       cmd_key_4;
    m_recCmdTable["KEY_5"] =       cmd_key_5;
    m_recCmdTable["KEY_6"] =       cmd_key_6;
    m_recCmdTable["KEY_7"] =       cmd_key_7;
    m_recCmdTable["KEY_8"] =       cmd_key_8;
    m_recCmdTable["KEY_9"] =       cmd_key_9;
    m_recCmdTable["POWERON"] =     cmd_powerOn;
    m_recCmdTable["POWEROFF"] =    cmd_powerOff;
    m_recCmdTable["PLAY"] =        cmd_play;
    m_recCmdTable["PAUSE"] =       cmd_pause;
    m_recCmdTable["STOP"] =        cmd_stop;
    m_recCmdTable["NEXT"] =        cmd_next;
    m_recCmdTable["PREVIOUS"] =    cmd_previous;
    m_recCmdTable["MUTE"] =        cmd_mute;
    m_recCmdTable["VOLUMEUP"] =    cmd_volumeUp;
    m_recCmdTable["VOLUMEDOWN"] =  cmd_volumeDown;
    m_recCmdTable["MODE"] =        cmd_mode;

    m_sendCmdTable[cmd_key_1] =     "KEY_1";
    m_sendCmdTable[cmd_key_2] =     "KEY_2";
    m_sendCmdTable[cmd_key_3] =     "KEY_3";
    m_sendCmdTable[cmd_key_4] =     "KEY_4";
    m_sendCmdTable[cmd_key_5] =     "KEY_5";
    m_sendCmdTable[cmd_key_6] =     "KEY_6";
    m_sendCmdTable[cmd_key_7] =     "KEY_7";
    m_sendCmdTable[cmd_key_8] =     "KEY_8";
    m_sendCmdTable[cmd_key_9] =     "KEY_9";
    m_sendCmdTable[cmd_powerOn] =   "KEY_POWER";
    m_sendCmdTable[cmd_powerOff] =  "KEY_POWER2";
    m_sendCmdTable[cmd_play] =      "KEY_PLAY";
    m_sendCmdTable[cmd_pause] =     "KEY_PAUSE";
    m_sendCmdTable[cmd_stop] =      "KEY_STOP";
    m_sendCmdTable[cmd_next] =      "KEY_NEXT";
    m_sendCmdTable[cmd_previous] =  "KEY_PREVIOUS";
    m_sendCmdTable[cmd_mute] =      "KEY_MUTE";
    m_sendCmdTable[cmd_volumeUp] =  "KEY_VOLUME_UP";
    m_sendCmdTable[cmd_volumeDown] ="KEY_VOLUME_DOWN";
    m_sendCmdTable[cmd_mode] =      "KEY_MODE";

    m_lircreceiver = new LircReceiver();
    m_lircsender = new LircSender();
    connect(m_lircreceiver, SIGNAL(buttonPressed(char*)), this, SLOT(cmdParser(char*)));
}

LircControl::~LircControl()
{
    delete m_lircreceiver;
}

void LircControl::sendCommand(commandCode cmdcode)
{
    QString cmd = m_sendCmdTable[cmdcode];
    m_lircsender->sendKey(cmd.toStdString().c_str());
}



void LircControl::cmdParser(char *cmd)
{
    commandCode cmdcode = m_recCmdTable[QString(cmd)];

    switch (cmdcode)
    {
    case cmd_play:
        emit play();
        break;
    case cmd_pause:
        emit pause();
        break;
    case cmd_stop:
        emit stop();
        break;
    default:
        ;
    }
}


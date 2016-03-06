#include "lirccontrol.h"
#include "lircdevice.h"

LircControl::LircControl(QObject *parent) : QObject(parent),
  m_lastToggle(true), m_lastCode(-1)
{
    m_sysCmdTable[328449] = cmd_sysPowerOn;
    m_sysCmdTable[328450] = cmd_sysPowerOff;

    m_sysCmdTable[5133] =   cmd_sysMute;
    m_sysCmdTable[5136] =   cmd_sysVolumeUp;
    m_sysCmdTable[5137] =   cmd_sysVolumeDown;

    m_sysCmdTable[5503] =   cmd_sysPhono;
    m_sysCmdTable[5439] =   cmd_sysCD;
    m_sysCmdTable[5247] =   cmd_sysTuner;
    m_sysCmdTable[5631] =   cmd_sysCDR;
    m_sysCmdTable[327686] = cmd_sysAUX1;
    m_sysCmdTable[327687] = cmd_sysAUX2;
    m_sysCmdTable[5311] =   cmd_sysTape;
    m_sysCmdTable[5631] =   cmd_sysMD;

    m_recCmdTable[0] =      cmd_key_0;
    m_recCmdTable[1] =      cmd_key_1;
    m_recCmdTable[2] =      cmd_key_2;
    m_recCmdTable[3] =      cmd_key_3;
    m_recCmdTable[4] =      cmd_key_4;
    m_recCmdTable[5] =      cmd_key_5;
    m_recCmdTable[6] =      cmd_key_6;
    m_recCmdTable[7] =      cmd_key_7;
    m_recCmdTable[8] =      cmd_key_8;
    m_recCmdTable[9] =      cmd_key_9;

    m_recCmdTable[53] =     cmd_play;
    m_recCmdTable[48] =     cmd_pause;
    m_recCmdTable[54] =     cmd_stop;
    m_recCmdTable[32] =     cmd_next;
    m_recCmdTable[33] =     cmd_previous;
    m_recCmdTable[52] =     cmd_forward;
    m_recCmdTable[50] =     cmd_rewind;
    m_recCmdTable[43] =     cmd_mode;
    m_recCmdTable[35] =     cmd_mode;
    m_recCmdTable[37] =     cmd_mode;

    LircDevice *lirc = new LircDevice;
    lirc->moveToThread(&lircThread);
    connect(&lircThread, &QThread::finished, lirc, &QObject::deleteLater);
    connect(&lircThread, &QThread::started, lirc, &LircDevice::initDevice);
    connect(lirc, &LircDevice::codeReceived, this, &LircControl::receiveCode);
    connect(this, &LircControl::irCode, lirc, &LircDevice::sendCode);

    lircThread.start();
}


LircControl::~LircControl()
{
    lircThread.quit();
    lircThread.wait();
}

void LircControl::receiveCode(int code)
{
    bool xRC5 = code & XRC5_MASK;
    bool toggle = xRC5 ? (code & RC5X_TOGGLE_MASK) : (code & RC5_TOGGLE_MASK);

    code = xRC5 ? (code & ~(RC5X_STARTBIT_MASK | RC5X_TOGGLE_MASK | XRC5_MASK)) : (code & ~(RC5_STARTBIT_MASK | RC5_TOGGLE_MASK));

    //qDebug() << "LircControl::receiveCode: code = " << code << ", ADR = " << (code & RC5_ADR_MASK) << ", CMD = " << (code & RC5_CMD_MASK);

    if ((toggle == m_lastToggle) && (code == m_lastCode))
        return;

    m_lastToggle = toggle;
    m_lastCode = code;
    cmdParser(code);
}



void LircControl::cmdParser(int code)
{
    sysCommandCode cmdsyscode = m_sysCmdTable.value(code, cmd_sysUnknown);

    switch(cmdsyscode) {
    case cmd_sysPowerOn:
        emit powerOn();
        break;
    case cmd_sysPowerOff:
        emit powerOff();
        break;
    default:
        break;
    }

    if (cmdsyscode == cmd_sysUnknown)
    {
        commandCode cmdcode = m_recCmdTable.value(code & RC5_CMD_MASK);

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
        case cmd_next:
            emit next();
            break;
        case cmd_previous:
            emit previous();
            break;
        case cmd_mode:
            emit mode();
            break;
        default:
            if ((cmdcode >= cmd_key_1) && (cmdcode <= cmd_key_9))
            {
                emit key((int)(code & 0x00000003) - 1);
            } else if (cmdcode == cmd_key_0){
                emit key(9);
            }
        }
    }
}

void LircControl::sendCode(sysCommandCode cmdCode)
{
    emit irCode(m_sysCmdTable.key(cmdCode, -1));
}


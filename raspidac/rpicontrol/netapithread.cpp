#include "netapithread.h"
#include "netapiserver.h"


NetAPIThread::NetAPIThread(int socketDescriptor, NetAPIServer *parent)
    : QThread(parent), socketDescriptor(socketDescriptor), m_netapiserver(parent)
{
    m_commandList["rc5direct"] =  cmd_rc5direct;
    m_commandList["standby"] =    cmd_standby;
    m_commandList["dacinput"] =   cmd_dacinput;
    m_commandList["mode"] =       cmd_mode;
    m_commandList["pm8000"] =     cmd_pm8000;
    m_commandList["radio"] =      cmd_radio;
    m_commandList["backlight"] =  cmd_backlight;
    m_commandList["spdifinput"] = cmd_spdifinput;
    m_commandList["play"] =       cmd_play;
    m_commandList["pause"] =      cmd_pause;
    m_commandList["next"] =       cmd_next;
    m_commandList["previous"] =   cmd_previous;
    m_commandList["stop"] =       cmd_stop;
    m_commandList["taster"] =     cmd_taster;
    m_commandList["shutdown"] =   cmd_shutdown;
    m_commandList["status"] =     cmd_status;
    m_commandList["radiolist"] =  cmd_radiolist;
    m_commandList["inputlist"] =  cmd_inputlist;
    m_commandList["metadata"] =   cmd_metadata;
}

NetAPIThread::~NetAPIThread()
{
}

void NetAPIThread::run()
{
    QTcpSocket tcpSocket;
    const int Timeout = 5000;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error(), tcpSocket.errorString());
        return;
    }

    while (tcpSocket.bytesAvailable() < (int)sizeof(quint16)) {
        if (!tcpSocket.waitForReadyRead(Timeout)) {
            emit error(tcpSocket.error(), tcpSocket.errorString());
            return;
        }
    }

    quint16 blockSize;
    QDataStream in(&tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    in >> blockSize;

    while (tcpSocket.bytesAvailable() < blockSize) {
        if (!tcpSocket.waitForReadyRead(Timeout)) {
            emit error(tcpSocket.error(), tcpSocket.errorString());
            return;
        }
    }
    QString command;
    in >> command;
    QString reply = parser(command);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << reply;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    tcpSocket.write(block);
    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();
}


QString NetAPIThread::parser(const QString &command)
{
    QString reply = "Error";
    int value;
    commandCode cmdcode;

    qDebug() << "NetAPIThread::parser: " << command;

    QStringList subcommand;
    QStringList tmpList = command.split(QRegExp("\"")); // Split by "
    bool inside = false;
//    subcommand.clear();
    foreach (QString s, tmpList) {
        if (inside) { // If 's' is inside quotes ...
            subcommand.append(s); // ... get the whole string
        } else { // If 's' is outside quotes ...
            subcommand.append(s.split(QRegExp("\\s+"), QString::SkipEmptyParts)); // ... get the splitted string
        }
        inside = !inside;
    }


    if(subcommand.size() < 2)
        return reply;

    cmdcode = m_commandList[subcommand.at(1)];

    if (subcommand.at(0).compare("set", Qt::CaseInsensitive) == 0)
    {       
        switch (cmdcode)
        {
        case cmd_rc5direct: //rc5direct
            if((value = onoff(subcommand.at(2), true)) < 0)
            {
                break;
            }
            else
            {
                emit setRC5direct(value);
                reply = "OK";
            }
            break;
        case cmd_standby: //standby
        {
            emit setMode(RaspiDAC::RPI_Standby);
            reply = "OK";
        }
            break;
        case cmd_dacinput:  //dacinput
            if((value = onoff(subcommand.at(2), true)) < 0)
            {
                break;
            }
            else
            {
                emit setDACinput(value);
                reply = "OK";
            }
            break;
        case cmd_mode: //mode
            RaspiDAC::GUIMode md;
            if((md = mode(subcommand.at(2))) > 3 || mode(subcommand.at(2)) < 1)
            {
                break;
            }
            else
            {
                emit setMode(md);
                reply = "OK";
            }
            break;
        case cmd_pm8000://pm8000
            if (subcommand.at(2).contains("vol+", Qt::CaseInsensitive))
                emit setPM8000(LircControl::cmd_sysVolumeUp);
            else if (subcommand.at(2).contains("vol-", Qt::CaseInsensitive))
                emit setPM8000(LircControl::cmd_sysVolumeDown);
            else if (subcommand.at(2).contains("mute", Qt::CaseInsensitive))
                emit setPM8000(LircControl::cmd_sysMute);
            break;
        case cmd_radio: //radio
            emit setRadio(subcommand.at(2).toInt());
            reply = "OK";
            break;
        case cmd_backlight: //backlight
            emit setBacklight(subcommand.at(2).toInt());
            reply = "OK";
            break;
        case cmd_spdifinput: //spdifinput
            emit setSPDIFInput(subcommand.at(2).toInt());
            reply = "OK";
            break;
        case cmd_play: //play
            emit setPlay();
            reply = "OK";
            break;
        case cmd_pause: //pause
            emit setPause();
            reply = "OK";
            break;
        case cmd_next: //next
            emit setNext();
            reply = "OK";
            break;
        case cmd_previous: //previous
            emit setPrevious();
            reply = "OK";
            break;
        case cmd_stop: //stop
            emit setStop();
            reply = "OK";
            break;
        case cmd_taster: //taster
            emit taster(subcommand.at(2).toInt());
            reply = "OK";
            break;
        case cmd_shutdown: //shutdown
            emit tasterZweitbelegung(2);
            reply = "OK";
            break;
        default:
            ;
        }
    }
    else if (subcommand.at(0).compare("get", Qt::CaseInsensitive) == 0)
    {
        switch (cmdcode)
        {
        case cmd_status: //status (struct UDPDatagram, für Initialisierung der Clienten)
            reply = "[RaspiDAC]";
            reply.append(static_cast<NetAPIServer*>(parent())->getDatagram());
            break;
        case cmd_radiolist: //radiolist
            //reply = static_cast<NetAPIServer*>(parent())->getRadioList();
            reply = "[radioList]";
            reply.append(m_netapiserver->getRadioList());
            break;
        case cmd_inputlist:
            reply = "[inputList]";
            reply.append(m_netapiserver->getInputList());
            break;
        case cmd_metadata: //metadata
            reply = "[MetaData]";
            reply.append(m_netapiserver->getMetaData());
            break;
        default:
            ;
        }
    }
    return reply;
}

int NetAPIThread::onoff(const QString &cmd, bool invers)
{
    if(cmd.compare("on", Qt::CaseInsensitive) == 0)
    {
        return invers?0:1;
    }
    else if (cmd.compare("off", Qt::CaseInsensitive) == 0)
    {
        return invers?1:0;
    }
    else if (cmd.compare("upnp", Qt::CaseInsensitive) == 0)
    {
        return 1;
    }
    else if (cmd.compare("spdif", Qt::CaseInsensitive) == 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

RaspiDAC::GUIMode NetAPIThread::mode(const QString &cmd)
{
    if (cmd.compare("standby", Qt::CaseInsensitive) == 0)
    {
        return RaspiDAC::RPI_Standby;
    }
    else if (cmd.compare("upnp", Qt::CaseInsensitive) == 0)
    {
        return RaspiDAC::RPI_Upnp;
    }
    else if (cmd.compare("radio", Qt::CaseInsensitive) == 0)
    {
        return RaspiDAC::RPI_Radio;
    }
    else if (cmd.compare("spdif", Qt::CaseInsensitive) == 0)
    {
        return RaspiDAC::RPI_Spdif;
    }
    return RaspiDAC::RPI_Standby;
}





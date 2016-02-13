#include "netapithread.h"
#include "netapiserver.h"


NetAPIThread::NetAPIThread(int socketDescriptor, NetAPIServer *parent)
    : QThread(parent), socketDescriptor(socketDescriptor), m_netapiserver(parent)
{
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
    int index, value;

    QStringList set_commands;
    QStringList get_commands;
    set_commands << "rc5direct" << "standby" << "dacinput" << "mode" << "pm8000"
                 << "radio" << "backlight" << "spdifinput" << "play" << "pause"
                 << "next" << "previous" << "stop" << "taster";
    get_commands << "status" << "radiolist" << "metadata";

    qDebug() << "NetAPIThread::parser: " << command;

//    QStringList subcommand = command.split(" ");
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

    if (subcommand.at(0).compare("set", Qt::CaseInsensitive) == 0)
    {
        if((index = set_commands.indexOf(subcommand.at(1))) > -1)
        {
            switch (index)
            {
            case 0: //rc5direct
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
            case 1: //standby
            {
                emit setMode(RaspiDAC::RPI_Standby);
                reply = "OK";
            }
                break;
            case 2:  //dacinput
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
            case 3: //mode
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
            case 4://pm8000

                break;
            case 5: //radio
                emit setRadio(subcommand.at(2).toInt());
                reply = "OK";
                break;
            case 6: //backlight
                emit setBacklight(subcommand.at(2).toInt());
                reply = "OK";
                break;
            case 7: //spdifinput
                emit setSPDIFInput(subcommand.at(2).toInt());
                reply = "OK";
                break;
            case 8: //play
                emit setPlay();
                reply = "OK";
                break;
            case 9: //pause
                emit setPause();
                reply = "OK";
                break;
            case 10: //next
                emit setNext();
                reply = "OK";
                break;
            case 11: //previous
                emit setPrevious();
                reply = "OK";
                break;
            case 12: //stop
                emit setStop();
                reply = "OK";
                break;
            case 13: //taster
                emit taster(subcommand.at(2).toInt());
                reply = "OK";
                break;
            }
        }
    }
    else if (subcommand.at(0).compare("get", Qt::CaseInsensitive) == 0)
    {
        if((index = get_commands.indexOf(subcommand.at(1))) > -1)
        {
            switch (index)
            {
                case 0: //status (struct UDPDatagram, für Initialisierung der Clienten)
                    //reply  = static_cast<NetAPIServer*>(parent())->getGUIMode();
                    reply = "[RaspiDAC]";
                    reply.append(static_cast<NetAPIServer*>(parent())->getDatagram());
                break;
                case 1: //radiolist
                    //reply = static_cast<NetAPIServer*>(parent())->getRadioList();
                    reply = "[radioList]";
                    reply.append(m_netapiserver->getRadioList());
                    break;
            case 2: //metadata
                    reply = "[MetaData]";
                    reply.append(m_netapiserver->getMetaData());
                break;
            }
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





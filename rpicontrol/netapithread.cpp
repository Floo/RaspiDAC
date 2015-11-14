#include "netapithread.h"

NetAPIThread::NetAPIThread(int socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
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
    set_commands << "rc5direct" << "standby" << "dacinput" << "mode" << "pm8000" << "radio" << "backlight"
                 << "spdifinput" << "play" << "pause" << "next" << "previous" << "stop";
    get_commands << "standby" << "radiolist";

    qDebug() << command;

    QStringList subcommand = command.split(" ");

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
                emit setStandby();
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
                if((value = onoff(subcommand.at(2))) > 3 || onoff(subcommand.at(2)) < 1)
                {
                    break;
                }
                else
                {
                    emit setMode(value);
                    reply = "OK";
                }
                break;
            case 4://pm8000

                break;
            case 5: //radio

                break;
            case 6: //backlight
                emit setBacklight(subcommand.at(2).toInt());
                reply = "OK";
                break;
            case 7: //spdifinput
                emit setSPDIFInput(subcommand.at(2).toInt());
                break;
            case 8: //play
                emit setPlay();
                break;
            case 9: //pause
                emit setPause();
                break;
            case 10: //next
                emit setNext();
                break;
            case 11: //previous
                emit setPrevious();
                break;
            case 12: //stop
                emit setStop();
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
                case 0: //standby

                    break;
                case 1: //radiolist

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
    else if (cmd.compare("upnp", Qt::CaseInsensitive) == 0)
    {
        return 1;
    }
    else if (cmd.compare("radio", Qt::CaseInsensitive) == 0)
    {
        return 2;
    }
    else if (cmd.compare("spdif", Qt::CaseInsensitive) == 0)
    {
        return 3;
    }
    else
    {
        return -1;
    }
}

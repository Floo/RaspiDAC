#ifndef NETAPITHREAD_H
#define NETAPITHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QHash>

#include "../raspidac.h"
#include "../rpicontrol/lirccontrol.h"

class NetAPIServer;

class NetAPIThread : public QThread
{

    Q_OBJECT

public:
    NetAPIThread(int socketDescriptor, NetAPIServer *parent);
    ~NetAPIThread();

public:

    void run() Q_DECL_OVERRIDE;

public slots:


signals:
    void error(int socketError, const QString &message);
    void setMode(RaspiDAC::GUIMode);
    void setDACinput(int value);
    void setSPDIFInput(int value);
    void setBacklight(int value);
    void setRC5direct(int value);
    void setPlay();
    void setPause();
    void setNext();
    void setPrevious();
    void setStop();
    void setRadio(int id);
    void taster(int);
    void tasterZweitbelegung(int);
    void setPM8000(LircControl::sysCommandCode);

private:
    int socketDescriptor;
    int onoff(const QString &cmd, bool invers = false);
    RaspiDAC::GUIMode mode(const QString &cmd);
    QString parser(const QString &command);
    NetAPIServer *m_netapiserver;

    enum commandCode
    {
        cmd_rc5direct,
        cmd_standby,
        cmd_dacinput,
        cmd_mode,
        cmd_pm8000,
        cmd_radio,
        cmd_backlight,
        cmd_spdifinput,
        cmd_play,
        cmd_pause,
        cmd_next,
        cmd_previous,
        cmd_stop,
        cmd_taster,
        cmd_shutdown,
        cmd_status,
        cmd_radiolist,
        cmd_inputlist,
        cmd_metadata
    };

    QHash<QString, commandCode> m_commandList;

};

#endif // NETAPITHREAD_H

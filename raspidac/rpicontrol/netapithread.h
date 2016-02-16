#ifndef NETAPITHREAD_H
#define NETAPITHREAD_H

#include <QThread>
#include <QTcpSocket>

#include "../raspidac.h"

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

private:
    int socketDescriptor;
    int onoff(const QString &cmd, bool invers = false);
    RaspiDAC::GUIMode mode(const QString &cmd);
    QString parser(const QString &command);
    NetAPIServer *m_netapiserver;

};

#endif // NETAPITHREAD_H

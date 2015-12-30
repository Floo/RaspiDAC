#ifndef NETAPISERVER_H
#define NETAPISERVER_H

#include <QTcpServer>

#include "netapithread.h"
#include "../application.h"

class Application;

class NetAPIServer : public QTcpServer
{

    Q_OBJECT

public:
    NetAPIServer(Application *app_h, QObject *parent = 0);
    ~NetAPIServer();

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    Application *app;

private slots:
    void setPlay();
    void setPause();
    void setNext();
    void setPrevious();
    void setStop();
    void setRadio(int);

signals:
    void play();
    void pause();
    void next();
    void previous();
    void stop();
    void radio(int);
};

#endif // NETAPISERVER_H

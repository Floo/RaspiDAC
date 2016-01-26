#ifndef NETAPISERVER_H
#define NETAPISERVER_H

#include <QTcpServer>

#include "netapithread.h"
#include "../raspidac.h"

class RaspiDAC;

class NetAPIServer : public QTcpServer
{

    Q_OBJECT

public:
    NetAPIServer(RaspiDAC *rpi_h, QObject *parent = 0);
    ~NetAPIServer();

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    RaspiDAC *m_rpi;

private slots:
    void setRadio(int);
    void setTaster(int);

signals:
    void radio(int);
    void taster(int);
};

#endif // NETAPISERVER_H

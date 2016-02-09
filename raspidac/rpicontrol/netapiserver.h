#ifndef NETAPISERVER_H
#define NETAPISERVER_H

#include <QTcpServer>
#include <QUdpSocket>

#include "../raspidac.h"
#include "helper.h"

class RaspiDAC;

class NetAPIServer : public QTcpServer
{

    Q_OBJECT

public:
    NetAPIServer(RaspiDAC *rpi_h, QObject *parent = 0);
    ~NetAPIServer();
    QString getGUIMode();
    QString getRadioList();
    QString getMetaData();
    QString getDatagram();
    void setMetaData(MetaData &md);


protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    RaspiDAC *m_rpi;
    QString m_RadioList;
    UDPDatagram m_datagram;

private slots:
    void sendDatagramm(UDPDatagram&);


signals:

};

#endif // NETAPISERVER_H

#ifndef NETAPISERVER_H
#define NETAPISERVER_H

#include <QTcpServer>

#include "../raspidac.h"

class RaspiDAC;

class NetAPIServer : public QTcpServer
{

    Q_OBJECT

public:
    NetAPIServer(RaspiDAC *rpi_h, QObject *parent = 0);
    ~NetAPIServer();
    QString getGUIMode();
    QString getRadioList();


protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    RaspiDAC *m_rpi;
    RaspiDAC::GUIMode m_GUIMode;
    QString m_RadioList;

private slots:
    void GUIMode(RaspiDAC::GUIMode);
    void radioList(const QString &list);


signals:

};

#endif // NETAPISERVER_H

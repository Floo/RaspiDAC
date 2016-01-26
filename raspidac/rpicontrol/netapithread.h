#ifndef NETAPITHREAD_H
#define NETAPITHREAD_H

#include <QThread>
#include <QTcpSocket>

class NetAPIThread : public QThread
{

    Q_OBJECT

public:
    NetAPIThread(int socketDescriptor, QObject *parent);
    ~NetAPIThread();

public:

    void run() Q_DECL_OVERRIDE;

signals:
    void error(int socketError, const QString &message);
    void setMode(int value);
    void setDACinput(int value);
    void setSPDIFInput(int value);
    void setBacklight(int value);
    void setRC5direct(int value);
    void setPlay();
    void setPause();
    void setNext();
    void setPrevious();
    void setStop();
    void setRadioStation(QString name, QString file, int id);
    void setRadio(int id);
    void taster(int);

private:
    int socketDescriptor;
    int onoff(const QString &cmd, bool invers = false);
    QString parser(const QString &command);
};

#endif // NETAPITHREAD_H

#ifndef LIRCRECEIVER_H
#define LIRCRECEIVER_H

#include <QObject>
#include <QSocketNotifier>
#include <QDebug>

#include <fcntl.h>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

#include <lirc/lirc_client.h>

class LircReceiver : public QObject
{
    Q_OBJECT
public:
    explicit LircReceiver(QObject *parent = 0);
    ~LircReceiver();

private slots:
    void readButton();

signals:
    void buttonPressed(char *);

private:
    int                 m_socket;
    QSocketNotifier     *m_socketNotifier;
    struct lirc_config  *m_config;
    char                *m_command;
};

#endif // LIRCRECEIVER_H

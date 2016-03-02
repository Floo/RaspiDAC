#ifndef LIRCSENDER_H
#define LIRCSENDER_H

#include <QObject>
#include <QDebug>
#include <QMutex>

#include <lirc/lirc_client.h>
#include <stdio.h>

class LircSender : public QObject
{
    Q_OBJECT
public:
    explicit LircSender(QObject *parent = 0);
    ~LircSender();

signals:

public slots:
    void sendKey(const char*);

private:
    int m_fd;
    QMutex m_mutex;
};

#endif // LIRCSENDER_H

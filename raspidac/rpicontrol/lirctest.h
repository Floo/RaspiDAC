#ifndef LIRCTEST_H
#define LIRCTEST_H

#include <QObject>
#include <QThread>
#include <QSocketNotifier>
#include <QDebug>

#include <fcntl.h>
#include <unistd.h>

#define PULSE_BIT       0x01000000
#define PULSE_MASK      0x00FFFFFF

class LircTest : public QThread
{
    Q_OBJECT

public:
    LircTest(QObject *parent = 0);
    ~LircTest();
    void run() Q_DECL_OVERRIDE;

private:
    int m_fd;
    QSocketNotifier *m_notifier;

};

#endif // LIRCTEST_H

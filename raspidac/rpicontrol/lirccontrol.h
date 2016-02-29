#ifndef LIRCCONTROL_H
#define LIRCCONTROL_H

#include <QObject>

class LircReceiver;
class LircSender;

class LircControl : public QObject
{
    Q_OBJECT
public:
    explicit LircControl(QObject *parent = 0);
    ~LircControl();

signals:
    void play();
    void stop();
    void pause();


public slots:
    void powerOnPM8000();
    void powerOffPM8000();
    void cmdParser(char*);

private:
    LircReceiver *m_lircreceiver;
    LircSender *m_lircsender;
};

#endif // LIRCCONTROL_H

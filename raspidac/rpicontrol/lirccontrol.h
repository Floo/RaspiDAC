#ifndef LIRCCONTROL_H
#define LIRCCONTROL_H

#include <QObject>
#include <QHash>

class LircReceiver;
class LircSender;

class LircControl : public QObject
{
    Q_OBJECT
public:
    explicit LircControl(QObject *parent = 0);
    ~LircControl();

    enum commandCode
    {
        cmd_key_1,
        cmd_key_2,
        cmd_key_3,
        cmd_key_4,
        cmd_key_5,
        cmd_key_6,
        cmd_key_7,
        cmd_key_8,
        cmd_key_9,
        cmd_powerOn,
        cmd_powerOff,
        cmd_play,
        cmd_pause,
        cmd_stop,
        cmd_next,
        cmd_previous,
        cmd_mute,
        cmd_volumeUp,
        cmd_volumeDown,
        cmd_mode
    };

signals:
    void play();
    void stop();
    void pause();


public slots:
    void cmdParser(char*);
    void sendCommand(commandCode);

private:
    LircReceiver *m_lircreceiver;
    LircSender *m_lircsender;

    QHash<QString, commandCode> m_recCmdTable;
    QHash<commandCode, QString> m_sendCmdTable;
};

#endif // LIRCCONTROL_H

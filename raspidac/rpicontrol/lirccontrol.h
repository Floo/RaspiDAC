#ifndef LIRCCONTROL_H
#define LIRCCONTROL_H

#include <QObject>
#include <QHash>
#include <QThread>
#include <QDebug>

#define RC5_TOGGLE_MASK     0x0000800
#define RC5X_TOGGLE_MASK    0x0020000
#define RC5_STARTBIT_MASK   0x0002000
#define RC5X_STARTBIT_MASK  0x0080000
#define RC5_CMD_MASK        0x000003F
#define RC5_ADR_MASK        0x00007C0

class LircReceiver;
class LircSender;

class LircControl : public QObject
{
    Q_OBJECT
    QThread lircThread;
public:
    explicit LircControl(QObject *parent = 0);
    ~LircControl();

    enum commandCode
    {
        cmd_unknown,
        cmd_key_0,
        cmd_key_1,
        cmd_key_2,
        cmd_key_3,
        cmd_key_4,
        cmd_key_5,
        cmd_key_6,
        cmd_key_7,
        cmd_key_8,
        cmd_key_9,
        cmd_play,
        cmd_pause,
        cmd_stop,
        cmd_next,
        cmd_previous,
        cmd_forward,
        cmd_rewind,
        cmd_mode
    };

    enum sysCommandCode
    {
        cmd_sysUnknown,
        cmd_sysPowerOn,
        cmd_sysPowerOff,
        cmd_sysMute,
        cmd_sysVolumeUp,
        cmd_sysVolumeDown,
        cmd_sysPhono,
        cmd_sysCD,
        cmd_sysTuner,
        cmd_sysCDR,
        cmd_sysAUX1,
        cmd_sysAUX2,
        cmd_sysTape,
        cmd_sysMD
    };

signals:
    void play();
    void stop();
    void pause();
    void powerOn();
    void powerOff();
    void next();
    void previous();
    void key(int);
    void mode();
    void irCode(int);

public slots:
    void receiveCode(int);
    void sendCode(sysCommandCode);


private:
    QHash<int, commandCode> m_recCmdTable;
    QHash<int, sysCommandCode> m_sysCmdTable;
    bool m_lastToggle;
    int m_lastCode;
    void cmdParser(int);

};


#endif // LIRCCONTROL_H

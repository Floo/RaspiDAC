#ifndef LIRCDEVICE_H
#define LIRCDEVICE_H

#include <QObject>
#include <QDebug>
#include <QSocketNotifier>
#include <QTimer>
#include <QThread>
#include <QMutex>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define PULSE_BIT      0x01000000
#define PULSE_MASK     0x00FFFFFF
#define RC5X_MASK	   0x000FC000

#define RC5_TOGGLE_MASK     0x0000800
#define RC5X_TOGGLE_MASK    0x0020000
#define RC5_STARTBIT_MASK   0x0002000
#define RC5X_STARTBIT_MASK  0x0080000
#define RC5_CMD_MASK        0x000003F
#define RC5_ADR_MASK        0x00007C0

#define PULSE_LENGTH        880 // Bitdauer ist 1,778 ms
#define SPACE_LENGTH        912
#define RC5X_SPACE_LENGTH   4466
#define EPS                 0.3 	// Relative Fehlertoleranz des empfangenen Signals 0.1 entspricht 10 %

#define COMP_PULSE_LENGTH(a, length) ((a > (length * (1 - EPS))) && (a < (length * (1 + EPS))))


class LircDevice : public QObject
{
    Q_OBJECT

public:
    explicit LircDevice(QObject *parent = 0);
    ~LircDevice();
    bool recvEnabled() { return m_recvEnabled; }
    void setRecvEnabled(bool enabled) { m_recvEnabled = enabled; }

public slots:
    void initDevice();
    void sendCode(int code);

private slots:
        void handleRead();

signals:
    void codeReceived(int);

private:
    int m_fd;
    QSocketNotifier *m_readNotifier;
    QTimer *m_disableRecvTimer;
    bool m_isSyncronized;
    int m_bitCount;
    bool m_xRC5;
    int m_code;
    int m_lastBit;
    int m_pulseCount;
	bool m_lastToggle;
    bool m_recvEnabled;
    QMutex m_sendMutex;

    int decode(int data);
    int encode(int code, char *data);
    void resetDecoder();

};

#endif // LIRCDEVICE_H

#ifndef LIRCDEVICE_H
#define LIRCDEVICE_H

#include <QObject>
#include <QDebug>
#include <QSocketNotifier>

#include <fcntl.h>
#include <unistd.h>

#define PULSE_BIT      0x01000000
#define PULSE_MASK     0x00FFFFFF
#define RC5X_MASK	   0x000FC000
#define PULSE_LENGTH   889 // Bitdauer ist 1,778 ms
#define EPS			   0.1 	// Relative Fehlertoleranz des empfangenen Signals 0.1 entspricht 10 %

#define COMP_PULSE_LENGTH(a, length) ((a > (length * (1 - EPS))) && (a < (length * (1 + EPS))))


class LircDevice : public QObject
{
    Q_OBJECT

public:
    explicit LircDevice(QObject *parent = 0);
    ~LircDevice();

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
    bool m_isSyncronized;
    int m_bitCount;
    bool m_xRC5;
    int m_code;
    int m_lastBit;
    int m_pulseCount;
	bool m_lastToggle;

    int decode(int data);
    int encode(int code, char *data);
    void resetDecoder();

};

#endif // LIRCDEVICE_H

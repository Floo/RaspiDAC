#ifndef RPIGPIO_H
#define RPIGPIO_H

#include <QObject>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <QDebug>
#include <QTime>

#define GPIO04 4    //Input-Select
#define GPIO27 27   //RC5-direct
#define GPIO05 5    //Taster1
#define GPIO06 6    //Taster3
#define GPIO13 13   //Taster2
#define GPIO12 12   //Taster4
#define GPIO23 23   //Relais
#define GPIO26 26   //LED_Power

#define PCA9530 0x60  //I2C Adresse des LED-PWM
#define CS8416  0x10  //I2C Adresse des SPDIF-Decoders

// -- CS8416 I2C COMMANDS

#define CS_CMD_IDVER            0x7f
#define CS_CMD_SER_AUDIO_FORMAT 0x05
#define CS_CMD_CONTROL0         0x00
#define CS_CMD_CONTROL1         0x01
#define CS_CMD_CONTROL2         0x02
#define CS_CMD_CONTROL3         0x03
#define CS_CMD_CONTROL4         0x04

#define CS_CMD_REC_CHAN_STAT     0x0A
#define CS_CMD_AUDIO_FORMAT_DET  0x0B
#define CS_CMD_REC_ERROR         0x0C
#define CS_CMD_IRQ_STAT          0x0D

#define LED_ON     0
#define LED_OFF    1
#define LED_BLINK  100
#define REL_ON     1
#define REL_OFF    0
#define RC5_DIRECT_ON  1
#define RC5_DIRECT_OFF 0
#define INPUT_UPNP   1
#define INPUT_DAC    0

#define BACKLIGHT_MAX   0
#define BACKLIGHT_DIMM  200
#define BACKLIGHT_MIN   255

class RPiGPIO;

class RPiTaster : public QObject
{
    Q_OBJECT

public:
    RPiTaster(RPiGPIO*);
    ~RPiTaster();

    void taster(int);

private:
    RPiGPIO *m_gpio;
    QTime m_debouncetime;

private slots:
};


class RPiGPIO : public QObject
{
    Q_OBJECT

public:
    RPiGPIO();
    ~RPiGPIO();

    int getLED();
    int getRC5direct();
    int getRelais();
    int getInputSelect();
    void pca9530Setup();
    void cs8416Setup();


public slots:
    void toggleLED();
    void setLED(int);
    void setRC5direct(int);
    void setRelais(int);
    void setInputSelect(int);
    void setBacklight(int);
    void setCS8416InputSelect(int);
    void getCS8416ID();
    void getCS8416Reg(int);

signals:
    void taster(int);
    void tasterZweitbelegung(int);

private:
    class Internal;
    Internal *m;
    int fd_pca9530 = 0;
    int fd_cs8416 = 0;
};

#endif // RPIGPIO_H

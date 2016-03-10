#ifndef RPIGPIO_H
#define RPIGPIO_H

#include <QObject>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <QDebug>
#include <QTime>
#include <QMutex>

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

#define DEBOUNCE_TIME 25 //ms

// TSL2591 Helligkeitssensor
#define TSL2591_ADDR                0x29
#define TSL2591_DEVICE_ID_VALUE     0x50
#define TSL2591_DEVICE_RESET_VALUE  0x80
#define TSL2591_COMMAND             0x80
#define TSL2591_NORMAL_OP           0x20
#define TSL2591_ENABLE_RW 	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x00)
#define TSL2591_CONFIG_RW 	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x01)
#define TSL2591_AILTL_RW  	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x04)
#define TSL2591_AILTH_RW  	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x05)
#define TSL2591_AIHTL_RW  	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x06)
#define TSL2591_AIHTH_RW  	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x07)
#define TSL2591_NPAILTL_RW	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x08)
#define TSL2591_NPAILTH_RW	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x09)
#define TSL2591_NPAIHTL_RW	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x0A)
#define TSL2591_NPAIHTH_RW	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x0B)
#define TSL2591_PERSIST_RW	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x0C)
#define TSL2591_PID_R       (TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x11)
#define TSL2591_ID_R        (TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x12)
#define TSL2591_STATUS_R 	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x13)
#define TSL2591_C0DATAL_R	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x14)
#define TSL2591_C0DATAH_R	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x15)
#define TSL2591_C1DATAL_R 	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x16)
#define TSL2591_C1DATAH_R	(TSL2591_COMMAND | TSL2591_NORMAL_OP | 0x17)
#define TSL2591_LUX_DF            408.0
#define TSL2591_LUX_COEFB         1.64  // CH0 coefficient 
#define TSL2591_LUX_COEFC         0.59  // CH1 coefficient A
#define TSL2591_LUX_COEFD         0.86  // CH2 coefficient B

class RPiGPIO;

class RPiTaster : public QObject
{
    Q_OBJECT

public:
    RPiTaster(RPiGPIO*);
    ~RPiTaster();

    void taster(int);
    void tasterPower();

private:
    RPiGPIO *m_gpio;
    QTime m_debouncetime;
    bool m_ts_state[3];
    QMutex mutex, mutexPower;

private slots:
};


class RPiGPIO : public QObject
{
    Q_OBJECT

public:
    RPiGPIO();
    ~RPiGPIO();
	
	typedef enum
	{
		TSL2591_INTEGRATIONTIME_100MS     = 0x00,
		TSL2591_INTEGRATIONTIME_200MS     = 0x01,
		TSL2591_INTEGRATIONTIME_300MS     = 0x02,
		TSL2591_INTEGRATIONTIME_400MS     = 0x03,
		TSL2591_INTEGRATIONTIME_500MS     = 0x04,
		TSL2591_INTEGRATIONTIME_600MS     = 0x05,
	} tsl2591IntegrationTime_t;
	
	typedef enum
	{
		TSL2591_GAIN_LOW                  = 0x00,    // low gain (1x)
		TSL2591_GAIN_MED                  = 0x10,    // medium gain (25x)
		TSL2591_GAIN_HIGH                 = 0x20,    // medium gain (428x)
		TSL2591_GAIN_MAX                  = 0x30,    // max gain (9876x)
	} tsl2591Gain_t;
	
	int getLED();
    int getRC5direct();
    int getRelais();
    int getInputSelect();
    void pca9530Setup();
    void cs8416Setup();
	int getCS8416ID();
    int getCS8416Reg();
    void tsl2591Setup();
	int getTSL2591Lux();
	void setTSL2591IntegrationTime(tsl2591IntegrationTime_t);
	void setTSL2591Gain(tsl2591Gain_t)

public slots:
    void toggleLED();
    void setLED(int);
    void setRC5direct(int);
    void setRelais(int);
    void setInputSelect(int);
    void setBacklight(int);
    void setCS8416InputSelect(int);

signals:
    void taster(int);
    void tasterZweitbelegung(int);

private:
    class Internal;
    Internal *m;
    int fd_pca9530 = 0;
    int fd_cs8416 = 0;
    int fd_tsl2591 = 0;
};

#endif // RPIGPIO_H

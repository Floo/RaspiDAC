#include "rpigpio.h"

#ifdef __rpi__
RPiTaster *rpiTaster;

void cbTaster1()
{
    //qDebug() << "Taster1 gedrückt";
    rpiTaster->taster(1);;
}

void cbTaster2()
{
    //qDebug() << "Taster2 gedrückt";
    rpiTaster->taster(2);
}

void cbTaster3()
{
    //qDebug() << "Taster3 gedrückt";
    rpiTaster->tasterPower();
}

#endif

RPiTaster::RPiTaster(RPiGPIO *gpio)
{
    m_gpio = gpio;
    m_ts_state[0] = true;
    m_ts_state[1] = true;
    m_ts_state[2] = true;

    wiringPiISR(GPIO05, INT_EDGE_BOTH, &cbTaster1); //SEL/PLAY/PAUSE
    wiringPiISR(GPIO06, INT_EDGE_BOTH, &cbTaster3); //POWER
    wiringPiISR(GPIO13, INT_EDGE_BOTH, &cbTaster2); //MENU

    m_debouncetime.start();
}

RPiTaster::~RPiTaster()
{

}

void RPiTaster::taster(int ts)
{
    mutex.lock();
    int timestamp = m_debouncetime.restart();
    if ((timestamp > DEBOUNCE_TIME) && m_ts_state[ts - 1])
    {
        m_ts_state[ts - 1] = false;
        emit m_gpio->taster(ts);
    }
    else if ((timestamp > DEBOUNCE_TIME) && !m_ts_state[ts - 1])
    {
        m_ts_state[ts - 1] = true;
    }
    mutex.unlock();
}

void RPiTaster::tasterPower()
{
    mutexPower.lock();
    int timestamp = m_debouncetime.restart();
    if ((timestamp > DEBOUNCE_TIME) && m_ts_state[2])
    {
        m_ts_state[2] = false;
        if (digitalRead(GPIO13) == 0)
        {
            //gleichzeitige Tastenbetätigung
            emit m_gpio->tasterZweitbelegung(2);
        }
        else if (digitalRead(GPIO05) == 0)
        {
            emit m_gpio->tasterZweitbelegung(1);
        }
        else
        {
            emit m_gpio->taster(3);
        }
    }
    else if ((timestamp > DEBOUNCE_TIME) && !m_ts_state[2])
    {
        m_ts_state[2] = true;
    }
    mutexPower.unlock();
}


class RPiGPIO::Internal
{
public:
    int LED;
    int Relais;
    int Input;
    int RC5direct;
};

RPiGPIO::RPiGPIO()
{
    int ret;
    if ((ret = wiringPiSetupSys()) == 0)
        qDebug() << "Initialisierung von wiringPi erfolgreich.";
    else
        qDebug() << "Fehler bei Initialsierung von wiringPi. Errorcode: " << ret;


    rpiTaster = new RPiTaster(this);

    m = new Internal();

    setLED(LED_OFF);
    setInputSelect(INPUT_UPNP);
    setRelais(REL_OFF);
    setRC5direct(RC5_DIRECT_OFF);

    pca9530Setup();
    cs8416Setup();
}

RPiGPIO::~RPiGPIO()
{
    delete m;
}

void RPiGPIO::setLED(int value)
{
    m->LED = value;
    digitalWrite(GPIO26, value);
}

void RPiGPIO::setInputSelect(int value)
{
    m->Input = value;
    digitalWrite(GPIO04, value);
}

void RPiGPIO::setRC5direct(int value)
{
    m->RC5direct = value;
    digitalWrite(GPIO27, value);
}

void RPiGPIO::setRelais(int value)
{
    m->Relais = value;
    digitalWrite(GPIO23, value);
}

int RPiGPIO::getLED()
{
    return m->LED;
}

int RPiGPIO::getRC5direct()
{
    return m->RC5direct;
}

int RPiGPIO::getInputSelect()
{
    return m->Input;
}

int RPiGPIO::getRelais()
{
    return m->Relais;
}

void RPiGPIO::toggleLED()
{
    //qDebug() << "Slot ausgelöst.";
    setLED(!m->LED);
}

void RPiGPIO::pca9530Setup()
{
    int value;
    if ((fd_pca9530 = wiringPiI2CSetup (PCA9530)) < 0)
    {
        qDebug() << "I2C PCA9530 konnte nicht initialisiert werden.";
        return;
    }
    qDebug() << "I2C PCA9530 initialisiert";
    value = wiringPiI2CReadReg8(fd_pca9530, 0x02); // read PWM0 register
    qDebug() << "I2C PCA9530 Register: " << value;

    wiringPiI2CWriteReg8(fd_pca9530, 0x05, 0xF2); //select LED0 at PWM0 rate
}

void RPiGPIO::setBacklight(int value)
{
    wiringPiI2CWriteReg8(fd_pca9530, 0x02, value);
}

void RPiGPIO::cs8416Setup()
{
    //int value;
    if ((fd_cs8416 = wiringPiI2CSetup (CS8416)) < 0)
    {
        qDebug() << "I2C CS8416 konnte nicht initialisiert werden.";
        return;
    }
    // --- CS8416 INIT INTO MASTER CLOCK GEN / RX MODE   
    wiringPiI2CWriteReg8(fd_cs8416, CS_CMD_SER_AUDIO_FORMAT, 0x84); //set Master-clock mode and I2S
    wiringPiI2CWriteReg8(fd_cs8416, CS_CMD_CONTROL1, 0x80); // set SMCLK=1
    wiringPiI2CWriteReg8(fd_cs8416, CS_CMD_CONTROL4, 0x80); //set RUN state

    qDebug() << "I2C CS8416 initialisiert";
}

void RPiGPIO::setCS8416InputSelect(int value)
{
    if ((value < 0) || (value > 3))
        return;
    int oldValue = wiringPiI2CReadReg8(fd_cs8416, CS_CMD_CONTROL4);
    int newValue = (oldValue & 0xC0) | (value << 3);
    wiringPiI2CWriteReg8(fd_cs8416, CS_CMD_CONTROL4, newValue);
    qDebug() << "I2C CS8416 input set to: " << value;
}

int RPiGPIO::getCS8416ID()
{
    int value = wiringPiI2CReadReg8(fd_cs8416, CS_CMD_IDVER);
    qDebug() << "I2C CS8416 ID: " << value;
	return value;
}

int RPiGPIO::getCS8416Reg()
{
    int ret = wiringPiI2CReadReg8(fd_cs8416, reg);
    qDebug() << "I2C CS8416 Register " << reg << ": " << ret;
	return ret;
}

void RPiGPIO::tsl2591Setup()
{
    if ((fd_tsl2591 = wiringPiI2CSetup (TSL2591_ADDR)) < 0)
    {
        qDebug() << "I2C TSL2591 konnte nicht initialisiert werden.";
        return;
    }

    wiringPiI2CWriteReg8(fd_tsl2591, TSL2591_CONFIG_RW, TSL2591_DEVICE_RESET_VALUE);
    int ret = wiringPiI2CReadReg8(fd_tsl2591, TSL2591_ID_R);
    if (ret != TSL2591_DEVICE_ID_VALUE)
    {
        qDebug() << "RPiGPIO::tsl2591Setup: Error reading DeviceID";
    }
}

int RPiGPIO::getLuxTSL2591()
{
	float atime, again, cpl, lux1, lux2;
		
    int valueC0 = wiringPiI2CReadReg16(fd_tsl2591, TSL2591_C0DATAL_R);
    int valueC1 = wiringPiI2CReadReg16(fd_tsl2591, TSL2591_C1DATAL_R);
	
	int conf = wiringPiI2CReadReg8(fd_tsl2591, TSL2591_CONFIG_RW);
	tsl2591Gain_t gain = conf & 0x30;
	tsl2591IntegrationTime_t integration = conf & 0x07;

	switch (integration)
	{
	case TSL2591_INTEGRATIONTIME_100MS:
		atime = 100;
		break;
	case TSL2591_INTEGRATIONTIME_200MS:
		atime = 200;
		break;
	case TSL2591_INTEGRATIONTIME_300MS:
		atime = 300;
		break;
	case TSL2591_INTEGRATIONTIME_400MS:
		atime = 400;
		break;
	case TSL2591_INTEGRATIONTIME_500MS:
		atime = 500;
		break;
	case TSL2591_INTEGRATIONTIME_600MS:
		atime = 600;
		break;
	default:
		atime = 100;
		break;
	}
	switch (gain)
	{
	case TSL2591_GAIN_LOW:
		again = 1;
		break;
	case TSL2591_GAIN_MED:
		again = 25;
		break;
	case TSL2591_GAIN_LOW:
		again = 428;
		break;
	case TSL2591_GAIN_LOW:
		again = 9876;
		break;
	default:
		again = 1;
		break;
	}
	cpl = (atime * again) / TSL2591_LUX_DF;

	lux1 = ( (float)valueC0 - (TSL2591_LUX_COEFB * (float)valueC1) ) / cpl;
	lux2 = ( ( TSL2591_LUX_COEFC * (float)valueC0 ) - ( TSL2591_LUX_COEFD * (float)valueC1 ) ) / cpl;
	return (int)(lux1 > lux2 ? lux1 : lux2);	
}

void RPiGPIO::setTSL2591Gain(tsl2591Gain_t gain)
{
	int conf = wiringPiI2CReadReg8(fd_tsl2591, TSL2591_CONFIG_RW);
	conf = conf & 0x07;
	conf = conf | gain;
	wiringPiI2CWriteReg8(fd_tsl2591, TSL2591_CONFIG_RW, conf);
}

void RPiGPIO::setTSL2591IntegrationTime(tsl2591IntegrationTime_t time)
{
	int conf = wiringPiI2CReadReg8(fd_tsl2591, TSL2591_CONFIG_RW);
	conf = conf & 0x30;
	conf = conf | time;
	wiringPiI2CWriteReg8(fd_tsl2591, TSL2591_CONFIG_RW, conf);
}




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

void RPiGPIO::getCS8416ID()
{
    int value = wiringPiI2CReadReg8(fd_cs8416, CS_CMD_IDVER);
    qDebug() << "I2C CS8416 ID: " << value;
}

void RPiGPIO::getCS8416Reg(int reg)
{
    int ret = wiringPiI2CReadReg8(fd_cs8416, reg);
    qDebug() << "I2C CS8416 Register " << reg << ": " << ret;
}


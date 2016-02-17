#include "rpigpio.h"

#ifdef __rpi__
RPiTaster *rpiTaster;

void cbTaster1FE()
{
    qDebug() << "Taster1 gedrückt";
    rpiTaster->tasterFE(1);;
}

void cbTaster2FE()
{
    qDebug() << "Taster2 gedrückt";
    rpiTaster->tasterFE(2);
}

void cbTaster3FE()
{
    qDebug() << "Taster3 gedrückt";
    rpiTaster->tasterFE(3);
}

void cbTaster1RE()
{
    qDebug() << "Taster1 losgelassen";
    rpiTaster->tasterRE(1);;
}

void cbTaster2RE()
{
    qDebug() << "Taster2 losgelassen";
    rpiTaster->tasterRE(2);
}

void cbTaster3RE()
{
    qDebug() << "Taster3 losgelassen";
    rpiTaster->tasterRE(3);
}
#endif

RPiTaster::RPiTaster(RPiGPIO *gpio)
{
    m_gpio = gpio;
    m_ts_state = {true, true, true};

    wiringPiISR(GPIO05, INT_EDGE_FALLING, &cbTaster1FE); //SEL/PLAY/PAUSE
    wiringPiISR(GPIO06, INT_EDGE_FALLING, &cbTaster3FE); //POWER
    wiringPiISR(GPIO13, INT_EDGE_FALLING, &cbTaster2FE); //MENU

    wiringPiISR(GPIO05, INT_EDGE_RISING, &cbTaster1RE); //SEL/PLAY/PAUSE
    wiringPiISR(GPIO06, INT_EDGE_RISING, &cbTaster3RE); //POWER
    wiringPiISR(GPIO13, INT_EDGE_RISING, &cbTaster2RE); //MENU

    m_debouncetime.start();
}

RPiTaster::~RPiTaster()
{

}

void RPiTaster::tasterFE(int ts)
{
    mutex.lock();
    if ((m_debouncetime.restart() > DEBOUNCE_TIME) && m_ts_state[ts - 1])
    {
        m_ts_state[ts - 1] = false;
        if ((ts == 3) && (digitalRead(GPIO13) == 0))
        {
            //gleichzeitige Tastenbetätigung
            emit m_gpio->tasterZweitbelegung(2);
        }
        else if ((ts == 3) && (digitalRead(GPIO05) == 0))
        {
            emit m_gpio->tasterZweitbelegung(1);
        }
        emit m_gpio->taster(ts);
    }
    mutex.unlock();
}

void RPiTaster::tasterRE(int ts)
{
    mutex.lock();
    if ((m_debouncetime.restart() > DEBOUNCE_TIME) && !m_ts_state[ts - 1])
    {
        m_ts_state[ts - 1] = true;
    }
    mutex.unlock();
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


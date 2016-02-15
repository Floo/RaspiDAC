#include "rpigpio.h"

#ifdef __rpi__
RPiTaster *rpiTaster;

void cbTaster1()
{
    qDebug() << "Taster1 gedrückt";
    rpiTaster->taster(1);;
}

void cbTaster2()
{
    qDebug() << "Taster2 gedrückt";
    rpiTaster->taster(2);
}

void cbTaster3()
{
    qDebug() << "Taster3 gedrückt";
    rpiTaster->taster(3);
}

void cbTaster4()
{
    qDebug() << "Taster4 gedrückt";
    rpiTaster->taster(4);
}
#endif

RPiTaster::RPiTaster(RPiGPIO *gpio)
{
    m_gpio = gpio;
    wiringPiISR(GPIO05, INT_EDGE_FALLING, &cbTaster1);
    wiringPiISR(GPIO06, INT_EDGE_FALLING, &cbTaster3);
    wiringPiISR(GPIO13, INT_EDGE_FALLING, &cbTaster2);
    wiringPiISR(GPIO12, INT_EDGE_FALLING, &cbTaster4);

    m_debouncetime.start();
}

RPiTaster::~RPiTaster()
{

}

void RPiTaster::taster(int ts)
{
    if (m_debouncetime.restart() > 100)
    {
        if ((ts == 3) && (digitalRead(GPIO13) == 1))
            //gleichzeitige Tastenbetätigung
            emit m_gpio->taster_shutdown();
        else
            emit m_gpio->taster(ts);
    }
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
    qDebug() << "Slot ausgelöst.";
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


#include "lircdevice.h"
#include "upplay/HelperStructs/CSettingsStorage.h"

LircDevice::LircDevice(QObject *parent) :
    QObject(parent), m_lastToggle(false), m_recvEnabled(true)
{
    m_disableRecvTimer = new QTimer;
    m_disableRecvTimer->setSingleShot(true);
}

LircDevice::~LircDevice()
{

}

void LircDevice::initDevice()
{
    QString dev = CSettingsStorage::getInstance()->getLircDevice();
    m_fd = open(dev.toUtf8(), O_RDWR|O_NONBLOCK);
    if (m_fd == -1)
    {
        qDebug() << "LircDevice::initDevice: FATAL, could not open device";
        return;
    }
    resetDecoder();
    m_readNotifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(m_readNotifier, SIGNAL(activated(int)), this, SLOT(handleRead()));
}

void LircDevice::handleRead()
{
    ssize_t count = sizeof(int);
    ssize_t result;
    int data, code;
    //int pulseBit = 0;
    //uint32_t pulseLength;
    m_readNotifier->setEnabled(false);
    do {
        result = read(m_fd, (void*)&data, count);
        if ((result == count) && m_recvEnabled)
        {
            //pulseBit = data & PULSE_BIT;
            //pulseLength = (uint32_t)(data & PULSE_MASK);
            //qDebug() << "LircDevice::handleRead: " << ((pulseBit) ? "pulse" : "space") << pulseLength;
            if ((code = decode(data)) != 0)
            {
                qDebug() << "LircDevice::handleRead: Code received " << code;
                emit codeReceived(code);
            }
        }
    } while (result > 0);
    m_readNotifier->setEnabled(true);
}

int LircDevice::decode(int data)
{
    bool pulseBit = data & PULSE_BIT;
    uint32_t pulseLength = (uint32_t)(data & PULSE_MASK);
    if (pulseLength > 6 * PULSE_LENGTH)
    {
        resetDecoder();
        return 0;
    }
    if (!m_isSyncronized && COMP_PULSE_LENGTH(pulseLength, PULSE_LENGTH) && pulseBit)
    {
        m_isSyncronized = true;
        m_lastBit = 1;
        m_code = m_lastBit;
        m_bitCount++;
    }
    if (m_isSyncronized)
    {
        if (COMP_PULSE_LENGTH(pulseLength, 2 * PULSE_LENGTH))
        {
            m_lastBit = !m_lastBit;
            m_bitCount++;
            m_code = m_code << 1;
            m_code = m_code | m_lastBit;
        }
        else if (COMP_PULSE_LENGTH(pulseLength, PULSE_LENGTH) && m_pulseCount == 0)
        {
            m_pulseCount++;
        }
        else if (COMP_PULSE_LENGTH(pulseLength, PULSE_LENGTH) && m_pulseCount == 1)
        {
            m_pulseCount = 0;
            m_bitCount++;
            m_code = m_code << 1;
            m_code = m_code | m_lastBit;
        }
        else if (m_pulseCount == 1)
        {
            resetDecoder();
        }
        else if (COMP_PULSE_LENGTH(pulseLength, 5 * PULSE_LENGTH) && m_bitCount == 8)
        {
            m_xRC5 = true;
            if (m_pulseCount == 0)
            {
                m_pulseCount++;
            }
            else
            {
                m_pulseCount = 0;
                m_bitCount++;
                m_code = m_code << 1;
                m_code = m_code | m_lastBit;
            }
        }
        else
        {
            resetDecoder();
        }
        if ((!m_xRC5 && m_bitCount == 14) || (m_xRC5 && m_bitCount == 20))
        {
            //int ret = (m_xRC5 ? (m_code | RC5X_MASK) : m_code);
            int ret = m_code;
            resetDecoder();
            return ret;
        }
    }
    return 0;
}

void LircDevice::resetDecoder()
{
    m_isSyncronized = false;
    m_bitCount = 0;
    m_xRC5 = false;
    m_code = 0;
    m_pulseCount = 0;
}

int LircDevice::encode(int code, char *data)
{
    bool lastBit = true;
    bool bit;
    int i, j, pulseLength, bitCount;
    u_int64_t bitFolge, mask;
	
	if (code & RC5X_MASK)
	{
		code = code | (RC5X_STARTBIT_MASK);
		if (m_lastToggle)
		{
			code = code | (RC5X_TOGGLE_MASK);
		}
	} else {
		code = code | (RC5_STARTBIT_MASK);
		if (m_lastToggle)
		{
			code = code | (RC5_TOGGLE_MASK);
		}
	}
    qDebug() << "LircDevice::encode: Code = " << code;
	// zuerst eine Bitfolge erstellen
	bitFolge = 0;
    bitCount = (code & RC5X_MASK) ? 20 : 14;
	do {
		bitCount--;
		bitFolge = bitFolge << 2;
        //bitFolge = bitFolge | ((code & (1 << bitCount)) ? 0b01 : 0b10);
		if (code & (1 << bitCount))
		{
            bitFolge = bitFolge | 0b01;
		} else {
            bitFolge = bitFolge | 0b10;
		}
	} while (bitCount > 0);
    //qDebug() << "LircDevice::encode: Bitfolge = " << bitFolge;
    // Zeiten zuordnen
    bitFolge = bitFolge << 1;
	bitCount = (code & RC5X_MASK) ? 40 : 28;
	bitCount--;
	i = 0;
	do {
		j = 0;
		do {
			j++;
			bitCount--;
            mask = 1;
            mask = (mask << bitCount);
            bit = (bool)(bitFolge & mask);
        } while ((bit == lastBit) && (bitCount > 0));
		
        lastBit = bit;
		
        if ((code & RC5X_MASK) && (bitCount == 24))
			pulseLength = 5 * PULSE_LENGTH;
		else
			pulseLength = j * PULSE_LENGTH;
        //qDebug() << "LircDevice::encode: Pulselength = " << pulseLength;
        memcpy(data + i * sizeof(int), &pulseLength, sizeof(int));
        i++;
	} while (bitCount > 0);
    if ((i % 2) == 0)
        i--;
    return i;
}

void LircDevice::sendCode(int code)
{
    char *data;
    int size;

    if (code == -1)
        return;

    setRecvEnabled(false);
    m_disableRecvTimer->singleShot(300, [=] {
        setRecvEnabled(true);
    });

    if (code & RC5X_MASK)
    {
        size = 40 * sizeof(int); //max 20 Bit
    }
    else
    {
        size = 28 * sizeof(int); // max 14 Bit
    }
    data = (char*)malloc(size);
	m_lastToggle = !m_lastToggle;
    int count = encode(code, data);
    write(m_fd, (void*)data, count * sizeof(int));
    free(data);
}

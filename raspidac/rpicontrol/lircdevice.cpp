#include "lircdevice.h"

LircDevice::LircDevice(QObject *parent) : QObject(parent)
{

}

LircDevice::~LircDevice()
{

}

void LircDevice::initDevice()
{
    m_fd = open("/dev/lirc0", O_RDWR|O_NONBLOCK);
    if (m_fd == -1)
    {
        qDebug() << "LircDevice::initDevice: FATAL, coulld not open device";
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
        if (result == count)
        {
            //pulseBit = data & PULSE_BIT;
            //pulseLength = (uint32_t)(data & PULSE_MASK);
            //qDebug() << "LircDevice::handleRead: " << ((pulseBit) ? "pulse" : "space") << pulseLength;
            if ((code = decode(data)) != 0)
            {
                //qDebug() << "LircDevice::handleRead: Code received " << code;
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
            int ret = (m_xRC5 ? (m_code | XRC5_MASK) : m_code);
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
    int bitCount = (code | XRC5_MASK) ? 22 : 14;
    bool lastBit = true;
    int i = 0;
    int pulseLength;
    while (bitCount-- >= 0)
    {
        if ((code | XRC5_MASK) && (bitCount == 12))
        {
            pulseLength = 5 * PULSE_LENGTH;
            memcpy(&pulseLength, data + i * sizeof(int), sizeof(int));
            i++;
            continue;
        }
        if ((code & (2^bitCount)) == lastBit)
        {
            pulseLength = PULSE_LENGTH;
            memcpy(&pulseLength, data + i * sizeof(int), sizeof(int));
            i++;
            memcpy(&pulseLength, data + i * sizeof(int), sizeof(int));
            i++;
        }
        else
        {
            lastBit = !lastBit;
            pulseLength = 2 * PULSE_LENGTH;
            memcpy(&pulseLength, data + i * sizeof(int), sizeof(int));
            i++;
        }
    }
    return i;
}

void LircDevice::sendCode(int code)
{
    if (code == -1)
        return;

    char *data;
    int size;
    if (code | XRC5_MASK)
    {
        size = 44 * sizeof(int);
    }
    else
    {
        size = 28 * sizeof(int);
    }
    data = (char*)malloc(size);
    int count = encode(code, data);
    write(m_fd, (void*)data, count);
    free(data);
}

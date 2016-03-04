#include "lirctest.h"

LircTest::LircTest(QObject *parent)
    : QThread(parent)
{

}

LircTest::~LircTest()
{

}

void LircTest::run()
{
    QByteArray buffer;
    m_fd = open("/dev/lirc0", O_RDONLY);//|O_NONBLOCK);
    if (m_fd >= 0)
    {
        ssize_t count;
        char block[sizeof(int)];
        //m_notifier->setEnabled(false);
        do
        {
            count = read(m_fd, block, sizeof(int));

            int data = (int)*block;
            qDebug() << "LircTest::run: " << ((data & PULSE_BIT) ? "pulse" : "space") << (u_int32_t)(data & PULSE_MASK);
            buffer.append(block, count);
        } while(count > 0);

    }
    else
    {
        qDebug() << "LircTest::run: Error: Could not open";
    }
}




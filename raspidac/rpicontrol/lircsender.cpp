#include "lircsender.h"

LircSender::LircSender(QObject *parent) : QObject(parent)
{
//    m_fd = lirc_get_local_socket(NULL, 0);
//    if (m_fd < 0) {
//        qDebug() << "LircSender::LircSender: Could not get file descriptor for LIRC socket";
//    }
}

LircSender::~LircSender()
{

}

//void LircSender::sendKey(const char *key)
//{
//    if (lirc_send_one(m_fd, "RC8000PM", key) == -1) {
//        qDebug() << "LircSender::sendKey: Error sending LIRC command";
//    }
//}

void LircSender::sendKey(const char *key)
{
    /*
     * Invoking system() function is temporary solution - we'll switch soon to
     * liblirc_client and lirc_send_one() - which will be available in next lirc
     * release.
     */

    char *cmd;
    int ret;

    m_mutex.lock();

    ret = asprintf (&cmd, "irsend SEND_ONCE RC8000PM %s", key);
    if (ret < 0)
    {
        qDebug() << "LircSender::sendKey: Error, can't prepare LIRC command";
    }
    ret = system (cmd);
    if (ret != 0)
    {
        qDebug() << "LircSender::sendKey: Error, can't send signal";
    }
    free (cmd);
    m_mutex.unlock();
}


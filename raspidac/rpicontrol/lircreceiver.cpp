#include "lircreceiver.h"

LircReceiver::LircReceiver(QObject *parent) : QObject(parent)
{
    int socketFlags;

    // socket setup with nonblock
    m_socket = lirc_init("RaspiDAC", 1);
    if (m_socket == -1) {
        qDebug() << "Failed to connect to LIRC";
        return;
    }

    if (lirc_readconfig(NULL, &m_config, NULL) == -1) {
        qDebug() << "Failed reading LIRC config file";
    }

    fcntl(m_socket, F_GETOWN, getpid());
    socketFlags = fcntl(m_socket, F_GETFL, 0);
    if (socketFlags != -1) {
        fcntl(socketFlags, F_SETFL, socketFlags | O_NONBLOCK);
    }

    m_socketNotifier = new QSocketNotifier(m_socket, QSocketNotifier::Read, 0);
    connect(m_socketNotifier, SIGNAL(activated(int)), this, SLOT(readButton()) );

    qDebug() << "LircReceiver::LircReceiver: connected to socket: " << m_socket;
}

LircReceiver::~LircReceiver()
{
    lirc_freeconfig(m_config);
    delete m_socketNotifier;
    lirc_deinit();

    qDebug() << "LircReceiver::~LircReceiver: cleaned up";
}

void LircReceiver::readButton()
{
    char *code;
    int ret;

    if (lirc_nextcode(&code) == 0 && code != NULL) {   // no error && a string is available
        // handle any command attached to that button
        while ( (ret = lirc_code2char(m_config, code, &m_command)) == 0 && m_command != NULL )
        {
            qDebug() << "LircClient::readButton: " << m_command;
            emit buttonPressed(m_command);
        }
        free(code);
    }
}


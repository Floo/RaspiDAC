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
}

// * cmd_SendIR()
// */
//unsigned int
//cmd_SendIR (struct libwebsocket *wsi, unsigned char *buffer, char *args)
//{
//  char *cmd;
//  int ret;

//  print_log (LOG_INFO, "(%p) (cmd_SendIR) processing request\n", wsi);

//  ret = asprintf (&cmd, "irsend SEND_ONCE %s", args);
//  if (ret < 0)
//    {
//      print_log (LOG_ERR, "(%p) (cmd_SendIR) can't prepare LIRC command\n", wsi);
//      return send_error (buffer, "Can't prepare LIRC command");
//    }

//  /*
//   * Invoking system() function is temporary solution - we'll switch soon to
//   * liblirc_client and lirc_send_one() - which will be available in next lirc
//   * release.
//   */
//  ret = system (cmd);
//  if (ret != 0)
//    {
//      print_log (LOG_ERR, "(%p) (cmd_SendIR) can't send signal\n", wsi);
//      free (cmd);
//      return send_error (buffer, "Can't send signal - please check server's log");
//    }

//  free (cmd);
//  return 0;
//}


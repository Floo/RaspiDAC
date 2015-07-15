#include "mpdradio.h"

Mpdradio::Mpdradio(uint mdpport, QObject *parent) : QObject(parent),
    m_initilised(false), m_mpd_connection(0)
{
    m_mpd_connection = mpd_connection_new("localhost", mdpport, 0);
    if (mpd_connection_get_error(m_mpd_connection) == MPD_ERROR_SUCCESS) {
        m_initilised = true;
        //mpd_run_toggle_pause(m_mpd_connection);
    } else {
        cerr << "Could not connect to mpd." << endl;
    }
}

Mpdradio::~Mpdradio()
{
    mpd_connection_free(m_mpd_connection);
}

//void Mpdradio::play_radio(QString& uri)
void Mpdradio::play_radio()
{
    QString uri = QString("http://rbb.ic.llnwd.net/stream/rbb_radioeins_mp3_m_a");
    bool ret = mpd_run_clear(m_mpd_connection);
    ret = mpd_run_add(m_mpd_connection, uri.toStdString().c_str());
    ret = mpd_run_play(m_mpd_connection);
}


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
    m_netmanager = new QNetworkAccessManager(this);
    connect(m_netmanager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(sl_playlistfetch_done(QNetworkReply*)));
}

Mpdradio::~Mpdradio()
{
    mpd_connection_free(m_mpd_connection);
}

void Mpdradio::play_radio(const QString& uri)
{
//    QString uri = QString("http://rbb.ic.llnwd.net/stream/rbb_radioeins_mp3_m_a");
    bool ret = mpd_run_clear(m_mpd_connection);
    ret = mpd_run_add(m_mpd_connection, uri.toStdString().c_str());
    ret = mpd_run_play(m_mpd_connection);
}

void Mpdradio::stop_radio()
{
    mpd_run_stop(m_mpd_connection);
}

void Mpdradio::sl_playlistfetch_done(QNetworkReply *reply)
{
    QString stationfile = QString(reply->readAll());
    if(QString::compare(filename.right(4), ".m3u", Qt::CaseInsensitive) == 0)
    {
        QStringList m3u = stationfile.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
        if(m3u.first().compare("#EXTM3U") == 0)
        {
            //erweiterte M3U-Datei
            for(int i = 1; i < m3u.size(); i++)
            {
                if(m3u.at(i).left(8).compare("#EXTINF:") == 0)
                {
                    stationname = m3u.at(i).right(m3u.at(i).length() - m3u.at(i).indexOf(",") - 1);
                    stream = m3u.at(i + 1);
                    break;
                }
            }

        }else{
            //einfache M3U-Datei
            stream = m3u.first();
        }
    }
    if(QString::compare(filename.right(4), ".pls", Qt::CaseInsensitive) == 0)
    {
        //Playlist-Datei
        QStringList pls = stationfile.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
        if(pls.first().compare("[playlist]", Qt::CaseInsensitive) == 0)
        {
            for(int i = 1; i < pls.size(); i++)
            {
                if(pls.at(i).left(5).compare("File1", Qt::CaseInsensitive) == 0)
                {
                    stream = pls.at(i).right(pls.at(i).length() - pls.at(i).indexOf("=") - 1);
                }
                if(pls.at(i).left(6).compare("Title1", Qt::CaseInsensitive) == 0)
                {
                    stationname = pls.at(i).right(pls.at(i).length() - pls.at(i).indexOf("=") - 1);
                }
            }
        }
    }
    if(stream.length() > 0)
    {
        qDebug() << "Play Radio-Stream: " << stream;
        play_radio(stream);
        emit station_changed(stationname);
    }
}

void Mpdradio::load_radio(int id)
{
    QString uri = CSettingsStorage::getInstance()->getRadioStationFile(id);
    stationname = CSettingsStorage::getInstance()->getRadioStationName(id);
    if((QString::compare(uri.trimmed().right(4), ".m3u",Qt::CaseInsensitive) == 0) ||
            (QString::compare(uri.trimmed().right(4), ".pls",Qt::CaseInsensitive) == 0))
    {
        filename = uri;
        m_netmanager->get(QNetworkRequest(QUrl(uri)));
    }else{
        qDebug() << "Play Radio-Stream: " << uri.trimmed();
        play_radio(uri.trimmed());
    }
}



#ifndef MPDRADIO_H
#define MPDRADIO_H

#include <QObject>
#include <iostream>
#include <string>
#include <mpd/client.h>
#include <HelperStructs/CSettingsStorage.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

using namespace std;

class Mpdradio : public QObject
{
    Q_OBJECT
public:
    explicit Mpdradio(uint mpdport = 6600, QObject *parent = 0);
    ~Mpdradio();
    bool m_initilised;

signals:
    void station_changed(QString);

public slots:
    void play_radio(const QString &uri);
    void load_radio(int id);
    void stop_radio();

private slots:
    void sl_playlistfetch_done(QNetworkReply* reply);

private:
    mpd_connection *m_mpd_connection;
    QNetworkAccessManager *m_netmanager;
    QString filename;
    QString stationname;
    QString stream;

};

#endif // MPDRADIO_H

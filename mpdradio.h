#ifndef MPDRADIO_H
#define MPDRADIO_H

#include <QObject>
#include <iostream>
#include <string>
#include <mpd/client.h>

using namespace std;

class Mpdradio : public QObject
{
    Q_OBJECT
public:
    explicit Mpdradio(uint mpdport = 6600, QObject *parent = 0);
    ~Mpdradio();
    bool m_initilised;

signals:

public slots:
//    void play_radio(QString& uri);
    void play_radio();
private:
    mpd_connection *m_mpd_connection;
};

#endif // MPDRADIO_H

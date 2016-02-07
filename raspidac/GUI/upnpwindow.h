#ifndef UPNPWINDOW_H
#define UPNPWINDOW_H

#include <string>
#include <iostream>

#include <QWidget>
#include <QImageReader>
#include <QLabel>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QFontDatabase>

#include "HelperStructs/Helper.h"
#include "ticker.h"


namespace Ui {

class UpnpWindow;
}

class UpnpWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UpnpWindow(QWidget *parent = 0);
    ~UpnpWindow();

signals:


public slots:
    void stopped();
    void paused();
    void playing();
    void setCurrentPosition(quint32);
    void update_track(const MetaData& md);
    void clear_track();
    void new_transport_state(int tps, const char *);

private slots:
    void sl_cover_fetch_done(QNetworkReply* reply);

private:
    Ui::UpnpWindow *ui;

    QLabel *m_label;
    Ticker *m_ticker;

    bool m_playing;
    quint32 m_completeLength_ms;
    bool m_metadata_available;

    MetaData m_metadata;

    QNetworkAccessManager *m_netmanager;

    void fetch_cover(const QString& URI);
    void no_cover_available();
};

#endif // UPNPWINDOW_H

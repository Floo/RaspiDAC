#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <iostream>

#include <QMainWindow>
#include <QImageReader>
#include <QLabel>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "HelperStructs/Helper.h"
#include "ticker.h"


namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void sig_pause();
    void sig_play();
    void sig_stop();
    void sig_forward();
    void sig_backward();
//    void sig_clear_playlist();
//    void sig_insert_tracks(MetaDataList&, int);
//    void sig_radio(const MetaData&, int, bool);
//    void sig_playradio(QString);
    void sig_playradio();

public slots:
    void stopped();
    void paused();
    void playing();
    void setCurrentPosition(quint32);
    void update_track(const MetaData& md);
    void new_transport_state(int tps, const char *);

private slots:
    void pause();
    void play();
    void stop();
    void forward();
    void backward();
    void radio();
    void sl_cover_fetch_done(QNetworkReply* reply);

private:
    Ui::MainWindow *ui;

    QLabel *m_label;
    Ticker *m_ticker;

    bool m_playing;
    quint32 m_completeLength_ms;
    bool m_metadata_available;

    MetaData m_metadata;

    QNetworkAccessManager *m_netmanager;

    void fetch_cover(const QString& URI);
    void total_time_changed(qint64 total_time);
    void no_cover_available();
};

#endif // MAINWINDOW_H

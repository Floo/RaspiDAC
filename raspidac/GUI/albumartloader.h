#ifndef ALBUMARTLOADER_H
#define ALBUMARTLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QImageReader>
#include <QLabel>

class AlbumArtLoader : public QObject
{
    Q_OBJECT
public:
    explicit AlbumArtLoader(QObject *parent = 0);
    ~AlbumArtLoader();
    void fetchAlbumArt(const QString& URI, QLabel *label);

signals:

public slots:

private slots:
    void sl_cover_fetch_done(QNetworkReply* reply);

private:
    QNetworkAccessManager *m_netmanager;
    void no_cover_available();
    QLabel *m_label;
};

#endif // ALBUMARTLOADER_H

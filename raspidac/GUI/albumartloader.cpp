#include "albumartloader.h"

AlbumArtLoader::AlbumArtLoader(QObject *parent) : QObject(parent)
{
    m_netmanager = new QNetworkAccessManager(this);
    connect(m_netmanager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(sl_cover_fetch_done(QNetworkReply*)));
}

AlbumArtLoader::~AlbumArtLoader()
{
    delete m_netmanager;
}

void AlbumArtLoader::fetchAlbumArt(const QString& URI, QLabel *label)
{
    m_label = label;
    if (URI.isEmpty()) {
        no_cover_available();
        return;
    }
    if (!m_netmanager) {
        return;
    }
    m_netmanager->get(QNetworkRequest(QUrl(URI)));
}

void AlbumArtLoader::sl_cover_fetch_done(QNetworkReply* reply)
{
    //qDebug() << "GUI_Player::sl_cover_fetch_done";
    if (reply->error() != QNetworkReply::NoError) {
        no_cover_available();
        return;
    }

    QString smime =
        reply->header(QNetworkRequest::ContentTypeHeader).toString();
    int scolon;
    if ((scolon = smime.indexOf(";")) > 0) {
        smime = smime.left(scolon);
    }
    QByteArray imtype;
    if (!smime.compare("image/png", Qt::CaseInsensitive)) {
        imtype = "PNG";
    } else     if (!smime.compare("image/jpeg", Qt::CaseInsensitive)) {
        imtype = "JPG";
    } else     if (!smime.compare("image/gif", Qt::CaseInsensitive)) {
        imtype = "GIF";
    } else {
        qDebug() << "GUI_Player::sl_cover_fetch_done: unsupported mime type: "<<
            smime;
        no_cover_available();
        return;
    }
    QImageReader reader((QIODevice*)reply, imtype);
    reader.setAutoDetectImageFormat(false);

    QImage image;
    if (!reader.read(&image)) {
        qDebug() << "GUI_Player::sl_vover_fetch_done: image read failed " <<
            reader.errorString();
        no_cover_available();
        return;
    }

    QPixmap pixmap;
    pixmap.convertFromImage(image);
    m_label->setPixmap(pixmap);
    reply->deleteLater();
}

void AlbumArtLoader::no_cover_available()
{
    m_label->setPixmap(QPixmap(QString(":/pics/resources/logo.png")));
}


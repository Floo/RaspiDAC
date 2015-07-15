#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "HelperStructs/Helper.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), m_playing(false), m_completeLength_ms(0)
{
    m_metadata_available = false;
    ui->setupUi(this);

    QFont f = ui->label->font();
    f.setPixelSize(10);
    ui->label->setFont(f);

    m_ticker = new Ticker(this);
    m_ticker->setGeometry(0, 0, 240, 30);
    m_ticker->move(10, 210);


//    m_label = new QLabel(this);
//    m_label->setText(QString("TEST"));
//    m_label->move(10,220);

    m_netmanager = new QNetworkAccessManager(this);
    connect(m_netmanager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(sl_cover_fetch_done(QNetworkReply*)));

    connect(ui->btnPause, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui->btnStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(ui->btnPlay, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui->btnRadio, SIGNAL(clicked()), this, SLOT(radio()));
    connect(ui->btnForward, SIGNAL(clicked()), this, SLOT(forward()));
    connect(ui->btnBackward, SIGNAL(clicked()), this, SLOT(backward()));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::play()
{
    emit sig_play();
}

void MainWindow::stop()
{
    emit sig_stop();
}

void MainWindow::pause()
{
    emit sig_pause();
}

void MainWindow::forward()
{
    emit sig_forward();
}

void MainWindow::backward()
{
    emit sig_backward();
}

void MainWindow::radio()
{
//    MetaData md;
//    md.filepath = QString("http://rbb.ic.llnwd.net/stream/rbb_radioeins_mp3_m_a");
//    md.artist = QString("RadioEins");
//    MetaDataList mdl;
//    mdl.resize(1);
//    mdl[0] = md;

//    emit sig_radio(md, 0, true);
//    emit sig_clear_playlist();
//    emit sig_insert_tracks(mdl, 0);
//    emit sig_playradio(QString("http://rbb.ic.llnwd.net/stream/rbb_radioeins_mp3_m_a"));
    emit sig_playradio();
}

void MainWindow::stopped()
{
        //qDebug() << "void GUI_Player::paused()";
    ui->lblState->setPixmap(QPixmap(QString(":/new/resources/stop.png")));
//    ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "stop.png"));
    m_playing = false;
}

void MainWindow::paused()
{
    //qDebug() << "void GUI_Player::paused()";
    ui->lblState->setPixmap(QPixmap(QString(":/new/resources/pause.png")));
    //ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    m_playing = false;
}

void MainWindow::playing(){
    //qDebug() << "void GUI_Player::playing()";
    ui->lblState->setPixmap(QPixmap(QString(":/new/resources/play.png")));
    //ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
    m_playing = true;
}

void MainWindow::setCurrentPosition(quint32 pos_sec)
{
    //qDebug() << "GUI_Player::setCurrentPosition: " << pos_sec << " S" <<
    // "song len " << m_completeLength_ms << " mS";
    if (m_completeLength_ms != 0) {
        int newSliderVal = (pos_sec * 100000) / (m_completeLength_ms);
        ui->songProgress->setValue(newSliderVal);
    }
    QString curPosString = Helper::cvtMsecs2TitleLengthString(pos_sec * 1000);
    ui->lblCurTime->setText(curPosString);
}

void MainWindow::update_track(const MetaData& md){
    if (!m_metadata.compare(md)) {
        return;
    }
    m_metadata = md;

    cerr << "Didl " << md.didl.toStdString() << endl;

    m_completeLength_ms = md.length_ms;
    total_time_changed(md.length_ms);

//    QString tmp = QString("<font color=\"#FFAA00\" size=\"+10\">");
//    if (md.bitrate < 96000) {
//        tmp += "*";
//    } else if (md.bitrate < 128000) {
//        tmp += "**";
//    } else if (md.bitrate < 160000) {
//        tmp += "***";
//    } else if (md.bitrate < 256000) {
//        tmp += "****";
//    } else {
//        tmp += "*****";
//    }
//    tmp += "</font>";

//    ui->lab_rating->setText(tmp);

    //this->setWindowTitle(QString("Upplay - ") + md.title);

    ui->lblAlbum->setText(md.album);
    ui->lblArtist->setText(md.artist);
    ui->lblTitel->setText(md.title);

    m_ticker->setText(md.artist + QString(" - ") + md.title);
    m_ticker->start();

    if (!md.albumArtURI.isEmpty()) {
        fetch_cover(md.albumArtURI);
    } else {
        no_cover_available();
       //ui->albumCover->setIcon(QIcon(Helper::getIconPath() + "logo.png"));
    }

    ui->songProgress->setEnabled(true);

    m_metadata_available = true;

    this->repaint();
}

void MainWindow::total_time_changed(qint64 total_time)
{
    QString length_str = Helper::cvtMsecs2TitleLengthString(total_time, true);
    m_completeLength_ms = total_time;
    ui->lblMaxTime->setText(length_str);
}

void MainWindow::fetch_cover(const QString& URI)
{
    if (!m_netmanager) {
        return;
    }
    m_netmanager->get(QNetworkRequest(QUrl(URI)));
}

void MainWindow::sl_cover_fetch_done(QNetworkReply* reply)
{
    qDebug() << "GUI_Player::sl_cover_fetch_done";
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
    ui->lblAlbumArt->setPixmap(pixmap);
//    ui->albumCover->setIcon(QIcon(pixmap));
//    ui->albumCover->repaint();
    reply->deleteLater();
}

void MainWindow::no_cover_available()
{
    ui->lblAlbumArt->setPixmap(QPixmap(QString(":/new/resources/logo.png")));
}

void MainWindow::new_transport_state(int tps, const char *)
{
    //m_tpstate = tps;
    switch (tps) {
    case AUDIO_UNKNOWN:
    case AUDIO_STOPPED:
    default:
        stopped();
        break;
    case AUDIO_PLAYING:
        playing();
        break;
    case AUDIO_PAUSED:
        paused();
        break;
    }
}



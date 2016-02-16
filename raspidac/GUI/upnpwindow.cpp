#include "upnpwindow.h"
#include "ui_upnpwindow.h"
#include "progressbar.h"
#include "ticker.h"
#include "albumartloader.h"

#include "HelperStructs/Helper.h"

UpnpWindow::UpnpWindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::UpnpWindow), m_playing(false), m_completeLength_ms(0)
{
    m_metadata_available = false;
    ui->setupUi(this);

    m_ticker = new Ticker(this);
    m_ticker->setLength(450);
    m_ticker->setPos(18, 5);
    m_ticker->setFontSize(24, true);

    m_progressbar = new ProgressBar(this);
    m_progressbar->setGeometry(0, 0, 180, 11);
    m_progressbar->move(20, 300);

    m_albumartloader = new AlbumArtLoader(this);
}

UpnpWindow::~UpnpWindow()
{
    delete ui;
}


void UpnpWindow::stopped()
{
        //qDebug() << "void GUI_Player::paused()";
    ui->lblState->setPixmap(QPixmap(QString(":/pics/resources/stop.png")));
    m_playing = false;
}

void UpnpWindow::paused()
{
    //qDebug() << "void GUI_Player::paused()";
    ui->lblState->setPixmap(QPixmap(QString(":/pics/resources/pause.png")));
    m_playing = false;
}

void UpnpWindow::playing(){
    //qDebug() << "void GUI_Player::playing()";
    ui->lblState->setPixmap(QPixmap(QString(":/pics/resources/play.png")));
    m_playing = true;
}


void UpnpWindow::setCurrentPosition(quint32 pos_sec)
{
    m_progressbar->setValue(pos_sec * 1000);

    QString curPosString = Helper::cvtMsecs2TitleLengthString(pos_sec * 1000);
    QString lengthString = Helper::cvtMsecs2TitleLengthString(m_completeLength_ms, true);
    ui->lblCurTime->setText(QString(curPosString + " / " + lengthString));
}

void UpnpWindow::update_track(const MetaData& md){
    if (!m_metadata.compare(md)) {
        return;
    }
    m_metadata = md;

    //cerr << "Didl " << md.didl.toStdString() << endl;

    m_completeLength_ms = md.length_ms;

    m_progressbar->setMaximum(md.length_ms);

    ui->lblAlbum->setText(md.album);
    ui->lblArtist->setText(md.artist);
    ui->lblTitel->setText(md.title);
    if (md.bitrate > 0)
    {
        ui->lblBitrate->setText(QString("%1 kBit/s").arg((8 * md.bitrate) / 1000));
    }
    else
    {
        ui->lblBitrate->setText("");
    }
    m_ticker->setText(md.artist + QString(" - ") + md.title);
    m_ticker->start();

    m_albumartloader->fetchAlbumArt(md.albumArtURI, ui->lblAlbumArt);

    m_metadata_available = true;
    this->repaint();
}

void UpnpWindow::clear_track()
{
    ui->lblAlbum->setText("");
    ui->lblArtist->setText("Musicplayer");
    ui->lblTitel->setText("");
    ui->lblCurTime->setText("0:00");
    ui->lblBitrate->setText("");

    m_ticker->setText("");

    ui->lblAlbumArt->setPixmap(QPixmap(QString(":/pics/resources/logo.png")));
}



void UpnpWindow::new_transport_state(int tps, const char *)
{
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



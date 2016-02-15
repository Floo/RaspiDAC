#include "radiowindow.h"
#include "ui_radiowindow.h"
#include "albumartloader.h"
#include "ticker.h"

RadioWindow::RadioWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RadioWindow)
{
    ui->setupUi(this);

    m_albumartloader = new AlbumArtLoader(this);
    m_stationtext = new Ticker(this);
    m_stationtext->setGeometry(ui->lblStationtext->geometry());
    m_stationtext->move(ui->lblStationtext->pos());
    ui->lblStationtext->setVisible(false);

}

RadioWindow::~RadioWindow()
{
    delete ui;
}


void RadioWindow::newStationName(QString stationname)
{
    ui->lblStationname->setText(stationname);
}

void RadioWindow::newRadioState(QString state)
{
    if(state.compare("play") == 0)
    {
        ui->lblState->setPixmap(QPixmap(QString(":/pics/resources/play.png")));
    }else{
        ui->lblState->setPixmap(QPixmap(QString(":/pics/resources/pause.png")));
    }
}

void RadioWindow::update_track(const MetaData& md)
{
    if (md.length_ms < 1)
    {
        ui->lblStationname->setText(md.album);
        //ui->lblStationtext->setText(md.artist);
        m_stationtext->setText(md.artist);
        m_stationtext->start();

        ui->lblNowPlaying->setText(md.title);

        m_albumartloader->fetchAlbumArt(md.albumArtURI, ui->lblAlbumArt);
    }
}

void RadioWindow::clear_track()
{
    ui->lblStationname->setText("");
    ui->lblStationtext->setText("");
    ui->lblNowPlaying->setText("");

    ui->lblAlbumArt->setPixmap(QPixmap(QString(":/pics/resources/logo.png")));
}

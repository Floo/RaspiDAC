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

    m_stationName = new Ticker(this);
    m_stationName->setLength(421);
    m_stationName->setPos(30, 60);
    m_stationName->setFontSize(20, true);

    m_stationText = new Ticker(this);
    m_stationText->setLength(421);
    m_stationText->setPos(30, 110);
    m_stationText->setFontSize(14, false);

    m_nowPlaying = new Ticker(this);
    m_nowPlaying->setLength(421);
    m_nowPlaying->setPos(30, 140);
    m_nowPlaying->setFontSize(14, false);
}

RadioWindow::~RadioWindow()
{
    delete ui;
}


void RadioWindow::newStationName(QString stationname)
{
    m_stationName->setText(stationname);
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
        m_stationName->setText(md.album);
        m_stationName->start();

        m_stationText->setText(md.artist);
        m_stationText->start();

        m_nowPlaying->setText(md.title);
        m_nowPlaying->start();

        m_albumartloader->fetchAlbumArt(md.albumArtURI, ui->lblAlbumArt);
    }
}

void RadioWindow::clear_track()
{
    m_stationName->setText("");
    m_stationText->setText("");
    m_nowPlaying->setText("");

    ui->lblAlbumArt->setPixmap(QPixmap(QString(":/pics/resources/logo.png")));
}



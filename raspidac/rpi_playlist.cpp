#include "rpi_playlist.h"
#include "raspidac.h"

Rpi_Playlist::Rpi_Playlist(QWidget *parent) :
    QWidget(parent)
{
    m_playRowPending = -1;
}

Rpi_Playlist::~Rpi_Playlist()
{

}


void Rpi_Playlist::fillPlaylist(MetaDataList& v_metadata, int cur_play_idx, int)
{
    RaspiDAC *rpi = static_cast<RaspiDAC*>(parent());
    OHProductQO::SourceType st = rpi->getSourceType();
    qDebug() << "Rpi_Playlist::fillPlaylist: SourceType = " << st << " cur_play_idx = " << cur_play_idx;
    if (st == OHProductQO::OHPR_SourceRadio)
    {
        if (st== OHProductQO::OHPR_SourceRadio) {
            _radioList.clear();
            foreach(MetaData md, v_metadata) {
                _radioList.append(md.title);
            }
            emit radioListChanged(_radioList);
            rpi->prepareDatagram(false, true);
        }
    }
}

void Rpi_Playlist::track_changed(int)
{

}

// This is for restoring from settings
void Rpi_Playlist::setMode(Playlist_Mode mode)
{
    _playlist_mode = mode;
}

// This is for setting the modes from an openhome player. Only the
// repeat and shuffle bits
void Rpi_Playlist::setPlayerMode(Playlist_Mode mode)
{
    _playlist_mode.repAll = mode.repAll;
    _playlist_mode.shuffle = mode.shuffle;
}

void Rpi_Playlist::psl_show_small_playlist_items(bool)
{

}

void Rpi_Playlist::psl_next_group_html(QString)
{

}

QStringList* Rpi_Playlist::getRadioList()
{
    return &_radioList;
}

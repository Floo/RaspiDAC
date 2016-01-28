#include "rpi_playlist.h"

Rpi_Playlist::Rpi_Playlist(QWidget *parent) :
    QWidget(parent), _sourceType(OHProductQO::OHPR_SourceUnknown)
{

}

Rpi_Playlist::~Rpi_Playlist()
{

}

// SLOT: fill all tracks in v_metadata into playlist
void Rpi_Playlist::fillPlaylist(MetaDataList& v_metadata, int cur_play_idx, int)
{
    if (_sourceType == OHProductQO::OHPR_SourceRadio) {
        _radioList.clear();
        foreach(MetaData md, v_metadata) {
            _radioList.append(md.title);
        }

    } else if (_sourceType == OHProductQO::OHPR_SourcePlaylist) {
        _playList.clear();
        foreach (MetaData md, v_metadata) {
            _playList.append(md.artist);
        }
    }
}

void Rpi_Playlist::track_changed(int row)
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

void Rpi_Playlist::psl_show_small_playlist_items(bool small_items)
{

}

void Rpi_Playlist::psl_next_group_html(QString html)
{

}

void Rpi_Playlist::setSourceType(OHProductQO::SourceType st)
{
    _sourceType = st;
}

QStringList Rpi_Playlist::getRadioList()
{
    return _radioList;
}

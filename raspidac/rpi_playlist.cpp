#include "rpi_playlist.h"

Rpi_Playlist::Rpi_Playlist(QWidget *parent) :
    QWidget(parent), _sourceType(OHProductQO::OHPR_SourceUnknown)
{
    m_playRowPending = -1;
}

Rpi_Playlist::~Rpi_Playlist()
{

}


void Rpi_Playlist::fillPlaylist(MetaDataList& v_metadata, int cur_play_idx, int)
{
    _mdl = v_metadata;

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

void Rpi_Playlist::sourceTypeChanged(OHProductQO::SourceType st)
{
    qDebug() << "Rpi_Playlist::sourceTypeChanged: " << st;

    if (m_playRowPending > -1){
        emit row_activated(m_playRowPending);
        m_playRowPending = -1;
    }

    if(st == _sourceType)
        return;

    _sourceType = st;

    if (_sourceType == OHProductQO::OHPR_SourceRadio) {
        _radioList.clear();
        foreach(MetaData md, _mdl) {
            _radioList.append(md.title);
        }
        emit radioListChanged(_radioList);
    }
    emit sig_source_type_changed(_sourceType);
}

QStringList Rpi_Playlist::getRadioList()
{
    return _radioList;
}

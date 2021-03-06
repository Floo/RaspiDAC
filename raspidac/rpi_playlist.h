#ifndef RPI_PLAYLIST_H
#define RPI_PLAYLIST_H

#include <QObject>
#include <QWidget>

#include "upplay/playlist/playlist.h"
#include "upplay/HelperStructs/MetaData.h"
#include "upplay/HelperStructs/PlaylistMode.h"
#include "../upplay/upqo/ohproduct_qo.h"

class Rpi_Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Rpi_Playlist(QWidget *parent = 0);
    ~Rpi_Playlist();
    //-----Interface zu upplay

    //-----Ende Interface
    QStringList* getRadioList();
    void setPlayRowPending(int row) {
        m_playRowPending = row;
    }

signals:
    //-----Interface zu upplay
    // Bad name, emitted on dbl-click, should be row_activated()
    void row_activated(int);

    // Inform about first row in selection (might want to start there
    // if play is requested)
    void selection_min_row(int);

    void clear_playlist();
    void playlist_mode_changed(const Playlist_Mode&);

    void dropped_tracks(const MetaDataList&, int);
    void sig_rows_removed(const QList<int>&, bool);

    void search_similar_artists(const QString&);
    void sig_no_focus();
    void sig_sort_tno();
    //-----Ende Interface

    void radioListChanged(const QStringList&);

public slots:
    //-----Interface zu upplay
    void fillPlaylist(MetaDataList&, int, int);
    void track_changed(int);
    void setMode(Playlist_Mode mode);
    void setPlayerMode(Playlist_Mode mode);
    void psl_show_small_playlist_items(bool small_items);
    void psl_next_group_html(QString html);
    //-----Ende Interface
private:
    Playlist_Mode _playlist_mode;
    QStringList _radioList;
    int m_playRowPending;

};

#endif // RPI_PLAYLIST_H

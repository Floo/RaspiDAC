#ifndef RPI_PLAYLIST_H
#define RPI_PLAYLIST_H

#include <QObject>
#include <QWidget>

#include "playlist/playlist.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "../upqo/ohproduct_qo.h"

class Rpi_Playlist : public QWidget
{
    Q_OBJECT
public:
    explicit Rpi_Playlist(QWidget *parent = 0);
    ~Rpi_Playlist();
    //-----Interface zu upplay

    //-----Ende Interface
    QStringList getRadioList();

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

public slots:
    //-----Interface zu upplay
    void fillPlaylist(MetaDataList&, int, int);
    void track_changed(int);
    void setMode(Playlist_Mode mode);
    void setPlayerMode(Playlist_Mode mode);
    void psl_show_small_playlist_items(bool small_items);
    void psl_next_group_html(QString html);
    //-----Ende Interface
    void setSourceType(OHProductQO::SourceType);
private:
    Playlist_Mode _playlist_mode;
    OHProductQO::SourceType _sourceType;
    QStringList _radioList;
    QStringList _playList;

};

#endif // RPI_PLAYLIST_H

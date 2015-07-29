#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <iostream>

#include <QObject>
#include <QApplication>

#include "mainwindow.h"
#include "upqo/renderingcontrol_qo.h"
#include "upqo/ohvolume_qo.h"
#include "upqo/ohtime_qo.h"
#include "upadapt/ohpladapt.h"
#include "playlist/PlaylistAVT.h"
#include "upadapt/avtadapt.h"
#include "HelperStructs/CSettingsStorage.h"
#include "mpdradio.h"

#include <libupnpp/upnpplib.hxx>
#include <libupnpp/control/mediarenderer.hxx>
#include <libupnpp/control/renderingcontrol.hxx>
#include <libupnpp/control/discovery.hxx>

#define CONNECT(a,b,c,d) m_app->connect(a, SIGNAL(b), c, SLOT(d), Qt::UniqueConnection)

using namespace UPnPClient;

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QApplication* qapp, int n_files, QObject *parent = 0);
    ~Application();
    bool is_initialized();

signals:

public slots:

private:
    bool m_initialized;
    bool m_hasOHRenderer;

    QApplication *m_app;
    UPnPDeviceDirectory *superdir;
    MainWindow *m_gui;
    CSettingsStorage *m_settings;
    AVTPlayer *m_avto;
    OHPlayer *m_ohplo;
    OHTimeQO *m_ohtmo;
    OHVolumeQO *m_ohvlo;
    RenderingControlQO *m_rdco;
    Mpdradio *m_mpdradio;
//    Playlist *m_playlist;

    void renderer_connections();
    void list_renderer();
    QString getVersion();
};

#endif // APPLICATION_H

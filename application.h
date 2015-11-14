#ifndef APPLICATION_H
#define APPLICATION_H

//#define __rpi__

#include <string>
#include <iostream>

#include <QObject>
#include <QApplication>

#include "upnpwindow.h"
#include "mainwindow.h"
#include "radiowindow.h"
#include "standbywindow.h"
#include "spdifwindow.h"
#include "upqo/renderingcontrol_qo.h"
#include "upqo/ohvolume_qo.h"
#include "upqo/ohtime_qo.h"
#include "upadapt/ohpladapt.h"
#include "playlist/PlaylistAVT.h"
#include "upadapt/avtadapt.h"
#include "HelperStructs/CSettingsStorage.h"
#include "mpdradio.h"
#include "rpicontrol/netapiserver.h"

#include <libupnpp/upnpplib.hxx>
#include <libupnpp/control/mediarenderer.hxx>
#include <libupnpp/control/renderingcontrol.hxx>
#include <libupnpp/control/discovery.hxx>

#ifdef __rpi__
#include "rpicontrol/rpigpio.h"
#endif

#define FOO 1

#define CONNECT(a,b,c,d) m_app->connect(a, SIGNAL(b), c, SLOT(d), Qt::UniqueConnection)

using namespace UPnPClient;

class NetAPIServer;

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QApplication* qapp, int n_files, QObject *parent = 0);
    ~Application();
    bool is_initialized();

signals:

public slots:
    void setMode(int);
    void setDACInput(int);
    void setSPDIFInput(int);
    void setBacklight(int);
    void setStandby();
    void new_transport_state(int, const char *);

private:
    bool m_initialized;
    bool m_hasOHRenderer;
    class Internal;
    Internal *m;

    QApplication *m_app;
    UPnPDeviceDirectory *superdir;
    MainWindow *m_mainWindow;
    StandbyWindow *m_standbyWindow;
    RadioWindow *m_radioWindow;
    UpnpWindow *m_upnpWindow;
    SpdifWindow *m_spdifWindow;
    CSettingsStorage *m_settings;
    AVTPlayer *m_avto;
    OHPlayer *m_ohplo;
    OHTimeQO *m_ohtmo;
    OHVolumeQO *m_ohvlo;
    RenderingControlQO *m_rdco;
    Mpdradio *m_mpdradio;
//    Playlist *m_playlist;
#ifdef __rpi__
    RPiGPIO *rpiGPIO;
#endif
    NetAPIServer *netAPIServer;
    void renderer_connections();
    void list_renderer();
    QString getVersion();
};

#endif // APPLICATION_H

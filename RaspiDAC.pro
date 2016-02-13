#-------------------------------------------------
#
# Project created by QtCreator 2015-01-18T13:41:56
#
#-------------------------------------------------

#QT       += core gui script webkit network xml
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

QT       += core gui script network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RaspiDAC
TEMPLATE = app
LANGUAGE = C++
INSTALLS += target


VERSION = 1.0.0

QMAKE_CXXFLAGS += -std=c++0x  -DUPPLAY_VERSION=\\\"$$VERSION\\\" -Wno-psabi
#QMAKE_CXXFLAGS += -DUSING_WEBENGINE

target.path = /home/pi/RaspiDAC

SOURCES += \
#    GUI/mainw/mainw.cpp \
#    GUI/mainw/mw_connections.cpp \
#    GUI/mainw/mw_controls.cpp \
#    GUI/mainw/mw_cover.cpp \
#    GUI/mainw/mw_menubar.cpp \
#    GUI/mainw/trayicon.cpp \
#    GUI/mdatawidget/mdatawidget.cpp \
#    GUI/mdatawidget/mw.cpp \
#    GUI/playctlwidget/playctlwidget.cpp \
#    GUI/playctlwidget/plw.cpp \
#    GUI/playerwidget/playerwidget.cpp \
#    GUI/playerwidget/plyr.cpp \
#    GUI/playlist/delegate/PlaylistItemDelegate.cpp \
#    GUI/playlist/entry/GUI_PlaylistEntryBig.cpp \
#    GUI/playlist/entry/GUI_PlaylistEntrySmall.cpp \
#    GUI/playlist/model/PlaylistItemModel.cpp \
#    GUI/playlist/view/ContextMenu.cpp \
#    GUI/playlist/view/PlaylistView.cpp \
#    GUI/playlist/GUI_Playlist.cpp \
#    GUI/prefs/confgui.cpp \
#    GUI/prefs/prefs.cpp \
#    GUI/prefs/sortprefs.cpp \
#    GUI/progresswidget/progresswidget.cpp \
#    GUI/progresswidget/pw.cpp \
    GUI/songcast/songcastdlg.cpp \
#    GUI/volumewidget/soundslider.cpp \
#    GUI/volumewidget/volumewidget.cpp \
#    GUI/volumewidget/vw.cpp \
#    GUI/widgets/directslider.cpp \
    HelperStructs/Helper.cpp \
    HelperStructs/CSettingsStorage.cpp \
    HelperStructs/Style.cpp \
    application.cpp \
#    dirbrowser/cdbrowser.cpp \
#    dirbrowser/dirbrowser.cpp \
#    dirbrowser/randplayer.cpp \
#    dirbrowser/rreaper.cpp \
    playlist/playlist.cpp \
    playlist/playlistavt.cpp \
    playlist/playlistlocrd.cpp \
    playlist/playlistohpl.cpp \
    playlist/playlistohrd.cpp \
    upadapt/songcast.cpp \
    upadapt/upputils.cpp \
    upplay.cpp \
    upqo/ohpool.cpp \
    raspidac/GUI/mainwindow.cpp \
    raspidac/GUI/radiowindow.cpp \
    raspidac/GUI/spdifwindow.cpp \
    raspidac/GUI/standbywindow.cpp \
    raspidac/GUI/ticker.cpp \
    raspidac/GUI/upnpwindow.cpp \
    raspidac/rpicontrol/netapiserver.cpp \
    raspidac/rpicontrol/netapithread.cpp \
    raspidac/raspidac.cpp \
    raspidac/rpi_playlist.cpp \
    raspidac/GUI/menuitem.cpp \
    raspidac/GUI/mainmenu.cpp \
    raspidac/GUI/menu.cpp

HEADERS  += \
#    GUI/mainw/mainw.h \
#    GUI/mainw/trayicon.h \
#    GUI/mdatawidget/mdatawidget.h \
#    GUI/mdatawidget/mdatawidgetif.h \
#    GUI/mdatawidget/ui_mdatawidget.h \
#    GUI/playctlwidget/playctlwidget.h \
#    GUI/playctlwidget/playctlwidgetif.h \
#    GUI/playerwidget/playerhwidget.h \
#    GUI/playerwidget/playervwidget.h \
#    GUI/playlist/delegate/PlaylistItemDelegate.h \
#    GUI/playlist/delegate/PlaylistItemDelegateInterface.h \
#    GUI/playlist/entry/GUI_PlaylistEntry.h \
#    GUI/playlist/entry/GUI_PlaylistEntryBig.h \
#    GUI/playlist/entry/GUI_PlaylistEntrySmall.h \
#    GUI/playlist/model/PlaylistItemModel.h \
#    GUI/playlist/view/ContextMenu.h \
#    GUI/playlist/view/PlaylistView.h \
#    GUI/playlist/GUI_Playlist.h \
#    GUI/prefs/confgui.h \
#    GUI/prefs/prefs.h \
#    GUI/prefs/sortprefs.h \
#    GUI/progresswidget/progresswidget.h \
#    GUI/progresswidget/progresswidgetif.h \
#    GUI/renderchoose/renderchoose.h \
    GUI/songcast/songcastdlg.h \
#    GUI/sourcechoose/sourcechoose.h \
#    GUI/volumewidget/soundslider.h \
#    GUI/volumewidget/ui_volumewidget.h \
#    GUI/volumewidget/volumewidget.h \
#    GUI/volumewidget/volumewidgetif.h \
#    GUI/volumewidget/vw.h \
#    GUI/widgets/directslider.h \
    HelperStructs/CSettingsStorage.h \
    HelperStructs/Helper.h \
    HelperStructs/PlaylistMode.h \
    HelperStructs/Style.h \
    application.h \
#    dirbrowser/cdbrowser.h \
#    dirbrowser/dirbrowser.h \
#    dirbrowser/randplayer.h \
#    dirbrowser/rreaper.h \
    playlist/playlist.h \
    playlist/playlistavt.h \
    playlist/playlistlocrd.h \
    playlist/playlistnull.h \
    playlist/playlistohpl.h \
    playlist/playlistohrcv.h \
    playlist/playlistohrd.h \
    upadapt/avtadapt.h \
    upadapt/ohpladapt.h \
    upadapt/ohifadapt.h \
    upadapt/ohrdadapt.h \
    upadapt/songcast.h \
    upqo/avtransport_qo.h \
    upqo/cdirectory_qo.h \
    upqo/ohinfo_qo.h \
    upqo/ohplaylist_qo.h \
    upqo/ohproduct_qo.h \
    upqo/ohradio_qo.h \
    upqo/ohreceiver_qo.h \
    upqo/ohvolume_qo.h \
    upqo/ohtime_qo.h \
    upqo/renderingcontrol_qo.h \
    raspidac/GUI/mainwindow.h \
    raspidac/GUI/radiowindow.h \
    raspidac/GUI/spdifwindow.h \
    raspidac/GUI/standbywindow.h \
    raspidac/GUI/ticker.h \
    raspidac/GUI/upnpwindow.h \
    raspidac/rpicontrol/netapiserver.h \
    raspidac/rpicontrol/netapithread.h \    
    raspidac/raspidac.h \
    raspidac/rpi_playlist.h \
    raspidac/GUI/menuitem.h \
    raspidac/GUI/mainmenu.h \
    raspidac/GUI/menu.h \
    HelperStructs/MetaData.h \
    raspidac/rpicontrol/helper.h

FORMS  += \
    raspidac/GUI/mainwindow.ui \
    raspidac/GUI/radiowindow.ui \
    raspidac/GUI/spdifwindow.ui \
    raspidac/GUI/standbywindow.ui \
    raspidac/GUI/upnpwindow.ui \
    GUI/songcast/songcastdlg.ui

RESOURCES += \
    raspidac/raspidac.qrc

LIBS += -lupnpp -lmpdclient

contains(QT_ARCH,arm):{
    message("ARM System")
    DEFINES += __rpi__
    LIBS += -lwiringPi -L/usr/local/lib
    HEADERS += raspidac/rpicontrol/rpigpio.h
    SOURCES +=     raspidac/rpicontrol/rpigpio.cpp
    RENDERER = RaspiDAC
}
else{
    message("x86_64 System")
    RENDERER = LinuxUpMpd
    #RENDERER = RaspiDAC
}

QMAKE_CXXFLAGS += -DRENDERER_NAME=\\\"$$RENDERER\\\"

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/include

DISTFILES +=

UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

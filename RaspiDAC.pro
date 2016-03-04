#-------------------------------------------------
#
# Project created by QtCreator 2015-01-18T13:41:56
#
#-------------------------------------------------

QT       += core gui script network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RaspiDAC
TEMPLATE = app
LANGUAGE = C++
INSTALLS += target


VERSION = 1.0.0

QMAKE_CXXFLAGS += -std=c++0x  -DUPPLAY_VERSION=\\\"$$VERSION\\\" -Wno-psabi

target.path = /home/pi/RaspiDAC

SOURCES += \
    upplay/GUI/songcast/songcastdlg.cpp \
    upplay/HelperStructs/Helper.cpp \
    upplay/HelperStructs/CSettingsStorage.cpp \
    upplay/HelperStructs/Style.cpp \
    upplay/application.cpp \
    upplay/playlist/playlist.cpp \
    upplay/playlist/playlistavt.cpp \
    upplay/playlist/playlistlocrd.cpp \
    upplay/playlist/playlistohpl.cpp \
    upplay/playlist/playlistohrd.cpp \
    upplay/upadapt/songcast.cpp \
    upplay/upadapt/upputils.cpp \
    upplay/upqo/ohpool.cpp \
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
    raspidac/GUI/menu.cpp \
    raspidac/GUI/progressbar.cpp \
    raspidac/GUI/albumartloader.cpp \
    raspidac/GUI/messagewindow.cpp \
    raspidac/rpicontrol/lircreceiver.cpp \
    raspidac/rpicontrol/lircsender.cpp \
    raspidac/rpicontrol/lirccontrol.cpp \
    RaspiDAC.cpp \
    raspidac/rpicontrol/lirctest.cpp

HEADERS  += \
    upplay/GUI/songcast/songcastdlg.h \
    upplay/HelperStructs/CSettingsStorage.h \
    upplay/HelperStructs/Helper.h \
    upplay/HelperStructs/PlaylistMode.h \
    upplay/HelperStructs/Style.h \
    upplay/application.h \
    upplay/playlist/playlist.h \
    upplay/playlist/playlistavt.h \
    upplay/playlist/playlistlocrd.h \
    upplay/playlist/playlistnull.h \
    upplay/playlist/playlistohpl.h \
    upplay/playlist/playlistohrcv.h \
    upplay/playlist/playlistohrd.h \
    upplay/upadapt/avtadapt.h \
    upplay/upadapt/ohpladapt.h \
    upplay/upadapt/ohifadapt.h \
    upplay/upadapt/ohrdadapt.h \
    upplay/upadapt/songcast.h \
    upplay/upqo/avtransport_qo.h \
    upplay/upqo/cdirectory_qo.h \
    upplay/upqo/ohinfo_qo.h \
    upplay/upqo/ohplaylist_qo.h \
    upplay/upqo/ohproduct_qo.h \
    upplay/upqo/ohradio_qo.h \
    upplay/upqo/ohreceiver_qo.h \
    upplay/upqo/ohvolume_qo.h \
    upplay/upqo/ohtime_qo.h \
    upplay/upqo/renderingcontrol_qo.h \
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
    upplay/HelperStructs/MetaData.h \
    raspidac/rpicontrol/helper.h \
    raspidac/GUI/progressbar.h \
    raspidac/GUI/albumartloader.h \
    raspidac/GUI/messagewindow.h \
    raspidac/rpicontrol/lircreceiver.h \
    raspidac/rpicontrol/lircsender.h \
    raspidac/rpicontrol/lirccontrol.h \
    raspidac/rpicontrol/lirctest.h

FORMS  += \
    raspidac/GUI/mainwindow.ui \
    raspidac/GUI/radiowindow.ui \
    raspidac/GUI/spdifwindow.ui \
    raspidac/GUI/standbywindow.ui \
    raspidac/GUI/upnpwindow.ui \
    upplay/GUI/songcast/songcastdlg.ui \
    raspidac/GUI/messagewindow.ui

RESOURCES += \
    raspidac/raspidac.qrc

LIBS += -lupnpp -lmpdclient -llirc_client

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
INCLUDEPATH += upplay
#INCLUDEPATH += /usr/include

DISTFILES +=

UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

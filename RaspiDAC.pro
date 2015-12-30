#-------------------------------------------------
#
# Project created by QtCreator 2015-01-18T13:41:56
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RaspiDAC
TEMPLATE = app
LANGUAGE = C++
INSTALLS += target

QMAKE_CXXFLAGS += -std=c++0x  -DUPPLAY_VERSION=\\\"$$VERSION\\\" -Wno-psabi

target.path = /home/pi/RaspiDAC

SOURCES += \
    RaspiDAC.cpp\
    application.cpp \
    playlist/PlaylistAVT.cpp \
    playlist/Playlist.cpp \
    upadapt/upputils.cpp \
    HelperStructs/Helper.cpp \
    HelperStructs/CSettingsStorage.cpp \
    mpdradio.cpp \
    ticker.cpp \
    upnpwindow.cpp \
    standbywindow.cpp \
    mainwindow.cpp \
    radiowindow.cpp \
    spdifwindow.cpp \
    rpicontrol/netapiserver.cpp \
    rpicontrol/netapithread.cpp

HEADERS  += \
    application.h \
    upqo/renderingcontrol_qo.h \
    upqo/cdirectory_qo.h \
    upqo/avtransport_qo.h \
    playlist/PlaylistAVT.h \
    playlist/Playlist.h \
    upadapt/upputils.h \
    upadapt/avtadapt.h \
    HelperStructs/Helper.h \
    HelperStructs/MetaData.h \
    HelperStructs/CSettingsStorage.h \
    upadapt/ohpladapt.h \
    upqo/ohvolume_qo.h \
    upqo/ohtime_qo.h \
    upqo/ohplaylist_qo.h \
    mpdradio.h \
    ticker.h \
    upnpwindow.h \
    standbywindow.h \
    mainwindow.h \
    radiowindow.h \
    spdifwindow.h \
    rpicontrol/netapiserver.h \
    rpicontrol/netapithread.h

FORMS  += \
    standbywindow.ui \
    mainwindow.ui \
    radiowindow.ui \
    upnpwindow.ui \
    spdifwindow.ui

RESOURCES += \
    RaspiDAC.qrc

LIBS += -lupnpp -lmpdclient

contains(QT_ARCH,arm):{
    message("ARM System")
    DEFINES += __rpi__
    LIBS += -lwiringPi -L/usr/local/lib
    HEADERS += rpicontrol/rpigpio.h
    SOURCES += rpicontrol/rpigpio.cpp
}
else{
    message("x86_64 System")
}

INCLUDEPATH += /usr/local/include

DISTFILES +=

UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

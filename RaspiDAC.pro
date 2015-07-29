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
    mainwindow.cpp \
    application.cpp \
    playlist/PlaylistAVT.cpp \
    playlist/Playlist.cpp \
    upadapt/upputils.cpp \
    HelperStructs/Helper.cpp \
    HelperStructs/CSettingsStorage.cpp \
    mpdradio.cpp \
    ticker.cpp

HEADERS  += \
    mainwindow.h \
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
    ticker.h

FORMS  += \
    mainwindow.ui

RESOURCES += \
    RaspiDAC.qrc

LIBS += -lupnpp -lmpdclient

DISTFILES +=

UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

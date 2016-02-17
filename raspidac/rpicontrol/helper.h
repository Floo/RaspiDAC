#ifndef HELPER_H
#define HELPER_H

#include <QTime>
#include <QCoreApplication>


struct UDPDatagram {

    bool metaDataHasChanged;
    int guiMode;
    int playMode;
    int spdifInput;
    bool radioListHasChanged;
    int port;

    UDPDatagram () {
        metaDataHasChanged = false;
        guiMode = 0;
        playMode = 0;
        spdifInput = 0;
        radioListHasChanged = false;
        port = 0;
    }

    QString toDatagram() {
        QStringList lst;

        lst.append(metaDataHasChanged?"true":"false");
        lst.append(QString::number(guiMode));
        lst.append(QString::number(playMode));
        lst.append(QString::number(spdifInput));
        lst.append(radioListHasChanged?"true":"false");
        lst.append(QString::number(port));
        return lst.join(";");
    }

};

class Delay
{
public:
    static void msleep( int millisecondsToWait )
    {
        QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
        while( QTime::currentTime() < dieTime )
        {
            QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
        }
    }
};


#endif // HELPER_H


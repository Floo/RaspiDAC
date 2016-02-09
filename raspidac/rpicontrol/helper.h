#ifndef HELPER_H
#define HELPER_H


struct UDPDatagram {

    bool metaDataHasChanged;
    int guiMode;
    int playMode;
    int spdifInput;
    bool radioListHasChanged;

    UDPDatagram () {
        metaDataHasChanged = false;
        guiMode = 0;
        playMode = 0;
        spdifInput = 0;
        radioListHasChanged = false;
    }

    QString toDatagram() {
        QStringList lst;

        lst.append(metaDataHasChanged?"true":"false");
        lst.append(QString::number(guiMode));
        lst.append(QString::number(playMode));
        lst.append(QString::number(spdifInput));
        lst.append(radioListHasChanged?"true":"false");
        return lst.join(";");
    }

};


#endif // HELPER_H


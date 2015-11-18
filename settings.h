#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>


class Settings
{
public:
    Settings();
    ~Settings();
    void setRadioStation(int id, QString name, QString file);
    QString getRadioStationName(int id);
    QString getRadioStationFile(int id);
    QString getRadioStationURL(int id);
    void setInputName(int id, QString name);
    QString getInputName(int id);
    void resetInputName();
    void clearRadioStation();

private:
    QSettings *m_settings;
};

#endif // SETTINGS_H

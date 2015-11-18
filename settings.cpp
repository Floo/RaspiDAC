#include "settings.h"

Settings::Settings()
{
    m_settings = new QSettings("JP", "RaspiDAC");
}

Settings::~Settings()
{

}

void Settings::setRadioStation(int id, QString name, QString file)
{
    if(id < 1 || id > 10)
        return;
    m_settings->beginWriteArray("RadioStation");
    m_settings->setArrayIndex(id);
    m_settings->setValue("Name", name);
    m_settings->setValue("File", file);
    m_settings->endArray();
}

QString Settings::getRadioStationName(int id)
{
    if(id < 1 || id > 10)
        return;
    m_settings->beginReadArray("RadioStation");
    m_settings->setArrayIndex(id);
    QString ret = m_settings->value("Name", "");
    m_settings->endArray();
    return ret;
}

QString Settings::getRadioStationFile(int id)
{
    if(id < 1 || id > 10)
        return;
    m_settings->beginReadArray("RadioStation");
    m_settings->setArrayIndex(id);
    QString ret = m_settings->value("File", "");
    m_settings->endArray();
    return ret;
}

void Settings::setInputName(int id, QString name)
{
    if(id < 1 || id > 4)
        return;
    m_settings->beginWriteArray("Input");
    m_settings->setArrayIndex(id);
    m_settings->setValue("Name", name);
    m_settings->endArray();
}

QString Settings::getInputName(int id)
{
    if(id < 1 || id > 4)
        return;
    m_settings->beginReadArray("Input");
    m_settings->setArrayIndex(id);
    QString ret = m_settings->value("Name", name);
    m_settings->endArray();
}

void Settings::resetInputName()
{
    m_settings->beginWriteArray("Input");
    for(int i = 1; i < 5; i++)
    {
        m_settings->setArrayIndex(id);
        m_settings->setValue("Name", "Input " + i);
    }
    m_settings->endArray();
}

void Settings::clearRadioStation()
{
    m_settings->beginWriteArray("RadioStation");
    for(int i = 1; i < 11; i++)
    {
        m_settings->setArrayIndex(id);
        m_settings->setValue("Name", "");
        m_settings->setValue("File", "");
    }
    m_settings->endArray();
}

QString Settings::getRadioStationURL(int id)
{
    QString file = getRadioStationFile(id);

}


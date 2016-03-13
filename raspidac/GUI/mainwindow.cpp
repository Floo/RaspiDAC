#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "upnpwindow.h"
#include "radiowindow.h"
#include "standbywindow.h"
#include "spdifwindow.h"
#include "messagewindow.h"

#include "../raspidac.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    if(QFontDatabase::addApplicationFont(":/fonts/resources/fonts/LiberationSans-Regular.ttf") == -1)
    {
        qDebug() << "Fehler beim Laden von LiberationSans";
    }
    if(QFontDatabase::addApplicationFont(":/fonts/resources/fonts/NotoSans-Regular.ttf") == -1)
    {
        qDebug() << "Fehler beim Laden von NotoSans";
    }
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/LiberationSans-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/NotoSans-Bold.ttf");

    ui->setupUi(this);
    m_standbyWindow = new StandbyWindow();
    m_upnpWindow = new UpnpWindow();
    m_radioWindow = new RadioWindow();
    m_spdifWindow = new SpdifWindow();
    m_messageWindow = new MessageWindow();

    if (addWidget(m_standbyWindow) != RaspiDAC::RPI_Standby)
        qDebug() << "Fehler bei Zuordung der GUI-Windows";
    if (addWidget(m_upnpWindow) != RaspiDAC::RPI_Upnp)
        qDebug() << "Fehler bei Zuordung der GUI-Windows";
    if (addWidget(m_radioWindow) != RaspiDAC::RPI_Radio)
        qDebug() << "Fehler bei Zuordung der GUI-Windows";
    if (addWidget(m_spdifWindow) != RaspiDAC::RPI_Spdif)
        qDebug() << "Fehler bei Zuordung der GUI-Windows";
    m_msgwID = addWidget(m_messageWindow);

    m_msgTimer = new QTimer();
    m_msgTimer->setSingleShot(true);

    connect(m_msgTimer, SIGNAL(timeout()), this, SLOT(hideMessage()));
}


MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::addWidget(QWidget *widget)
{
    return ui->stackedWidget->addWidget(widget);
}

void MainWindow::setCurrentIndex(int index)
{
    if (ui->stackedWidget->currentIndex() == m_msgwID)
    {
        m_lastWindow = index;
    }
    ui->stackedWidget->setCurrentIndex(index);
}

int MainWindow::currentIndex()
{
    if (ui->stackedWidget->currentIndex() == m_msgwID)
    {
        return m_lastWindow;
    }
    return ui->stackedWidget->currentIndex();
}

void MainWindow::updateTrack(const MetaData &metadata)
{
    if (currentIndex() == RaspiDAC::RPI_Upnp)
        m_upnpWindow->update_track(metadata);
    else if ((currentIndex() == RaspiDAC::RPI_Radio) && (metadata.length_ms == 0))
        m_radioWindow->update_track(metadata);
}

void MainWindow::forceUpdateTrackUpnp(const MetaData &md)
{
    m_upnpWindow->update_track(md);
}

void MainWindow::clearTrack()
{
    m_upnpWindow->clear_track();
    m_radioWindow->clear_track();
}

void MainWindow::upnp_setCurrentPosition(quint32 sec)
{
    m_upnpWindow->setCurrentPosition(sec);
}

void MainWindow::stopped()
{
    m_upnpWindow->stopped();
    m_radioWindow->newRadioState("pause");
}

void MainWindow::playing()
{
    m_upnpWindow->playing();
    m_radioWindow->newRadioState("play");
}

void MainWindow::paused()
{
    m_upnpWindow->paused();
    m_radioWindow->newRadioState("pause");
}

void MainWindow::input(QString txt)
{
    m_spdifWindow->setInput(txt);
}

void MainWindow::spdifInput(int input)
{
    m_spdifWindow->setInput(QString("Input %1").arg(input + 1));
    QStringList lst = CSettingsStorage::getInstance()->getSpdifInputNames();
    if (input < lst.size())
        m_spdifWindow->setInputName(lst.at(input));
    else
        m_spdifWindow->setInputName("");
}

void MainWindow::showMessage(QString &msg, int msec)
{
    m_msgTimer->stop();
    m_messageWindow->setMessage(msg);
    if (msec > 0)
    {
        m_msgTimer->start(msec);
    }
    if (ui->stackedWidget->currentIndex() == m_msgwID)
    {
        return;
    }
    m_lastWindow = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(m_msgwID);
}

void MainWindow::hideMessage()
{
    if (ui->stackedWidget->currentIndex() == m_msgwID)
    {
        ui->stackedWidget->setCurrentIndex(m_lastWindow);
    }
    m_msgTimer->stop();
    emit messageWindowClosed();
}

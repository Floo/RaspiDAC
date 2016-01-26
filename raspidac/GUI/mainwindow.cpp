#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    windowMap["standby"] = addWidget(m_standbyWindow);
    windowMap["upnp"] = addWidget(m_upnpWindow);
    windowMap["radio"] = addWidget(m_radioWindow);
    windowMap["spdif"] = addWidget(m_spdifWindow);

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
    ui->stackedWidget->setCurrentIndex(index);
}

int MainWindow::currentIndex()
{
    return ui->stackedWidget->currentIndex();
}

int MainWindow::getIndexByName(QString key)
{
    return windowMap.value(key);
}

void MainWindow::upnp_updateTrack(const MetaData &metadata)
{
    m_upnpWindow->update_track(metadata);
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
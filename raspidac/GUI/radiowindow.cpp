#include "radiowindow.h"
#include "ui_radiowindow.h"

RadioWindow::RadioWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RadioWindow)
{
    ui->setupUi(this);
}

RadioWindow::~RadioWindow()
{
    delete ui;
}


void RadioWindow::newStationName(QString stationname)
{
    ui->lblStationname->setText(stationname);
}

void RadioWindow::newRadioState(QString state)
{
    if(state.compare("play") == 0)
    {
        ui->lblState->setPixmap(QPixmap(QString(":/pics/resources/play.png")));
    }else{
        ui->lblState->setPixmap(QPixmap(QString(":/pics/resources/pause.png")));
    }
}

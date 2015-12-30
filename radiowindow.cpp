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


void RadioWindow::new_station_name(QString stationname)
{
    ui->lblStationname->setText(stationname);
}

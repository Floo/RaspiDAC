#include "standbywindow.h"
#include "ui_standbywindow.h"

StandbyWindow::StandbyWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandbyWindow)
{

    ui->setupUi(this);
}

StandbyWindow::~StandbyWindow()
{
    delete ui;
}

void StandbyWindow::setCurrentTime()
{
    ui->lblUhrzeit->setText(QTime::currentTime().toString("hh:mm"));
}

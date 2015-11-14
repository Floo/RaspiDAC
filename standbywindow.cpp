#include "standbywindow.h"
#include "ui_standbywindow.h"

StandbyWindow::StandbyWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandbyWindow)
{
    //ui->lblUhrzeit->setStyleSheet("QLabel { background-color : red; color : blue; }");
    ui->setupUi(this);
}

StandbyWindow::~StandbyWindow()
{
    delete ui;
}

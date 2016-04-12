#include "spdifwindow.h"
#include "ui_spdifwindow.h"

SpdifWindow::SpdifWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpdifWindow)
{
    ui->setupUi(this);
}

SpdifWindow::~SpdifWindow()
{
    delete ui;
}

void SpdifWindow::setInput(QString txt)
{
    ui->lblInput->setText(txt);
}

void SpdifWindow::setInputName(QString txt)
{
    ui->lblInputName->setText(txt);
}

void SpdifWindow::setCurrentTime()
{
    ui->lblUhrzeit->setText(QTime::currentTime().toString("hh:mm"));
}

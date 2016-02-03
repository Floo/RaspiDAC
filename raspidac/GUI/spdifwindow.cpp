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

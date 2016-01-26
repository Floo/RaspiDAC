#include "standbywindow.h"
#include "ui_standbywindow.h"

StandbyWindow::StandbyWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandbyWindow)
{
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(setCurrentTime()));
    m_timer->setInterval(200);
    m_timer->start();
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

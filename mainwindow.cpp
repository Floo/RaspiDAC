#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

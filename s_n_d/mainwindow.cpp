#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

//	ui->searchPage->setRootPath("/home/roberto");
}

MainWindow::~MainWindow()
{
	delete ui;
}

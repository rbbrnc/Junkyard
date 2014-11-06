#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "basepage.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Remove arrow indicator on button with menu
    ui->optionButton->setStyleSheet("::menu-indicator {image: none;}");

    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
        BasePage *bp = qobject_cast<BasePage *>(ui->stackedWidget->widget(i));
        bp->createOptionsMenu(ui->optionButton);
        connect(bp, SIGNAL(changePage(int)), this, SLOT(onChangePage(int)));
    }

    // Set curent page
    onChangePage(SearchPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_backButton_clicked()
{
    onChangePage(m_prevPage);
}

void MainWindow::onChangePage(int pageIndex)
{
    if (pageIndex == ui->stackedWidget->currentIndex()) {
        return;
    }

    if ((pageIndex < 0) || (pageIndex > ui->stackedWidget->count())) {
        qWarning() << __PRETTY_FUNCTION__ << "Invalid page index:" << pageIndex;
        return;
    }

    // Update previous page
    m_prevPage = ui->stackedWidget->currentIndex();

    // Set options menu for page to be shown
    BasePage *bp = qobject_cast<BasePage *>(ui->stackedWidget->widget(pageIndex));
    QMenu *menu = bp->optionsMenu();
    if (menu) {
        ui->optionButton->setMenu(menu);
        ui->optionButton->setVisible(!menu->isEmpty());
    } else {
        ui->optionButton->setVisible(false);
    }

    // On MainMenuPage don't show the backButton
    ui->backButton->setVisible(pageIndex != SearchPage);
    ui->stackedWidget->setCurrentIndex(pageIndex);
}

void MainWindow::on_actionQuit_triggered()
{
    qDebug() << __func__;
    // Ask to save before exit.
//    if (maybeSave()) {
        close();
//    }
}


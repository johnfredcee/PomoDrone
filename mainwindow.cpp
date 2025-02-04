#include "mainwindow.h"
#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QAction>

MainWindow::MainWindow(QWidget *parent) : QDialog(parent), timer(new QTimer(this))
{
    // ensure window is not initally visible
    setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowFlag(Qt::FramelessWindowHint, true);

    // System Tray Icon
    trayIcon = std::make_unique<QSystemTrayIcon>(QIcon(":/icons/tray_icon.png"), this);

    // Menu for Tray Icon
    trayMenu = std::make_unique<QMenu>(this);

    showAction = std::make_unique<QAction>("Show Tasks", this);
    hideAction = std::make_unique<QAction>("Hide Tasks", this);
    startAction = std::make_unique<QAction>("Start Pomodoro", this);
    stopAction = std::make_unique<QAction>("Stop Pomodoro", this);
    pauseAction = std::make_unique<QAction>("Pause Pomodoro", this);
    quitAction = std::make_unique<QAction>("Quit", this);

    connect(showAction.get(), &QAction::triggered, this, &MainWindow::showTasks);
    connect(hideAction.get(), &QAction::triggered, this, &MainWindow::hideTasks);
    connect(startAction.get(), &QAction::triggered, this, &MainWindow::startPomodoro);
    connect(stopAction.get(), &QAction::triggered, this, &MainWindow::stopPomodoro);
    connect(pauseAction.get(), &QAction::triggered, this, &MainWindow::pausePomodoro);

    connect(quitAction.get(), &QAction::triggered, qApp, [this]() {
        if (QMessageBox::question(this, "Quit", "Are you sure you want to quit?")
            == QMessageBox::Yes) {
            QApplication::quit();
        }
    });

    trayMenu->addAction(showAction.get());
    trayMenu->addAction(hideAction.get());
    trayMenu->addSeparator();
    trayMenu->addAction(startAction.get());
    trayMenu->addAction(stopAction.get());
    trayMenu->addAction(pauseAction.get());
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction.get());

    trayIcon->setContextMenu(trayMenu.get());
    trayIcon->show();

    connect(timer.get(), &QTimer::timeout, this, &MainWindow::showNotification);
}

MainWindow::~MainWindow() { }

void MainWindow::startPomodoro()
{
    if (timer->isActive()) {
        trayIcon->showMessage("Pomodoro Already Running", "Stay focused!",
                              QSystemTrayIcon::Information);
        return;
    }
    timer->start(25 * 60 * 1000); // 25 minutes
    trayIcon->showMessage("Pomodoro Started", "Focus for 25 minutes!",
                          QSystemTrayIcon::Information);
}

void MainWindow::stopPomodoro()
{
    if (timer->isActive())
    {
        timer->stop();
    }
}

void MainWindow::pausePomodoro()
{
    if (timer->isActive())
    {
        int remainingTime = timer->remainingTime();
        timer->stop();
        timer->setInterval(remainingTime);
    }
}

void MainWindow::showNotification()
{
    trayIcon->showMessage("Pomodoro Over!", "Take a break!", QSystemTrayIcon::Information);
    timer->stop();
}

void MainWindow::showTasks()
{
    setVisible(true);
}

void MainWindow::hideTasks()
{
    setVisible(false);
}
#include "mainwindow.h"
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QAction>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) : QDialog(parent), timer(new QTimer(this))
{
    // ensure window is not initally visible
    setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowFlag(Qt::FramelessWindowHint, true);

    // System Tray Icon ----------------------------------------
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

    // Task Management UI ----------------------------------------
    QVBoxLayout *layout = new QVBoxLayout(this);

    taskListWidget = new QListWidget(this);
    layout->addWidget(taskListWidget);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    taskInput = new QLineEdit(this);
    taskInput->setPlaceholderText("Enter task description...");
    pomodoroInput = new QSpinBox(this);
    pomodoroInput->setRange(1, 10);
    pomodoroInput->setValue(1);
    
    inputLayout->addWidget(taskInput);
    inputLayout->addWidget(pomodoroInput);
    
    layout->addLayout(inputLayout);

    addTaskButton = new QPushButton("➕ Add Task", this);
    removeTaskButton = new QPushButton("🗑️ Remove Task", this);
    layout->addWidget(addTaskButton);
    layout->addWidget(removeTaskButton);

    connect(addTaskButton, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(removeTaskButton, &QPushButton::clicked, this, &MainWindow::removeSelectedTask);

    setLayout(layout);

    loadTasks();
    
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


void MainWindow::addTask() {
    QString description = taskInput->text().trimmed();
    int pomodoroCount = pomodoroInput->value();

    if (description.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Task description cannot be empty.");
        return;
    }

    // Add to the list
    tasks.push_back({description, pomodoroCount});
    taskListWidget->addItem(description + " (" + QString::number(pomodoroCount) + " Pomodoros)");

    // Clear input
    taskInput->clear();
    pomodoroInput->setValue(1);
    
    saveTasks(); 
}

void MainWindow::removeSelectedTask() {
    int row = taskListWidget->currentRow();
    if (row >= 0) {
        taskListWidget->takeItem(row);
        tasks.erase(tasks.begin() + row);
        saveTasks();
    }
}

void MainWindow::saveTasks() {
    QSettings settings("pomos.ini", QSettings::IniFormat);
 
    settings.beginWriteArray("tasks");
    for (int i = 0; i < tasks.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("description", tasks[i].description);
        settings.setValue("pomodoros", tasks[i].pomodoros);
    }
    settings.endArray();
}

void MainWindow::loadTasks() {
    QSettings settings("pomos.ini", QSettings::IniFormat);
 
    int size = settings.beginReadArray("tasks");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString description = settings.value("description").toString();
        int pomodoros = settings.value("pomodoros").toInt();

        tasks.push_back({description, pomodoros});
        taskListWidget->addItem(description + " (" + QString::number(pomodoros) + " Pomodoros)");
    }
    settings.endArray();
}
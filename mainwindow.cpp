#include "mainwindow.h"
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QAction>
#include <QSettings>
#include <memory>

MainWindow::MainWindow(QWidget *parent) : QDialog(parent)
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
    startAction = std::make_unique<QAction>("End Pomodoro", this);
    endAction = std::make_unique<QAction>("Abandon Pomodoro", this);
    pauseAction = std::make_unique<QAction>("Pause Pomodoro", this);
    quitAction = std::make_unique<QAction>("Quit", this);

    connect(showAction.get(), &QAction::triggered, this, &MainWindow::showTasks);
    connect(hideAction.get(), &QAction::triggered, this, &MainWindow::hideTasks);   
    connect(startAction.get(), &QAction::triggered, this, &MainWindow::startPomodoro);
    connect(endAction.get(), &QAction::triggered, this, &MainWindow::abandonPomodoro);
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
    trayMenu->addAction(endAction.get());
    trayMenu->addAction(pauseAction.get());
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction.get());

    trayIcon->setContextMenu(trayMenu.get());
    trayIcon->show();

    // Timer ----------------------------------------
    breakTimer = std::make_unique<QTimer>();
    pomoTimer = std::make_unique<QTimer>();
    bIsPaused = false;


    connect(pomoTimer.get(), &QTimer::timeout, this, &MainWindow::updateElapsedTime);
    connect(breakTimer.get(), &QTimer::timeout, this, &MainWindow::updateElapsedTime);

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


void MainWindow::startBreakTimer()
{
    pomoTimer->stop();
    breakTimer->setInterval(1000);
    breakTimer->start();
    elapsedBreakTime = breaktime;

}

void MainWindow::startPomoTimer()
{
    breakTimer->stop();
    if (!bIsPaused)
    {
        pomoTimer->start(1000);
    }
    else {
        pomoTimer->start();
        bIsPaused = false;
    }
}

void MainWindow::startPomodoro()
{
    if (pomoTimer->isActive()) {
        trayIcon->showMessage("Pomodoro Already Running", "Stay focused!",
                              QSystemTrayIcon::Information);
        return;
    }
    else {
        startPomoTimer();
        trayIcon->showMessage("Pomodoro Started", "Focus for 25 minutes!",  QSystemTrayIcon::Information);
        startAction->setEnabled(false);
        pauseAction->setEnabled(true);
        endAction->setEnabled(true);
    }
}

void MainWindow::abandonPomodoro()
{
    if (pomoTimer->isActive())
    {
        Task& currentTask = tasks.front();
        currentTask.elapsedtime = currentTask.pomodoros * pomotime;
        pomoTimer->stop();
        startAction->setEnabled(true);
        pauseAction->setEnabled(false);
        endAction->setEnabled(false);
    }
}

void MainWindow::stopPomodoro()
{
    if (pomoTimer->isActive())
    {
        Task& currentTask = tasks.front();
        currentTask.pomodoros--;
        if (currentTask.pomodoros == 0)
        {
            tasks.pop_front();
            delete taskListWidget->takeItem(0);
        }
        startBreakTimer();
        startAction->setEnabled(true);
        pauseAction->setEnabled(false);
        endAction->setEnabled(false);
    }
}

void MainWindow::pausePomodoro()
{
    if (pomoTimer->isActive())
    {
        int remainingTime = pomoTimer->remainingTime();
        pomoTimer->stop();
        pomoTimer->setInterval(remainingTime);
        bIsPaused = true;
    }
}

void MainWindow::showNotification()
{
    trayIcon->showMessage("Pomodoro Over!", "Take a break!", QSystemTrayIcon::Information);
 
}

void MainWindow::endBreak()
{
    breakTimer->stop();
    startPomodoro();
}

void MainWindow::updateElapsedTime()
{
    if (pomoTimer->isActive())
    {
        Task& currentTask = tasks.front();
        int elapsedTime  = currentTask.elapsedtime;
        elapsedTime++;
        int remaining = pomotime * currentTask.pomodoros - elapsedTime; // Time left in seconds
        int minutes = remaining / 60;
        int seconds = remaining % 60;
         // Update the system tray tooltip
        trayIcon->setToolTip(QString("Pomodoro: %1:%2 left").arg(minutes).arg(seconds, 2, 10, QChar('0')));
        if (remaining % pomotime == 0)
        {
            showNotification();
            stopPomodoro();
        }
        currentTask.elapsedtime = elapsedTime;   
    }
    if (breakTimer->isActive())
    {
        int remaining = breaktime - elapsedBreakTime; // Time left in seconds
        int minutes = remaining / 60;
        int seconds = remaining % 60;
        trayIcon->setToolTip(QString("Break %1:%2 left").arg(minutes).arg(seconds, 2, 10, QChar('0')));        
        if (remaining == 0)
        {
            endBreak();
        }
    }
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
    tasks.push_back({description, pomodoroCount, 0});
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
        settings.setValue("elapsedTime", tasks[i].elapsedtime);
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
        int elapsedTime = settings.value("elapsedTime").toInt();
        tasks.push_back({description, pomodoros});
        taskListWidget->addItem(description + " (" + QString::number(pomodoros) + " Pomodoros)");
    }
    settings.endArray();
}
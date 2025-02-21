#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTimer>
#include <Qdialog>
#include "deque"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// pomo time in secconds
constexpr int pomotime = 20 * 60;

struct Task {
    QString description;
    int pomodoros;
    int elapsedtime;
};

class MainWindow : public QDialog {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startPomodoro();
    void stopPomodoro();
    void pausePomodoro();
    void showNotification();
    void showTasks();
    void hideTasks();
    void addTask();
    void removeSelectedTask();
    void saveTasks();
    void loadTasks();
    void updateElapsedTime();

private:
    std::deque<Task> tasks;

    QListWidget *taskListWidget;
    QLineEdit *taskInput; 
    QSpinBox *pomodoroInput;
    QPushButton *addTaskButton;
    QPushButton *removeTaskButton;

    std::unique_ptr<QSystemTrayIcon> trayIcon;
    std::unique_ptr<QMenu> trayMenu;
    std::unique_ptr<QAction> showAction;
    std::unique_ptr<QAction> startAction;
    std::unique_ptr<QAction> stopAction;
    std::unique_ptr<QAction> pauseAction;
    std::unique_ptr<QAction> hideAction;
    std::unique_ptr<QAction> quitAction;
    std::unique_ptr<QTimer> timer;
    std::unique_ptr<QTimer> auxTimer;

    bool bIsPaused;
};

#endif // MAINWINDOW_H

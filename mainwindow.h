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
#include "vector"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct Task {
    QString description;
    int pomodoros;
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
    
private:
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

    std::vector<Task> tasks;

};

#endif // MAINWINDOW_H

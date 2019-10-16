#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QJsonObject>
#include <QPushButton>

#include "src/driver/tcp_driver_interface.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void chartUpdate(RobotData data);
    void inProgressSignal(qint64 value);
    void progressFinighSignal();

private slots:
    void on_tabWidget_tabCloseRequested(int index);
    void onPushbuttonCreateClicked();
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonExport_clicked();
    void onTimer();

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();

    void inProgress(qint64 value);
    void progressFinish();

private:
    void createProject();
    bool openProject(QString filePath = "");
    bool saveProject(QString filePath = "");
    bool saveAsProject();
    void clearProject();

    void loadProject(QJsonObject &json);
    QString stringifyProject();

private:
    Ui::MainWindow *ui;
    QPushButton *pushButtonTabCreate;

    QDir projectSaveDirectory;
    QString projectPath;

    bool stopSignal;
    TcpDriverInterface driverInterface;
};
#endif // MAINWINDOW_H

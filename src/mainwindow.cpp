#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

#include "src/chart/chart_view_form.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), stopSignal(true) {
    ui->setupUi(this);

    projectSaveDirectory = QDir(QDir::homePath() + "/Documents/KU Control Tuner Project");
    if (!projectSaveDirectory.exists()) {
        projectSaveDirectory.mkdir(projectSaveDirectory.path());
    }

    QWidget *pTabCornerWidget = new QWidget(this);

    pushButtonTabCreate = new QPushButton(pTabCornerWidget);
    pushButtonTabCreate->setText("+");
    pushButtonTabCreate->setMaximumSize(QSize(50, 50));
    connect(pushButtonTabCreate, &QPushButton::clicked, this, &MainWindow::onPushbuttonCreateClicked);

    QHBoxLayout *pHLayout = new QHBoxLayout(pTabCornerWidget);
    pHLayout->addWidget(pushButtonTabCreate);
    pHLayout->setMargin(0);

    ui->tabWidget->setCornerWidget(pTabCornerWidget, Qt::TopRightCorner);
    onPushbuttonCreateClicked();

    driverInterface.init();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_tabWidget_tabCloseRequested(int index) {
    if (stopSignal && index != -1 && ui->tabWidget->count() != 1) {
        ui->tabWidget->removeTab(index);
    }
}

void MainWindow::onPushbuttonCreateClicked() {
    QStringList names;
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        names << ui->tabWidget->tabText(i);
    }

    QString text;
    int count = 1;
    while (true) {
        QString name = "Tab " + QString::number(count++);
        if (names.indexOf(name) == -1) {
            text = name;
            break;
        }
    }

    ChartViewForm *form = new ChartViewForm(this);
    connect(ui->pushButtonStart, &QPushButton::clicked, form, &ChartViewForm::start);
    connect(ui->pushButtonStop, &QPushButton::clicked, form, &ChartViewForm::stop);
    connect(this, &MainWindow::chartUpdate, form, &ChartViewForm::update);
    ui->tabWidget->addTab(form, text);
    ui->tabWidget->setCurrentWidget(form);
}

void MainWindow::on_pushButtonStart_clicked() {
    pushButtonTabCreate->setEnabled(false);
    stopSignal = false;
    QTimer::singleShot(1, this, &MainWindow::onTimer);
}

void MainWindow::on_pushButtonStop_clicked() {
    pushButtonTabCreate->setEnabled(true);
    stopSignal = true;
}

void MainWindow::onTimer() {
    RobotData data = driverInterface.getRobotData();
    emit chartUpdate(data);

    if (!stopSignal) {
        QTimer::singleShot(1, this, &MainWindow::onTimer);
    }
}

void MainWindow::createProject() {
    if (!projectPath.isEmpty()) {
        QMessageBox::StandardButton reply;
        QString message = "Do you want to save the " + QFileInfo(projectPath).fileName();
        reply = QMessageBox::question(this, "new script", message, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Yes) {
            saveProject(projectPath);
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    clearProject();
    this->setWindowTitle("KU-Control-Tuner");
}

bool MainWindow::openProject(QString filePath) {
    bool result = false;

    if (filePath.isEmpty()) {
        filePath =  QFileDialog::getOpenFileName(this, "Open project", projectSaveDirectory.path(), "Json files (*.json)");
    }

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8");

            QString data = in.readAll();

            try {
                QJsonObject json = QJsonDocument::fromJson(data.toLocal8Bit()).object();
                loadProject(json);
                projectPath = filePath;
                this->setWindowTitle(QFileInfo(projectPath).fileName());
                result = true;
            } catch (std::runtime_error &e) {
                QMessageBox::warning(this, "", e.what());
            }

            file.close();
        }
    }
    return result;
}

bool MainWindow::saveProject(QString filePath) {
    bool result = false;
    if (filePath.isEmpty()) {
        if (projectPath.isEmpty()) {
            return saveAsProject();
        } else {
            filePath = projectPath;
        }
    }

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");

            try {
                out << stringifyProject();
                result = true;
            } catch (std::runtime_error &e) {
                QMessageBox::warning(this, "", e.what());
            }

            file.close();
        }
    }
    return result;
}

bool MainWindow::saveAsProject() {
    bool result = false;
    QString filePath =  QFileDialog::getSaveFileName(this, "Save project", projectSaveDirectory.path(), "Json files (*.json)");
    if (!filePath.isEmpty()) {
        if (saveProject(filePath)) {
            result = true;
            projectPath = filePath;
            this->setWindowTitle(QFileInfo(projectPath).fileName());
        }
    }
    return result;
}

void MainWindow::clearProject() {
    ui->tabWidget->clear();
}

void MainWindow::loadProject(QJsonObject &json) {
    QJsonArray tabs = json["tabs"].toArray();
    int count = tabs.count();
    if (count) {
        clearProject();
        for (int i = 0; i < count; i++) {
            QJsonObject tab = tabs[i].toObject();
            QString name = tab["name"].toString();
            QJsonArray values = tab["values"].toArray();

            QStringList datas;
            for (int j = 0; j < values.count(); j++) {
                datas << values[j].toString();
            }

            ChartViewForm *form = new ChartViewForm(this);
            form->init(datas);
            connect(ui->pushButtonStart, &QPushButton::clicked, form, &ChartViewForm::start);
            connect(ui->pushButtonStop, &QPushButton::clicked, form, &ChartViewForm::stop);
            connect(this, &MainWindow::chartUpdate, form, &ChartViewForm::update);
            ui->tabWidget->addTab(form, name);
        }

        ui->tabWidget->setCurrentIndex(0);
    }
}

QString MainWindow::stringifyProject() {
    QJsonObject json;
    QJsonArray tabs;
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        QString name = ui->tabWidget->tabText(i);
        ChartViewForm *form = static_cast<ChartViewForm *>(ui->tabWidget->widget(i));
        QStringList datas = form->getDatas();

        QJsonObject tab;
        QJsonArray values;

        tab["name"] = name;
        for (auto data : datas) {
            values.push_back(data);
        }
        tab["values"] = values;

        tabs.push_back(tab);
    }

    json["tabs"] = tabs;
    QJsonDocument doc(json);
    return doc.toJson(QJsonDocument::Indented);
}

void MainWindow::on_actionNew_triggered() {
    createProject();
}

void MainWindow::on_actionOpen_triggered() {
    openProject();
}

void MainWindow::on_actionSave_triggered() {
    saveProject();
}

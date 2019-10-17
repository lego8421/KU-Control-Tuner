#include "src/tuner/parameter_tuner_form.h"
#include "ui_parameter_tuner_form.h"

#include <QTableWidgetItem>
#include <QPushButton>
#include <QDebug>


ParameterTunerForm::ParameterTunerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterTunerForm) {
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    dialog = new ChartSelectDialog(this);
    connect(dialog, &ChartSelectDialog::itemDoubleClicked, this, &ParameterTunerForm::onChartSelected);

    init();
}

ParameterTunerForm::~ParameterTunerForm() {
    delete ui;
}

void ParameterTunerForm::update(RobotData data) {
    int count = ui->tableWidget->rowCount();

    if (!count) {
        return;
    }

    for (int i = 0; i < count; i++) {
        QString name = ui->tableWidget->item(i, 0)->text();
        double value = 0.0;

        if (name == "ControlCommand") {
            value = data.robotCommand;
        } else if(name == "ControlStatus") {
            value = data.robotStatus;
        } else if(name == "OperationState") {
            value = data.isPathOperating;
        } else if(name == "CollisionStatus") {
            value = data.collisionStatus;
        } else if(name.indexOf("JSMeasAngle") != -1) {
            QRegExp regex(REGEX_NUM);
            if (regex.indexIn(name) != -1) {
                int index = regex.cap(1).toInt();
                value = data.measuredJointAngle[index];
            }
        } else if(name.indexOf("JSMeasVel") != -1) {
            QRegExp regex(REGEX_NUM);
            if (regex.indexIn(name) != -1) {
                int index = regex.cap(1).toInt();
                value = data.measuredJointVelocity[index];
            }
        } else if(name.indexOf("JSDesAngle") != -1) {
            QRegExp regex(REGEX_NUM);
            if (regex.indexIn(name) != -1) {
                int index = regex.cap(1).toInt();
                value = data.desiredJointAngle[index];
            }
        } else if(name.indexOf("JSMeasTorqueJTS") != -1) {
            QRegExp regex(REGEX_NUM);
            if (regex.indexIn(name) != -1) {
                int index = regex.cap(1).toInt();
                value = data.measuredJointTorque[index];
            }
        } else if(name.indexOf("CSMeasPos") != -1) {
            QRegExp regex(REGEX_NUM);
            if (regex.indexIn(name) != -1) {
                int index = regex.cap(1).toInt();
                value = data.measuredCartesianPose[index];
            }
        } else if(name.indexOf("CSMeasVel") != -1) {
            QRegExp regex(REGEX_NUM);
            if (regex.indexIn(name) != -1) {
                int index = regex.cap(1).toInt();
                value = data.measuredCartesianVelocity[index];
            }
        } else if(name.indexOf("CSDesPos") != -1) {
            QRegExp regex(REGEX_NUM);
            if (regex.indexIn(name) != -1) {
                int index = regex.cap(1).toInt();
                value = data.desiredCartesianPose[index];
            }
        } else if(name.indexOf("MeasForceLPF2") != -1) {
            QRegExp regex(REGEX_NUM);
            if (regex.indexIn(name) != -1) {
                int index = regex.cap(1).toInt();
                value = data.measuredForce[index];
            }
        }

        ui->tableWidget->item(i, 1)->setText(QString::number(value, 'f', 3));
    }
}

void ParameterTunerForm::onPushButtonWriteClicked() {
    auto index = ui->tableWidget->indexAt(this->focusWidget()->pos());
    if (index.isValid()) {
        QTableWidgetItem *itemName = ui->tableWidget->item(index.row(), 0);
        if (itemName) {
            QString name = itemName->text();
            QTableWidgetItem *itemValue = ui->tableWidget->item(index.row(), 2);
            double value = itemValue->text().toDouble();

            QJsonObject json;
            json["command"] = 123123;
            json[name] = value;

            emit sendJson(json);
        }
    }
}

void ParameterTunerForm::onChartSelected(QString name) {
    addItem(name);
}

void ParameterTunerForm::on_pushButtonAdd_clicked() {
    dialog->show();
    dialog->raise();
}

void ParameterTunerForm::on_pushButtonDelete_clicked() {
    int row = ui->tableWidget->currentRow();
    deleteRow(row);
}

void ParameterTunerForm::init(QStringList names) {
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    dataNames.clear();
    for (auto name : names) {
        addItem(name);
    }
}

QStringList ParameterTunerForm::getDataNames() {
    return dataNames;
}

void ParameterTunerForm::addItem(QString name) {
    if (dataNames.indexOf(name) == -1) {
        dataNames << name;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(row + 1);

        QTableWidgetItem *item1 = new QTableWidgetItem(name);
        item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tableWidget->setItem(row, 0, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(0));
        item2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tableWidget->setItem(row, 1, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(0));
        item3->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 2, item3);

        QPushButton *button = new QPushButton("write");
        connect(button, &QPushButton::clicked, this, &ParameterTunerForm::onPushButtonWriteClicked);
        ui->tableWidget->setCellWidget(row, 3, button);
    }
}

void ParameterTunerForm::deleteRow(int row) {
    int column = ui->tableWidget->currentColumn();
    if (row != -1) {
        QTableWidgetItem *item = ui->tableWidget->item(row, 2);
        if (item) {
            QString text = item->text();
            dataNames.removeOne(text);
        }
        ui->tableWidget->removeRow(row);
        if (row == ui->tableWidget->rowCount()) {
            ui->tableWidget->setCurrentCell(row - 1, column);
        } else {
            ui->tableWidget->setCurrentCell(row, column);
        }
    }
}

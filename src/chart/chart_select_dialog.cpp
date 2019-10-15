#include "src/chart/chart_select_dialog.h"
#include "ui_chart_select_dialog.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>


ChartSelectDialog::ChartSelectDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::ChartSelectDialog) {
    ui->setupUi(this);

    QJsonObject parameters = loadParameters();
    writeParameters(parameters);
}

ChartSelectDialog::~ChartSelectDialog() {
    delete ui;
}

void ChartSelectDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    if (!item->childCount()) {
        QString name = item->text(column);
        emit itemDoubleClicked(name);
    }
}

QJsonObject ChartSelectDialog::loadParameters() {
    QJsonObject ret;
    QFile file(":/parameters/parameters.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");

        QString data = in.readAll();

        ret = QJsonDocument::fromJson(data.toLocal8Bit()).object();

        file.close();
    }
    return ret;
}

void ChartSelectDialog::writeParameters(QJsonObject &json) {
    QJsonArray parameters = json["parameters"].toArray();
    ui->treeWidget->clear();

    for (int i = 0; i < parameters.size(); i++) {
        QJsonObject json = parameters[i].toObject();
        QString name = json["name"].toString();
        QString type = json["type"].toString();
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

        item->setText(0, name);

        if (type == "array") {
            int count = json["array num"].toInt();
            for (int i = 0; i < count; i++) {
                QString childName = name + "[" + QString::number(i) + "]";
                QTreeWidgetItem *child = new QTreeWidgetItem(item);
                child->setText(0, childName);
                child->setExpanded(true);
                item->addChild(child);
            }
            item->setExpanded(true);
        }

        ui->treeWidget->addTopLevelItem(item);
    }
}

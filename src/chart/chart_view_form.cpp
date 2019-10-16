#include "src/chart/chart_view_form.h"
#include "ui_chart_view_form.h"

#include <QListWidgetItem>


ChartViewForm::ChartViewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartViewForm), initTime(0.0) {
    ui->setupUi(this);

    dialog = new ChartSelectDialog(this);

    connect(dialog, &ChartSelectDialog::itemDoubleClicked, [ = ](QString name) {
        if (dataNames.indexOf(name) == -1 && dataNames.size() < 7) {
            dataNames << name;
            ui->listWidget->addItem(name);
        }
    });

    init();
}

ChartViewForm::~ChartViewForm() {
    delete ui;
}

void ChartViewForm::start() {
    ui->pushButtonAdd->setEnabled(false);
    ui->pushButtonDelete->setEnabled(false);

    QTime time = QTime::currentTime();
    qreal seconds = 60 * 60 * time.hour() + 60 * time.minute() + time.second();
    initTime = 1000 * seconds + time.msec();

    QCustomPlot *plot = ui->widgetChart;
    int count = dataNames.count();

    plot->clearItems();
    plot->clearGraphs();

    plot->setInteraction(QCP::iRangeDrag, true);
    plot->setInteraction(QCP::iSelectPlottables, true);
    plot->setInteraction(QCP::iRangeZoom, true);

    if (!count) {
        return;
    }

    QList<QColor> color;
    if (count == 2) {
        color = rgColors;
    } else if (count == 3) {
        color = rgbColors;
    } else {
        color = rainbowColors;
    }
    for (int i = 0; i < count; i++) {
        QString name = dataNames[i];
        plot->addGraph();
        plot->graph()->setLineStyle(QCPGraph::lsLine);
        plot->graph()->setPen(QPen(color.at(i), 3.0));
        plot->graph()->setName(name);
    }

    plot->xAxis->setLabel("Time (s)");

    plot->legend->setVisible(true);
    plot->legend->setBrush(QColor(255, 255, 255, 150));
    plot->legend->setWrap(3);
    plot->legend->setRowSpacing(1);
    plot->legend->setColumnSpacing(2);
    plot->legend->setFillOrder(QCPLayoutGrid::FillOrder::foColumnsFirst, true);
    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);

    plot->replot();
}

void ChartViewForm::stop() {
    ui->pushButtonAdd->setEnabled(true);
    ui->pushButtonDelete->setEnabled(true);
}

void ChartViewForm::update(RobotData data) {
    QCustomPlot *plot = ui->widgetChart;
    int count = dataNames.count();

    if (!count) {
        return;
    }

    QTime time = QTime::currentTime();
    qreal seconds = 60 * 60 * time.hour() + 60 * time.minute() + time.second();
    qreal timeValue = (1000.0 * seconds + time.msec() - initTime) / 1000.0;

    for (int i = 0; i < count; i++) {
        QString name = dataNames[i];
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
        dataRange.setX(std::max(dataRange.x(), value));
        dataRange.setY(std::min(dataRange.y(), value));

        plot->graph(i)->addData(timeValue, value);
    }

    qreal offset = 0.1;
    qreal xMax = timeValue;
    qreal xMin = xMax - (100 * timeRange / 1000.0);
    qreal yMax = dataRange.x() * ((dataRange.x() >= 0) ? 1 + offset : 1 - offset);
    qreal yMin = dataRange.y() * ((dataRange.y() >= 0) ? 1 + offset : 1 - offset);

    plot->xAxis->setRange(xMin, xMax);
    plot->yAxis->setRange(yMin, yMax);

    plot->replot();
}

void ChartViewForm::on_pushButtonAdd_clicked() {
    dialog->show();
    dialog->raise();
}

void ChartViewForm::on_pushButtonDelete_clicked() {
    int row = ui->listWidget->currentRow();
    if (row != -1) {
        QString name = ui->listWidget->item(row)->text();
        dataNames.removeOne(name);
        ui->listWidget->takeItem(row);
        if (row == ui->listWidget->count()) {
            ui->listWidget->setCurrentRow(row - 1);
        } else {
            ui->listWidget->setCurrentRow(row);
        }
    }
}

void ChartViewForm::init(QStringList names) {
    ui->listWidget->clear();
    for (auto name : names) {
        ui->listWidget->addItem(name);
    }
    dataNames = names;
}

QStringList ChartViewForm::getDataNames() {
    return dataNames;
}

QStringList ChartViewForm::exportDatas() {
    QStringList ret;
    QCustomPlot *plot = ui->widgetChart;
    int graphCount = plot->graphCount();

    if (graphCount) {
        int dataCount = plot->graph()->data()->size();

        QString header;
        for (int i = 0; i < graphCount; i++) {
            header += "time," + plot->graph(i)->name() + ",";
        }
        ret << header;

        for (int i = 0; i < dataCount; i++) {
            QString data;
            for (int index = 0; index < graphCount; index++) {
                data += QString::number(plot->graph(index)->data()->at(i)->key, 'f', 3) + ",";
                data += QString::number(plot->graph(index)->data()->at(i)->value, 'f', 3) + ",";
            }
            ret << data;
        }
    }

    return ret;
}

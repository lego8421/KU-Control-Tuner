#ifndef PARAMETER_TUNER_FORM_H
#define PARAMETER_TUNER_FORM_H

#include <QWidget>

#include "src/define.h"
#include "src/chart/chart_select_dialog.h"


namespace Ui {
class ParameterTunerForm;
}

class ParameterTunerForm : public QWidget {
    Q_OBJECT

public:
    explicit ParameterTunerForm(QWidget *parent = nullptr);
    ~ParameterTunerForm();

signals:
    void sendJson(QJsonObject json);

public slots:
    void update(RobotData data);

private slots:
    void onPushButtonWriteClicked();
    void onChartSelected(QString name);
    void on_pushButtonAdd_clicked();
    void on_pushButtonDelete_clicked();

public:
    void init(QStringList names = {});
    QStringList getDataNames();

private:
    void addItem(QString name);
    void deleteRow(int row);

private:
    Ui::ParameterTunerForm *ui;
    ChartSelectDialog *dialog;

    QStringList dataNames;
};

#endif // PARAMETER_TUNER_FORM_H

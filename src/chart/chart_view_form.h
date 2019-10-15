#ifndef CHART_VIEW_FORM_H
#define CHART_VIEW_FORM_H

#include <QWidget>

#include "src/define.h"
#include "chart_select_dialog.h"


const QList<QColor> rainbowColors = {QColor("#ff0000"), QColor("#ff8c00"), QColor("#ffff00"), QColor("#008000"), QColor("#0000ff"), QColor("#4b0082"), QColor("#800080")};
const QList<QColor> rgbColors = {QColor("#ff0000"), QColor("#008000"), QColor("#0000ff")};
const QList<QColor> rgColors = {QColor("#ff0000"), QColor("#008000")};
const int timeRange = 100;
const QString REGEX_NUM("\\[([\\d])\\]");

namespace Ui {
class ChartViewForm;
}

class ChartViewForm : public QWidget {
    Q_OBJECT

public:
    explicit ChartViewForm(QWidget *parent = nullptr);
    ~ChartViewForm();

public slots:
    void start();
    void stop();
    void update(RobotData data);

private slots:
    void on_pushButtonAdd_clicked();
    void on_pushButtonDelete_clicked();

public:
    void init(QStringList names = {});
    QStringList getDatas();

private:
    Ui::ChartViewForm *ui;
    ChartSelectDialog *dialog;

    double initTime;
    QPointF dataRange;
    QStringList datas;
};

#endif // CHART_VIEW_FORM_H
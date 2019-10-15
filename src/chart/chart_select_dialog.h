#ifndef CHART_SELECT_DIALOG_H
#define CHART_SELECT_DIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QJsonObject>


namespace Ui {
class ChartSelectDialog;
}

class ChartSelectDialog : public QDialog {
    Q_OBJECT

public:
    explicit ChartSelectDialog(QWidget *parent = nullptr);
    ~ChartSelectDialog();

signals:
    void itemDoubleClicked(QString text);

private slots:
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    QJsonObject loadParameters();
    void writeParameters(QJsonObject &json);

private:
    Ui::ChartSelectDialog *ui;
};

#endif // CHART_SELECT_DIALOG_H

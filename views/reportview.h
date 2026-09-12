#pragma once
#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QStandardItemModel>

class ReportView : public QWidget {
    Q_OBJECT
public:
    explicit ReportView(QWidget* p = nullptr);
private slots:
    void onGenerate();
    void onExportCsv();
private:
    void setupUi();
    void fillBonus();
    QTableView* table_;
    QStandardItemModel* model_;
    QPushButton *genBtn_, *expBtn_;
    QLabel* status_;
};

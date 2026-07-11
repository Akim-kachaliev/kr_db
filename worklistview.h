#pragma once
#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QStandardItemModel>

class WorkListView : public QWidget {
    Q_OBJECT
public:
    explicit WorkListView(QWidget* p = nullptr);
private slots:
    void onCreate();
    void onAssign();
    void onComplete();
    void refresh();
private:
    void setupUi();
    QTableView* table_;
    QStandardItemModel* model_;
    QPushButton *createBtn_, *assignBtn_, *completeBtn_, *refreshBtn_;
    QLabel* status_;
};

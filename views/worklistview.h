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
    void onDelete();
    void onEdit();
    void refresh();
private:
    void setupUi();
    void refreshAssignments(int workId);
    QTableView* table_;
    QTableView* assignTable_;
    QStandardItemModel* model_;
    QStandardItemModel* assignModel_;
    QPushButton *createBtn_, *assignBtn_, *completeBtn_, *refreshBtn_, *deleteBtn_, *editBtn_;
    QLabel* status_;
};
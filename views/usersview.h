#pragma once
#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QStandardItemModel>

class UsersView : public QWidget {
    Q_OBJECT
public:
    explicit UsersView(QWidget* p = nullptr);
private slots:
    void onCreate();
    void onEdit();
    void onDelete();
    void refresh();
private:
    void setupUi();
    QTableView* table_;
    QStandardItemModel* model_;
    QPushButton *createBtn_, *editBtn_, *deleteBtn_, *refreshBtn_;
    QLabel* status_;
};
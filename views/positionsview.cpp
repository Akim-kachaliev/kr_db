#include "positionsview.h"
#include "../models/databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSqlQuery>
#include <QSqlError>

// Диалог создания/редактирования должности

class PositionDialog : public QDialog {
public:
    PositionDialog(QWidget* p = nullptr) : QDialog(p) {
        setWindowTitle("Должность");
        setMinimumWidth(400);
        auto* f = new QFormLayout(this);
        
        nameEdit_ = new QLineEdit(this);
        salarySpin_ = new QDoubleSpinBox(this);
        salarySpin_->setRange(1.0, 10000000.0);
        salarySpin_->setDecimals(2);
        salarySpin_->setSuffix(" руб.");
        salarySpin_->setValue(50000.0);
        
        f->addRow("Наименование:", nameEdit_);
        f->addRow("Базовый оклад:", salarySpin_);
        
        auto* b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        f->addRow(b);
        connect(b, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(b, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }
    
    void setData(const QString& name, double salary) {
        nameEdit_->setText(name);
        salarySpin_->setValue(salary);
    }
    
    QString name() const { return nameEdit_->text().trimmed(); }
    double salary() const { return salarySpin_->value(); }
    
private:
    QLineEdit* nameEdit_;
    QDoubleSpinBox* salarySpin_;
};


PositionsView::PositionsView(QWidget* p) : QWidget(p) {
    setupUi();
    refresh();
}

void PositionsView::setupUi() {
    auto* m = new QVBoxLayout(this);
    
    table_ = new QTableView(this);
    model_ = new QStandardItemModel(this);
    model_->setHorizontalHeaderLabels({"ID", "Наименование", "Базовый оклад"});
    table_->setModel(model_);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    m->addWidget(table_);
    
    auto* r = new QHBoxLayout();
    createBtn_ = new QPushButton("Создать", this);
    editBtn_ = new QPushButton("Редактировать", this);
    deleteBtn_ = new QPushButton("Удалить", this);
    refreshBtn_ = new QPushButton("Обновить", this);
    status_ = new QLabel(this);
    
    r->addWidget(createBtn_);
    r->addWidget(editBtn_);
    r->addWidget(deleteBtn_);
    r->addWidget(refreshBtn_);
    r->addStretch();
    r->addWidget(status_);
    m->addLayout(r);
    
    connect(createBtn_, &QPushButton::clicked, this, &PositionsView::onCreate);
    connect(editBtn_, &QPushButton::clicked, this, &PositionsView::onEdit);
    connect(deleteBtn_, &QPushButton::clicked, this, &PositionsView::onDelete);
    connect(refreshBtn_, &QPushButton::clicked, this, &PositionsView::refresh);
}

void PositionsView::refresh() {
    model_->removeRows(0, model_->rowCount());
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Номер_должности, Наименование, Базовый_оклад "
                "FROM Должность ORDER BY Наименование")) {
        QMessageBox::critical(this, "Ошибка", q.lastError().text());
        return;
    }
    while (q.next()) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(q.value(0).toInt()))
            << new QStandardItem(q.value(1).toString())
            << new QStandardItem(QString::number(q.value(2).toDouble(), 'f', 2));
        model_->appendRow(row);
    }
    status_->setText(QString("Записей: %1").arg(model_->rowCount()));
}

void PositionsView::onCreate() {
    PositionDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;
    if (dlg.name().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите наименование");
        return;
    }
    
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("INSERT INTO Должность (Наименование, Базовый_оклад) VALUES (?, ?)");
    q.addBindValue(dlg.name());
    q.addBindValue(dlg.salary());
    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка", q.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Должность создана");
    refresh();
}

void PositionsView::onEdit() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Внимание", "Выберите должность");
        return;
    }
    int id = model_->item(idx.row(), 0)->text().toInt();
    
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Наименование, Базовый_оклад FROM Должность WHERE Номер_должности=?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return;
    
    PositionDialog dlg(this);
    dlg.setData(q.value(0).toString(), q.value(1).toDouble());
    if (dlg.exec() != QDialog::Accepted) return;
    
    QSqlQuery upd(DatabaseManager::getInstance()->db());
    upd.prepare("UPDATE Должность SET Наименование=?, Базовый_оклад=? "
                "WHERE Номер_должности=?");
    upd.addBindValue(dlg.name());
    upd.addBindValue(dlg.salary());
    upd.addBindValue(id);
    if (!upd.exec()) {
        QMessageBox::critical(this, "Ошибка", upd.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Должность обновлена");
    refresh();
}

void PositionsView::onDelete() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Внимание", "Выберите должность");
        return;
    }
    int id = model_->item(idx.row(), 0)->text().toInt();
    QString name = model_->item(idx.row(), 1)->text();
    
    if (QMessageBox::question(this, "Подтверждение",
        QString("Удалить должность «%1»?").arg(name)) != QMessageBox::Yes) return;
    
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("DELETE FROM Должность WHERE Номер_должности=?");
    q.addBindValue(id);
    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка",
            "Не удалось удалить: " + q.lastError().text() +
            "\n\nВозможно, должность связана с сотрудниками.");
        return;
    }
    QMessageBox::information(this, "Успех", "Должность удалена");
    refresh();
}
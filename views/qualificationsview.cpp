#include "qualificationsview.h"
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

class QualificationDialog : public QDialog {
public:
    QualificationDialog(QWidget* p = nullptr) : QDialog(p) {
        setWindowTitle("Квалификация");
        setMinimumWidth(400);
        auto* f = new QFormLayout(this);
        
        nameEdit_ = new QLineEdit(this);
        coeffSpin_ = new QDoubleSpinBox(this);
        coeffSpin_->setRange(0.1, 5.0);
        coeffSpin_->setDecimals(2);
        coeffSpin_->setSingleStep(0.1);
        coeffSpin_->setValue(1.0);
        
        f->addRow("Наименование:", nameEdit_);
        f->addRow("Коэффициент:", coeffSpin_);
        
        auto* b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        f->addRow(b);
        connect(b, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(b, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }
    
    void setData(const QString& name, double coeff) {
        nameEdit_->setText(name);
        coeffSpin_->setValue(coeff);
    }
    
    QString name() const { return nameEdit_->text().trimmed(); }
    double coeff() const { return coeffSpin_->value(); }
    
private:
    QLineEdit* nameEdit_;
    QDoubleSpinBox* coeffSpin_;
};

QualificationsView::QualificationsView(QWidget* p) : QWidget(p) {
    setupUi();
    refresh();
}

void QualificationsView::setupUi() {
    auto* m = new QVBoxLayout(this);
    table_ = new QTableView(this);
    model_ = new QStandardItemModel(this);
    model_->setHorizontalHeaderLabels({"ID", "Наименование", "Коэффициент"});
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
    r->addWidget(createBtn_); r->addWidget(editBtn_);
    r->addWidget(deleteBtn_); r->addWidget(refreshBtn_);
    r->addStretch(); r->addWidget(status_);
    m->addLayout(r);
    
    connect(createBtn_, &QPushButton::clicked, this, &QualificationsView::onCreate);
    connect(editBtn_, &QPushButton::clicked, this, &QualificationsView::onEdit);
    connect(deleteBtn_, &QPushButton::clicked, this, &QualificationsView::onDelete);
    connect(refreshBtn_, &QPushButton::clicked, this, &QualificationsView::refresh);
}

void QualificationsView::refresh() {
    model_->removeRows(0, model_->rowCount());
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Номер_квалификации, Наименование, Коэффициент "
                "FROM Квалификация ORDER BY Наименование")) {
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

void QualificationsView::onCreate() {
    QualificationDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;
    if (dlg.name().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите наименование");
        return;
    }
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("INSERT INTO Квалификация (Наименование, Коэффициент) VALUES (?, ?)");
    q.addBindValue(dlg.name());
    q.addBindValue(dlg.coeff());
    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка", q.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Квалификация создана");
    refresh();
}

void QualificationsView::onEdit() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) return;
    int id = model_->item(idx.row(), 0)->text().toInt();
    
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Наименование, Коэффициент FROM Квалификация WHERE Номер_квалификации=?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return;
    
    QualificationDialog dlg(this);
    dlg.setData(q.value(0).toString(), q.value(1).toDouble());
    if (dlg.exec() != QDialog::Accepted) return;
    
    QSqlQuery upd(DatabaseManager::getInstance()->db());
    upd.prepare("UPDATE Квалификация SET Наименование=?, Коэффициент=? "
                "WHERE Номер_квалификации=?");
    upd.addBindValue(dlg.name());
    upd.addBindValue(dlg.coeff());
    upd.addBindValue(id);
    if (!upd.exec()) {
        QMessageBox::critical(this, "Ошибка", upd.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Квалификация обновлена");
    refresh();
}

void QualificationsView::onDelete() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) return;
    int id = model_->item(idx.row(), 0)->text().toInt();
    QString name = model_->item(idx.row(), 1)->text();
    
    if (QMessageBox::question(this, "Подтверждение",
        QString("Удалить квалификацию «%1»?").arg(name)) != QMessageBox::Yes) return;
    
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("DELETE FROM Квалификация WHERE Номер_квалификации=?");
    q.addBindValue(id);
    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка",
            "Не удалось удалить: " + q.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Квалификация удалена");
    refresh();
}
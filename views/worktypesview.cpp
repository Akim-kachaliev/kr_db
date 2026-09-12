#include "worktypesview.h"
#include "../models/databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDoubleSpinBox>
#include <QSqlQuery>
#include <QSqlError>

class WorkTypeDialog : public QDialog {
public:
    WorkTypeDialog(QWidget* p = nullptr) : QDialog(p) {
        setWindowTitle("Вид работы");
        setMinimumWidth(450);
        auto* f = new QFormLayout(this);
        
        nameEdit_ = new QLineEdit(this);
        descrEdit_ = new QPlainTextEdit(this);
        descrEdit_->setFixedHeight(60);
        paySpin_ = new QDoubleSpinBox(this);
        paySpin_->setRange(1.0, 10000000.0);
        paySpin_->setDecimals(2);
        paySpin_->setSuffix(" руб.");
        paySpin_->setValue(5000.0);
        hoursSpin_ = new QDoubleSpinBox(this);
        hoursSpin_->setRange(0.5, 1000.0);
        hoursSpin_->setDecimals(2);
        hoursSpin_->setSuffix(" ч.");
        hoursSpin_->setValue(8.0);
        
        f->addRow("Наименование:", nameEdit_);
        f->addRow("Описание:", descrEdit_);
        f->addRow("Базовая оплата:", paySpin_);
        f->addRow("Нормативная длительность:", hoursSpin_);
        
        auto* b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        f->addRow(b);
        connect(b, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(b, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }
    
    void setData(const QString& name, const QString& descr,
                 double pay, double hours) {
        nameEdit_->setText(name);
        descrEdit_->setPlainText(descr);
        paySpin_->setValue(pay);
        hoursSpin_->setValue(hours);
    }
    
    QString name() const { return nameEdit_->text().trimmed(); }
    QString descr() const { return descrEdit_->toPlainText().trimmed(); }
    double pay() const { return paySpin_->value(); }
    double hours() const { return hoursSpin_->value(); }
    
private:
    QLineEdit* nameEdit_;
    QPlainTextEdit* descrEdit_;
    QDoubleSpinBox *paySpin_, *hoursSpin_;
};

WorkTypesView::WorkTypesView(QWidget* p) : QWidget(p) {
    setupUi();
    refresh();
}

void WorkTypesView::setupUi() {
    auto* m = new QVBoxLayout(this);
    table_ = new QTableView(this);
    model_ = new QStandardItemModel(this);
    model_->setHorizontalHeaderLabels(
        {"ID", "Наименование", "Описание", "Базовая оплата", "Норматив (ч)"});
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
    
    connect(createBtn_, &QPushButton::clicked, this, &WorkTypesView::onCreate);
    connect(editBtn_, &QPushButton::clicked, this, &WorkTypesView::onEdit);
    connect(deleteBtn_, &QPushButton::clicked, this, &WorkTypesView::onDelete);
    connect(refreshBtn_, &QPushButton::clicked, this, &WorkTypesView::refresh);
}

void WorkTypesView::refresh() {
    model_->removeRows(0, model_->rowCount());
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Код_вида_работы, Наименование, Описание, "
                "Базовая_оплата, Нормативная_длительность "
                "FROM Вид_работы ORDER BY Наименование")) {
        QMessageBox::critical(this, "Ошибка", q.lastError().text());
        return;
    }
    while (q.next()) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(q.value(0).toInt()))
            << new QStandardItem(q.value(1).toString())
            << new QStandardItem(q.value(2).toString())
            << new QStandardItem(QString::number(q.value(3).toDouble(), 'f', 2))
            << new QStandardItem(QString::number(q.value(4).toDouble(), 'f', 2));
        model_->appendRow(row);
    }
    status_->setText(QString("Записей: %1").arg(model_->rowCount()));
}

void WorkTypesView::onCreate() {
    WorkTypeDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;
    if (dlg.name().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите наименование");
        return;
    }
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("INSERT INTO Вид_работы (Наименование, Описание, Базовая_оплата, "
              "Нормативная_длительность) VALUES (?, ?, ?, ?)");
    q.addBindValue(dlg.name());
    q.addBindValue(dlg.descr());
    q.addBindValue(dlg.pay());
    q.addBindValue(dlg.hours());
    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка", q.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Вид работы создан");
    refresh();
}

void WorkTypesView::onEdit() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) return;
    int id = model_->item(idx.row(), 0)->text().toInt();
    
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Наименование, Описание, Базовая_оплата, Нормативная_длительность "
              "FROM Вид_работы WHERE Код_вида_работы=?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return;
    
    WorkTypeDialog dlg(this);
    dlg.setData(q.value(0).toString(), q.value(1).toString(),
                q.value(2).toDouble(), q.value(3).toDouble());
    if (dlg.exec() != QDialog::Accepted) return;
    
    QSqlQuery upd(DatabaseManager::getInstance()->db());
    upd.prepare("UPDATE Вид_работы SET Наименование=?, Описание=?, "
                "Базовая_оплата=?, Нормативная_длительность=? "
                "WHERE Код_вида_работы=?");
    upd.addBindValue(dlg.name());
    upd.addBindValue(dlg.descr());
    upd.addBindValue(dlg.pay());
    upd.addBindValue(dlg.hours());
    upd.addBindValue(id);
    if (!upd.exec()) {
        QMessageBox::critical(this, "Ошибка", upd.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Вид работы обновлён");
    refresh();
}

void WorkTypesView::onDelete() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) return;
    int id = model_->item(idx.row(), 0)->text().toInt();
    QString name = model_->item(idx.row(), 1)->text();
    
    if (QMessageBox::question(this, "Подтверждение",
        QString("Удалить вид работы «%1»?").arg(name)) != QMessageBox::Yes) return;
    
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("DELETE FROM Вид_работы WHERE Код_вида_работы=?");
    q.addBindValue(id);
    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка",
            "Не удалось удалить: " + q.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Вид работы удалён");
    refresh();
}
#include "worklistview.h"
#include "workdialog.h"
#include "../controllers/authcontroller.h"
#include "../controllers/workcontroller.h"
#include "../models/additionalwork.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QDate>

WorkListView::WorkListView(QWidget* p) : QWidget(p) {
    setupUi();
    refresh();
}

void WorkListView::setupUi() {
    auto* m = new QVBoxLayout(this);
    table_ = new QTableView(this);
    model_ = new QStandardItemModel(this);
    model_->setHorizontalHeaderLabels(
        {"ID", "Наименование", "Описание", "Создана",
         "Начало", "Окончание", "Исполнителей"});
    table_->setModel(model_);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    m->addWidget(table_);
    auto* r = new QHBoxLayout();
    createBtn_ = new QPushButton("Создать работу", this);
    assignBtn_ = new QPushButton("Назначить сотрудника", this);
    completeBtn_ = new QPushButton("Завершить", this);
    refreshBtn_ = new QPushButton("Обновить", this);
    status_ = new QLabel(this);
    r->addWidget(createBtn_);
    r->addWidget(assignBtn_);
    r->addWidget(completeBtn_);
    r->addWidget(refreshBtn_);
    r->addStretch();
    r->addWidget(status_);
    m->addLayout(r);
    auto* u = AuthController::getInstance()->currentUser();
    bool canEdit = u && (u->hasRole("Администратор") || u->hasRole("Руководитель"));
    createBtn_->setEnabled(canEdit);
    assignBtn_->setEnabled(canEdit);
    connect(createBtn_, &QPushButton::clicked, this, &WorkListView::onCreate);
    connect(assignBtn_, &QPushButton::clicked, this, &WorkListView::onAssign);
    connect(completeBtn_, &QPushButton::clicked, this, &WorkListView::onComplete);
    connect(refreshBtn_, &QPushButton::clicked, this, &WorkListView::refresh);
}

void WorkListView::refresh() {
    model_->removeRows(0, model_->rowCount());
    auto works = WorkController::getInstance()->getWorks();
    for (AdditionalWork* w : works) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(w->id()))
            << new QStandardItem(w->title())
            << new QStandardItem(w->descr())
            << new QStandardItem(w->created().toString("dd.MM.yyyy"))
            << new QStandardItem(w->planStart().toString("dd.MM.yyyy"))
            << new QStandardItem(w->planEnd().toString("dd.MM.yyyy"))
            << new QStandardItem(QString::number(w->planWorkers()));
        model_->appendRow(row);
        delete w;
    }
    status_->setText(QString("Записей: %1").arg(model_->rowCount()));
}

void WorkListView::onCreate() {
    WorkDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;
    if (dlg.title().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите заголовок");
        return;
    }
    int id = WorkController::getInstance()->createWork(
        dlg.workTypeId(), dlg.title(), dlg.descr(),
        dlg.planStart(), dlg.planEnd(), dlg.workers());
    if (id == 0) {
        QMessageBox::critical(this, "Ошибка",
            WorkController::getInstance()->lastError());
        return;
    }
    QMessageBox::information(this, "Успех",
        QString("Работа создана. ID = %1").arg(id));
    refresh();
}

void WorkListView::onAssign() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Внимание", "Выберите работу");
        return;
    }
    int workId = model_->item(idx.row(), 0)->text().toInt();
    bool ok = false;
    int empId = QInputDialog::getInt(this, "Назначение",
        "ID сотрудника:", 1, 1, 999999, 1, &ok);
    if (!ok) return;
    if (WorkController::getInstance()->assignEmployee(workId, empId)) {
        QMessageBox::information(this, "Успех", "Сотрудник назначен");
    } else {
        QMessageBox::critical(this, "Ошибка",
            WorkController::getInstance()->lastError());
    }
}

void WorkListView::onComplete() {
    bool ok = false;
    int assignId = QInputDialog::getInt(this, "Завершение",
        "ID назначения:", 1, 1, 999999, 1, &ok);
    if (!ok) return;
    if (WorkController::getInstance()->completeAssignment(
            assignId, QDate::currentDate())) {
        QMessageBox::information(this, "Успех",
            "Назначение завершено. Сумма доплаты рассчитана.");
    } else {
        QMessageBox::critical(this, "Ошибка",
            WorkController::getInstance()->lastError());
    }
}

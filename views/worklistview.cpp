#include "worklistview.h"
#include "workdialog.h"
#include "../controllers/authcontroller.h"
#include "../controllers/workcontroller.h"
#include "../models/additionalwork.h"
#include "../models/assignment.h"
#include "../models/employee.h"
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

    // Верхняя таблица: работы
    table_ = new QTableView(this);
    model_ = new QStandardItemModel(this);
    model_->setHorizontalHeaderLabels(
        {"ID", "Наименование", "Описание", "Создана",
         "Начало", "Окончание", "Исполнителей"});
    table_->setModel(model_);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    m->addWidget(table_);

    // Нижняя таблица: назначения
    assignModel_ = new QStandardItemModel(this);
    assignModel_->setHorizontalHeaderLabels({"Назначение", "Сотрудник", "Статус", "Сумма"});
    assignTable_ = new QTableView(this);
    assignTable_->setModel(assignModel_);
    assignTable_->horizontalHeader()->setStretchLastSection(true);
    assignTable_->setMaximumHeight(150);
    m->addWidget(new QLabel("Назначенные сотрудники:", this));
    m->addWidget(assignTable_);

    // Кнопки
    auto* r = new QHBoxLayout();
    createBtn_ = new QPushButton("Создать работу", this);
    assignBtn_ = new QPushButton("Назначить сотрудника", this);
    completeBtn_ = new QPushButton("Завершить", this);
    deleteBtn_ = new QPushButton("Удалить", this);
    editBtn_ = new QPushButton("Редактировать", this);
    refreshBtn_ = new QPushButton("Обновить", this);
    status_ = new QLabel(this);

    r->addWidget(createBtn_);
    r->addWidget(assignBtn_);
    r->addWidget(completeBtn_);
    r->addWidget(editBtn_);
    r->addWidget(deleteBtn_);
    r->addWidget(refreshBtn_);
    r->addStretch();
    r->addWidget(status_);
    m->addLayout(r);

    auto* u = AuthController::getInstance()->currentUser();
    bool canEdit = u && (u->hasRole("Администратор") || u->hasRole("Руководитель"));
    createBtn_->setEnabled(canEdit);
    assignBtn_->setEnabled(canEdit);
    completeBtn_->setEnabled(canEdit);
    deleteBtn_->setEnabled(canEdit);
    editBtn_->setEnabled(canEdit);

    connect(createBtn_, &QPushButton::clicked, this, &WorkListView::onCreate);
    connect(assignBtn_, &QPushButton::clicked, this, &WorkListView::onAssign);
    connect(completeBtn_, &QPushButton::clicked, this, &WorkListView::onComplete);
    connect(deleteBtn_, &QPushButton::clicked, this, &WorkListView::onDelete);
    connect(editBtn_, &QPushButton::clicked, this, &WorkListView::onEdit);
    connect(refreshBtn_, &QPushButton::clicked, this, &WorkListView::refresh);

    // Связь: выбор работы → показ назначений
    connect(table_->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, [this](const QModelIndex& current, const QModelIndex&) {
                if (current.isValid()) {
                    int workId = model_->item(current.row(), 0)->text().toInt();
                    refreshAssignments(workId);
                } else {
                    assignModel_->removeRows(0, assignModel_->rowCount());
                }
            });
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
    assignModel_->removeRows(0, assignModel_->rowCount());
}

void WorkListView::refreshAssignments(int workId) {
    assignModel_->removeRows(0, assignModel_->rowCount());
    auto assignments = Assignment::findByWork(workId);
    for (Assignment* a : assignments) {
        Employee* e = Employee::findById(a->employeeId());
        QString empName = e ? e->fullName() : QString("ID %1").arg(a->employeeId());
        delete e;

        QString status = a->completed().isValid() ? "Завершено" : "В работе";
        QString sum = a->payment() > 0 ? QString::number(a->payment(), 'f', 2) : "—";

        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(a->id()))
            << new QStandardItem(empName)
            << new QStandardItem(status)
            << new QStandardItem(sum);
        assignModel_->appendRow(row);
        delete a;
    }
}

void WorkListView::onCreate() {
    WorkDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

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
    QString workTitle = model_->item(idx.row(), 1)->text();

    auto employees = Employee::findAll();
    if (employees.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет сотрудников в базе данных");
        return;
    }

    QStringList items;
    QList<int> ids;
    for (Employee* e : employees) {
        items << QString("%1 (ID %2)").arg(e->fullName()).arg(e->id());
        ids << e->id();
        delete e;
    }

    bool ok = false;
    QString chosen = QInputDialog::getItem(this, "Назначение сотрудника",
                                           QString("Выберите сотрудника для работы «%1»:").arg(workTitle),
                                           items, 0, false, &ok);
    if (!ok || chosen.isEmpty()) return;

    int index = items.indexOf(chosen);
    if (index < 0) return;
    int empId = ids[index];

    if (WorkController::getInstance()->assignEmployee(workId, empId)) {
        QMessageBox::information(this, "Успех", "Сотрудник назначен");
        refreshAssignments(workId);
    } else {
        QMessageBox::critical(this, "Ошибка",
                              WorkController::getInstance()->lastError());
    }
}

void WorkListView::onComplete() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Внимание", "Выберите работу");
        return;
    }
    int workId = model_->item(idx.row(), 0)->text().toInt();

    auto assignments = Assignment::findByWork(workId);
    QStringList items;
    QList<int> ids;
    for (Assignment* a : assignments) {
        if (a->completed().isValid()) { delete a; continue; }
        Employee* e = Employee::findById(a->employeeId());
        QString empName = e ? e->fullName() : QString("ID %1").arg(a->employeeId());
        delete e;
        items << QString("Назначение %1: %2").arg(a->id()).arg(empName);
        ids << a->id();
        delete a;
    }

    if (items.isEmpty()) {
        QMessageBox::information(this, "Информация",
                                 "Нет активных назначений по этой работе");
        return;
    }

    bool ok = false;
    QString chosen = QInputDialog::getItem(this, "Завершение назначения",
                                           "Выберите назначение для завершения:", items, 0, false, &ok);
    if (!ok || chosen.isEmpty()) return;

    int index = items.indexOf(chosen);
    if (index < 0) return;
    int assignId = ids[index];

    if (WorkController::getInstance()->completeAssignment(
            assignId, QDate::currentDate())) {
        QMessageBox::information(this, "Успех",
                                 "Назначение завершено. Сумма доплаты рассчитана.");
        refreshAssignments(workId);
    } else {
        QMessageBox::critical(this, "Ошибка",
                              WorkController::getInstance()->lastError());
    }
}

void WorkListView::onDelete() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Внимание", "Выберите работу для удаления");
        return;
    }

    int workId = model_->item(idx.row(), 0)->text().toInt();
    QString workTitle = model_->item(idx.row(), 1)->text();

    if (QMessageBox::question(this, "Подтверждение",
                              QString("Удалить работу «%1» (ID = %2)?\n\n"
                                      "Все назначения по этой работе также будут удалены.")
                                  .arg(workTitle).arg(workId)) != QMessageBox::Yes) {
        return;
    }

    if (WorkController::getInstance()->deleteWork(workId)) {
        QMessageBox::information(this, "Успех", "Работа удалена");
        refresh();
    } else {
        QMessageBox::critical(this, "Ошибка",
                              WorkController::getInstance()->lastError());
    }
}

void WorkListView::onEdit() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Внимание", "Выберите работу для редактирования");
        return;
    }
    int workId = model_->item(idx.row(), 0)->text().toInt();

    AdditionalWork* w = AdditionalWork::findById(workId);
    if (!w) {
        QMessageBox::critical(this, "Ошибка", "Работа не найдена");
        return;
    }

    WorkDialog dlg(w, this);
    delete w;

    if (dlg.exec() != QDialog::Accepted) return;

    if (WorkController::getInstance()->updateWork(
            workId, dlg.workTypeId(), dlg.title(), dlg.descr(),
            dlg.planStart(), dlg.planEnd(), dlg.workers())) {
        QMessageBox::information(this, "Успех", "Работа обновлена");
        refresh();
    } else {
        QMessageBox::critical(this, "Ошибка",
                              WorkController::getInstance()->lastError());
    }
}
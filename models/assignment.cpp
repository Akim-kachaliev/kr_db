#include "assignment.h"
#include <QSqlError>
#include "databasemanager.h"
#include <QVariant>

bool Assignment::save() {
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (id_ == 0) {
        q.prepare("INSERT INTO Назначение "
                  "(Номер_сотрудника, Номер_доп_работы, Начало_работ, Дата_завершения) "
                  "VALUES (?, ?, ?, ?) RETURNING Номер_назначения");
        q.addBindValue(empId_);
        q.addBindValue(workId_);
        q.addBindValue(started_.isValid() ? QVariant(started_) : QVariant());
        q.addBindValue(completed_.isValid() ? QVariant(completed_) : QVariant());
    } else {
        q.prepare("UPDATE Назначение SET Начало_работ=?, Дата_завершения=? "
                  "WHERE Номер_назначения=?");
        q.addBindValue(started_.isValid() ? QVariant(started_) : QVariant());
        q.addBindValue(completed_.isValid() ? QVariant(completed_) : QVariant());
        q.addBindValue(id_);
    }
    if (!q.exec()) {
        DatabaseManager::getInstance()->setLastError(q.lastError().text());
        return false;
    }
    if (id_ == 0 && q.next()) id_ = q.value(0).toInt();
    if (completed_.isValid() && id_ > 0) {
        QSqlQuery q2(DatabaseManager::getInstance()->db());
        q2.prepare("SELECT Сумма_доплаты FROM Назначение WHERE Номер_назначения=?");
        q2.addBindValue(id_);
        if (q2.exec() && q2.next()) payment_ = q2.value(0).toDouble();
    }
    return true;
}

bool Assignment::complete(const QDate& completed) {
    completed_ = completed;
    return save();
}

QList<Assignment*> Assignment::findByEmployee(int empId) {
    QList<Assignment*> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT n.Номер_назначения, n.Номер_сотрудника, n.Номер_доп_работы, "
              "n.Дата_назначения, n.Начало_работ, n.Дата_завершения, n.Сумма_доплаты "
              "FROM Назначение n WHERE n.Номер_сотрудника = ? "
              "ORDER BY n.Дата_назначения DESC");
    q.addBindValue(empId);
    if (!q.exec()) return result;
    while (q.next()) {
        auto* a = new Assignment();
        a->id_ = q.value(0).toInt();
        a->empId_ = q.value(1).toInt();
        a->workId_ = q.value(2).toInt();
        a->assigned_ = q.value(3).toDate();
        a->started_ = q.value(4).toDate();
        a->completed_ = q.value(5).toDate();
        a->payment_ = q.value(6).toDouble();
        result.append(a);
    }
    return result;
}

QList<Assignment*> Assignment::findByWork(int workId) {
    QList<Assignment*> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Номер_назначения, Номер_сотрудника, Номер_доп_работы, "
              "Дата_назначения, Начало_работ, Дата_завершения, Сумма_доплаты "
              "FROM Назначение WHERE Номер_доп_работы = ? ORDER BY Дата_назначения");
    q.addBindValue(workId);
    if (!q.exec()) return result;
    while (q.next()) {
        auto* a = new Assignment();
        a->id_ = q.value(0).toInt();
        a->empId_ = q.value(1).toInt();
        a->workId_ = q.value(2).toInt();
        a->assigned_ = q.value(3).toDate();
        a->started_ = q.value(4).toDate();
        a->completed_ = q.value(5).toDate();
        a->payment_ = q.value(6).toDouble();
        result.append(a);
    }
    return result;
}

Assignment* Assignment::findById(int id) {
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Номер_назначения, Номер_сотрудника, Номер_доп_работы, "
              "Дата_назначения, Начало_работ, Дата_завершения, Сумма_доплаты "
              "FROM Назначение WHERE Номер_назначения = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return nullptr;
    auto* a = new Assignment();
    a->id_ = q.value(0).toInt();
    a->empId_ = q.value(1).toInt();
    a->workId_ = q.value(2).toInt();
    a->assigned_ = q.value(3).toDate();
    a->started_ = q.value(4).toDate();
    a->completed_ = q.value(5).toDate();
    a->payment_ = q.value(6).toDouble();
    return a;
}

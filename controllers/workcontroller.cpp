#include "workcontroller.h"
#include "../models/databasemanager.h"
#include "../models/assignment.h"
#include <QSqlQuery>
#include <QSqlError>

WorkController* WorkController::inst_ = nullptr;
WorkController* WorkController::getInstance() {
    if (!inst_) inst_ = new WorkController();
    return inst_;
}

int WorkController::createWork(int workTypeId, const QString& title,
                               const QString& descr,
                               const QDate& start, const QDate& end, int workers) {
    AdditionalWork w;
    w.setTitle(title); w.setDescr(descr);
    w.setPlanStart(start); w.setPlanEnd(end);
    w.setPlanWorkers(workers); w.setWorkTypeId(workTypeId);
    if (!w.save()) { err_ = DatabaseManager::getInstance()->lastError(); return 0; }
    return w.id();
}

bool WorkController::assignEmployee(int workId, int empId) {
    Assignment a;
    a.setWorkId(workId); a.setEmployeeId(empId);
    if (!a.save()) { err_ = DatabaseManager::getInstance()->lastError(); return false; }
    return true;
}

bool WorkController::completeAssignment(int assignId, const QDate& completed) {
    Assignment* a = Assignment::findById(assignId);
    if (!a) { err_ = "Назначение не найдено"; return false; }

    bool ok = a->complete(completed);   // ← ВЫЗЫВАЕМ complete(), а не save()

    if (!ok) err_ = DatabaseManager::getInstance()->lastError();
    delete a;
    return ok;
}

bool WorkController::deleteWork(int workId) {
    QSqlQuery query(DatabaseManager::getInstance()->db());

    // Сначала удаляем назначения
    query.prepare("DELETE FROM Назначение WHERE Номер_доп_работы = ?");
    query.addBindValue(workId);
    if (!query.exec()) {
        err_ = query.lastError().text();
        return false;
    }

    // Затем саму работу
    query.prepare("DELETE FROM Дополнительная_работа WHERE Номер_доп_работы = ?");
    query.addBindValue(workId);
    if (!query.exec()) {
        err_ = query.lastError().text();
        return false;
    }

    return true;
}

bool WorkController::updateWork(int workId, int workTypeId, const QString& title,
                                const QString& descr, const QDate& start,
                                const QDate& end, int workers) {
    AdditionalWork* w = AdditionalWork::findById(workId);
    if (!w) { err_ = "Работа не найдена"; return false; }

    w->setTitle(title);
    w->setDescr(descr);
    w->setPlanStart(start);
    w->setPlanEnd(end);
    w->setPlanWorkers(workers);
    w->setWorkTypeId(workTypeId);

    bool ok = w->save();
    if (!ok) err_ = DatabaseManager::getInstance()->lastError();
    delete w;
    return ok;
}

QList<AdditionalWork*> WorkController::getWorks() { return AdditionalWork::findAll(); }
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
    a->setCompleted(completed);
    bool ok = a->save();
    if (!ok) err_ = DatabaseManager::getInstance()->lastError();
    delete a;
    return ok;
}

QList<AdditionalWork*> WorkController::getWorks() { return AdditionalWork::findAll(); }

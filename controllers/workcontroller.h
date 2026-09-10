#pragma once
#include <QString>
#include <QList>
#include <QDate>
#include "../models/additionalwork.h"

class WorkController {
public:
    static WorkController* getInstance();
    int createWork(int workTypeId, const QString& title, const QString& descr,
                   const QDate& start, const QDate& end, int workers);
    bool assignEmployee(int workId, int empId);
    bool completeAssignment(int assignId, const QDate& completed);
    QList<AdditionalWork*> getWorks();
    QString lastError() const { return err_; }
private:
    WorkController() = default;
    static WorkController* inst_;
    QString err_;
};

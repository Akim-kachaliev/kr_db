#pragma once
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QDate>

class Assignment {
public:
    int id() const { return id_; }
    int employeeId() const { return empId_; }
    int workId() const { return workId_; }
    QDate assigned() const { return assigned_; }
    QDate started() const { return started_; }
    QDate completed() const { return completed_; }
    double payment() const { return payment_; }
    void setId(int v) { id_ = v; }
    void setEmployeeId(int v) { empId_ = v; }
    void setWorkId(int v) { workId_ = v; }
    void setAssigned(const QDate& v) { assigned_ = v; }
    void setStarted(const QDate& v) { started_ = v; }
    void setCompleted(const QDate& v) { completed_ = v; }
    void setPayment(double v) { payment_ = v; }
    bool save();
    bool complete(const QDate& completed);
    static QList<Assignment*> findByEmployee(int empId);
    static QList<Assignment*> findByWork(int workId);
    static Assignment* findById(int id);
private:
    int id_ = 0;
    int empId_ = 0;
    int workId_ = 0;
    QDate assigned_, started_, completed_;
    double payment_ = 0.0;
};

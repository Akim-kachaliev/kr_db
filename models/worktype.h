#pragma once
#include <QSqlQuery>
#include <QString>
#include <QList>

class WorkType {
public:
    int id() const { return id_; }
    QString name() const { return name_; }
    QString descr() const { return descr_; }
    double basePay() const { return basePay_; }
    double estHours() const { return estHours_; }
    void setId(int v) { id_ = v; }
    void setName(const QString& v) { name_ = v; }
    void setDescr(const QString& v) { descr_ = v; }
    void setBasePay(double v) { basePay_ = v; }
    void setEstHours(double v) { estHours_ = v; }
    static QList<WorkType*> findAll();
private:
    int id_ = 0;
    QString name_, descr_;
    double basePay_ = 0.0;
    double estHours_ = 0.0;
};

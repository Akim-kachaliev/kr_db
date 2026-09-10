#pragma once
#include <QSqlQuery>
#include <QString>
#include <QList>

class Position {
public:
    int id() const { return id_; }
    QString name() const { return name_; }
    double salary() const { return salary_; }
    void setId(int v) { id_ = v; }
    void setName(const QString& v) { name_ = v; }
    void setSalary(double v) { salary_ = v; }
    static QList<Position*> findAll();
private:
    int id_ = 0;
    QString name_;
    double salary_ = 0.0;
};

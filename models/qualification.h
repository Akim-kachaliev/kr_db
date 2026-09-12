#pragma once
#include <QSqlQuery>
#include <QString>
#include <QList>

class Qualification {
public:
    int id() const { return id_; }
    QString name() const { return name_; }
    double coeff() const { return coeff_; }
    void setId(int v) { id_ = v; }
    void setName(const QString& v) { name_ = v; }
    void setCoeff(double v) { coeff_ = v; }
    static QList<Qualification*> findAll();
private:
    int id_ = 0;
    QString name_;
    double coeff_ = 1.0;
};

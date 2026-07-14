#pragma once
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QDate>

class Employee {
public:
    int id() const { return id_; }
    QString lastName() const { return ln_; }
    QString firstName() const { return fn_; }
    QString middleName() const { return mn_; }
    QString phone() const { return phone_; }
    QDate birthDate() const { return bd_; }
    QDate hireDate() const { return hd_; }
    int qualId() const { return qualId_; }
    int posId() const { return posId_; }
    int userId() const { return userId_; }
    void setId(int v) { id_ = v; }
    void setLastName(const QString& v) { ln_ = v; }
    void setFirstName(const QString& v) { fn_ = v; }
    void setMiddleName(const QString& v) { mn_ = v; }
    void setPhone(const QString& v) { phone_ = v; }
    void setBirthDate(const QDate& v) { bd_ = v; }
    void setHireDate(const QDate& v) { hd_ = v; }
    void setQualId(int v) { qualId_ = v; }
    void setPosId(int v) { posId_ = v; }
    void setUserId(int v) { userId_ = v; }
    QString fullName() const;
    static QList<Employee*> findAll();
    static Employee* findById(int id);
private:
    int id_ = 0;
    QString ln_, fn_, mn_, phone_;
    QDate bd_, hd_;
    int qualId_ = 0, posId_ = 0, userId_ = 0;
};

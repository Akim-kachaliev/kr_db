#pragma once
#include <QSqlQuery>
#include <QString>

class User {
public:
    int id() const { return id_; }
    QString name() const { return name_; }
    QString login() const { return login_; }
    QString email() const { return email_; }
    int roleId() const { return roleId_; }
    QString passwordHash() const { return hash_; }
    void setId(int v) { id_ = v; }
    void setName(const QString& v) { name_ = v; }
    void setLogin(const QString& v) { login_ = v; }
    void setEmail(const QString& v) { email_ = v; }
    void setRoleId(int v) { roleId_ = v; }
    void setPasswordHash(const QString& v) { hash_ = v; }
    bool hasRole(const QString& roleName) const;
    bool save();
    static User* findByLogin(const QString& login);
private:
    int id_ = 0;
    QString name_, login_, email_, hash_;
    int roleId_ = 0;
};

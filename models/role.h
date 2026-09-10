#pragma once
#include <QSqlQuery>
#include <QString>
#include <QList>

class Role {
public:
    int id() const { return id_; }
    QString name() const { return name_; }
    void setId(int v) { id_ = v; }
    void setName(const QString& v) { name_ = v; }
    static Role* findById(int id);
    static QList<Role*> findAll();
private:
    int id_ = 0;
    QString name_;
};

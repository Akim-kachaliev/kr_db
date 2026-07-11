#include "user.h"
#include <QSqlError>
#include "role.h"
#include "databasemanager.h"

bool User::hasRole(const QString& roleName) const {
    Role* r = Role::findById(roleId_);
    if (!r) return false;
    bool m = (r->name() == roleName);
    delete r;
    return m;
}

bool User::save() {
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (id_ == 0) {
        q.prepare("INSERT INTO List (Номер_роли, Имя, Логин, email, Хеш_пароля) "
                  "VALUES (?, ?, ?, ?, ?) RETURNING Номер_пользователя");
        q.addBindValue(roleId_);
        q.addBindValue(name_);
        q.addBindValue(login_);
        q.addBindValue(email_);
        q.addBindValue(hash_);
    } else {
        q.prepare("UPDATE List SET Имя=?, Логин=?, email=?, Номер_роли=? "
                  "WHERE Номер_пользователя=?");
        q.addBindValue(name_);
        q.addBindValue(login_);
        q.addBindValue(email_);
        q.addBindValue(roleId_);
        q.addBindValue(id_);
    }
    if (!q.exec()) {
        DatabaseManager::getInstance()->setLastError(q.lastError().text());
        return false;
    }
    if (id_ == 0 && q.next()) id_ = q.value(0).toInt();
    return true;
}

User* User::findByLogin(const QString& login) {
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Номер_пользователя, Имя, Логин, email, Хеш_пароля, Номер_роли "
              "FROM List WHERE Логин = ?");
    q.addBindValue(login);
    if (!q.exec() || !q.next()) return nullptr;
    auto* u = new User();
    u->id_ = q.value(0).toInt();
    u->name_ = q.value(1).toString();
    u->login_ = q.value(2).toString();
    u->email_ = q.value(3).toString();
    u->hash_ = q.value(4).toString();
    u->roleId_ = q.value(5).toInt();
    return u;
}

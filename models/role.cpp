#include "role.h"
#include <QSqlError>
#include "databasemanager.h"

Role* Role::findById(int id) {
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Номер_роли, Наименование FROM Роль WHERE Номер_роли = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return nullptr;
    auto* r = new Role();
    r->id_ = q.value(0).toInt();
    r->name_ = q.value(1).toString();
    return r;
}

QList<Role*> Role::findAll() {
    QList<Role*> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Номер_роли, Наименование FROM Роль ORDER BY Номер_роли"))
        return result;
    while (q.next()) {
        auto* r = new Role();
        r->id_ = q.value(0).toInt();
        r->name_ = q.value(1).toString();
        result.append(r);
    }
    return result;
}

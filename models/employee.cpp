#include "employee.h"
#include <QSqlError>
#include "databasemanager.h"

QString Employee::fullName() const {
    QString r = ln_;
    if (!fn_.isEmpty()) r += " " + fn_.left(1) + ".";
    if (!mn_.isEmpty()) r += " " + mn_.left(1) + ".";
    return r;
}

QList<Employee*> Employee::findAll() {
    QList<Employee*> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Номер_сотрудника, Фамилия, Имя, Отчество, Телефон, "
                "Дата_рождения, Дата_приема, Номер_квалификации, "
                "Номер_должности, Номер_пользователя "
                "FROM Сотрудник ORDER BY Фамилия, Имя"))
        return result;
    while (q.next()) {
        auto* e = new Employee();
        e->id_ = q.value(0).toInt();
        e->ln_ = q.value(1).toString();
        e->fn_ = q.value(2).toString();
        e->mn_ = q.value(3).toString();
        e->phone_ = q.value(4).toString();
        e->bd_ = q.value(5).toDate();
        e->hd_ = q.value(6).toDate();
        e->qualId_ = q.value(7).toInt();
        e->posId_ = q.value(8).toInt();
        e->userId_ = q.value(9).toInt();
        result.append(e);
    }
    return result;
}

Employee* Employee::findById(int id) {
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Номер_сотрудника, Фамилия, Имя, Отчество, Телефон, "
              "Дата_рождения, Дата_приема, Номер_квалификации, "
              "Номер_должности, Номер_пользователя "
              "FROM Сотрудник WHERE Номер_сотрудника = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return nullptr;
    auto* e = new Employee();
    e->id_ = q.value(0).toInt();
    e->ln_ = q.value(1).toString();
    e->fn_ = q.value(2).toString();
    e->mn_ = q.value(3).toString();
    e->phone_ = q.value(4).toString();
    e->bd_ = q.value(5).toDate();
    e->hd_ = q.value(6).toDate();
    e->qualId_ = q.value(7).toInt();
    e->posId_ = q.value(8).toInt();
    e->userId_ = q.value(9).toInt();
    return e;
}

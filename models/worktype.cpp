#include "worktype.h"
#include <QSqlError>
#include "databasemanager.h"

QList<WorkType*> WorkType::findAll() {
    QList<WorkType*> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Код_вида_работы, Наименование, Описание, "
                "Базовая_оплата, Нормативная_длительность "
                "FROM Вид_работы ORDER BY Наименование"))
        return result;
    while (q.next()) {
        auto* w = new WorkType();
        w->id_ = q.value(0).toInt();
        w->name_ = q.value(1).toString();
        w->descr_ = q.value(2).toString();
        w->basePay_ = q.value(3).toDouble();
        w->estHours_ = q.value(4).toDouble();
        result.append(w);
    }
    return result;
}

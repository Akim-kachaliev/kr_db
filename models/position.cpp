#include "position.h"
#include <QSqlError>
#include "databasemanager.h"

QList<Position*> Position::findAll() {
    QList<Position*> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Номер_должности, Наименование, Базовый_оклад "
                "FROM Должность ORDER BY Наименование"))
        return result;
    while (q.next()) {
        auto* p = new Position();
        p->id_ = q.value(0).toInt();
        p->name_ = q.value(1).toString();
        p->salary_ = q.value(2).toDouble();
        result.append(p);
    }
    return result;
}

#include "qualification.h"
#include <QSqlError>
#include "databasemanager.h"

QList<Qualification*> Qualification::findAll() {
    QList<Qualification*> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Номер_квалификации, Наименование, Коэффициент "
                "FROM Квалификация ORDER BY Наименование"))
        return result;
    while (q.next()) {
        auto* ql = new Qualification();
        ql->id_ = q.value(0).toInt();
        ql->name_ = q.value(1).toString();
        ql->coeff_ = q.value(2).toDouble();
        result.append(ql);
    }
    return result;
}

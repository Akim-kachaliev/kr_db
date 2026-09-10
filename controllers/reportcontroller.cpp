#include "reportcontroller.h"
#include <QSqlError>
#include "../models/databasemanager.h"
#include <QSqlQuery>

ReportController* ReportController::inst_ = nullptr;
ReportController* ReportController::getInstance() {
    if (!inst_) inst_ = new ReportController();
    return inst_;
}

QList<BonusRow> ReportController::bonusSummary() {
    QList<BonusRow> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec(
            "SELECT s.Номер_сотрудника, "
            "s.Фамилия || ' ' || LEFT(s.Имя,1) || '. ' || "
            "COALESCE(LEFT(s.Отчество,1) || '.', '') AS fio, "
            "d.Наименование AS pos, k.Наименование AS qual, "
            "COUNT(n.Номер_назначения) AS cnt, "
            "COALESCE(SUM(n.Сумма_доплаты), 0) AS total "
            "FROM Сотрудник s "
            "JOIN Должность d ON d.Номер_должности = s.Номер_должности "
            "JOIN Квалификация k ON k.Номер_квалификации = s.Номер_квалификации "
            "LEFT JOIN Назначение n ON n.Номер_сотрудника = s.Номер_сотрудника "
            "  AND n.Дата_завершения IS NOT NULL "
            "GROUP BY s.Номер_сотрудника, s.Фамилия, s.Имя, s.Отчество, "
            "d.Наименование, k.Наименование "
            "ORDER BY total DESC")) {
        err_ = q.lastError().text();
        return result;
    }
    while (q.next()) {
        BonusRow r;
        r.id = q.value(0).toInt();
        r.name = q.value(1).toString();
        r.position = q.value(2).toString();
        r.qual = q.value(3).toString();
        r.count = q.value(4).toInt();
        r.total = q.value(5).toDouble();
        result.append(r);
    }
    return result;
}

#include "additionalwork.h"
#include <QSqlError>
#include "databasemanager.h"

bool AdditionalWork::save() {
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (id_ == 0) {
        q.prepare("INSERT INTO Дополнительная_работа "
                  "(Наименование, Описание, Плановое_начало, Плановое_окончание, "
                  "Плановое_число_исполнителей, Код_вида_работы) "
                  "VALUES (?, ?, ?, ?, ?, ?) RETURNING Номер_доп_работы");
        q.addBindValue(title_);
        q.addBindValue(descr_);
        q.addBindValue(ps_);
        q.addBindValue(pe_);
        q.addBindValue(pw_);
        q.addBindValue(wtId_);
    } else {
        q.prepare("UPDATE Дополнительная_работа SET Наименование=?, Описание=?, "
                  "Плановое_начало=?, Плановое_окончание=?, "
                  "Плановое_число_исполнителей=?, Код_вида_работы=? "
                  "WHERE Номер_доп_работы=?");
        q.addBindValue(title_);
        q.addBindValue(descr_);
        q.addBindValue(ps_);
        q.addBindValue(pe_);
        q.addBindValue(pw_);
        q.addBindValue(wtId_);
        q.addBindValue(id_);
    }
    if (!q.exec()) {
        DatabaseManager::getInstance()->setLastError(q.lastError().text());
        return false;
    }
    if (id_ == 0 && q.next()) id_ = q.value(0).toInt();
    return true;
}

QList<AdditionalWork*> AdditionalWork::findAll() {
    QList<AdditionalWork*> result;
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT Номер_доп_работы, Наименование, Описание, "
                "Дата_создания, Плановое_начало, Плановое_окончание, "
                "Плановое_число_исполнителей, Код_вида_работы "
                "FROM Дополнительная_работа ORDER BY Дата_создания DESC"))
        return result;
    while (q.next()) {
        auto* w = new AdditionalWork();
        w->id_ = q.value(0).toInt();
        w->title_ = q.value(1).toString();
        w->descr_ = q.value(2).toString();
        w->created_ = q.value(3).toDate();
        w->ps_ = q.value(4).toDate();
        w->pe_ = q.value(5).toDate();
        w->pw_ = q.value(6).toInt();
        w->wtId_ = q.value(7).toInt();
        result.append(w);
    }
    return result;
}

AdditionalWork* AdditionalWork::findById(int id) {
    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Номер_доп_работы, Наименование, Описание, "
              "Дата_создания, Плановое_начало, Плановое_окончание, "
              "Плановое_число_исполнителей, Код_вида_работы "
              "FROM Дополнительная_работа WHERE Номер_доп_работы = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return nullptr;
    auto* w = new AdditionalWork();
    w->id_ = q.value(0).toInt();
    w->title_ = q.value(1).toString();
    w->descr_ = q.value(2).toString();
    w->created_ = q.value(3).toDate();
    w->ps_ = q.value(4).toDate();
    w->pe_ = q.value(5).toDate();
    w->pw_ = q.value(6).toInt();
    w->wtId_ = q.value(7).toInt();
    return w;
}

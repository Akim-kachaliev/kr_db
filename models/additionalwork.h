#pragma once
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QDate>

class AdditionalWork {
public:
    int id() const { return id_; }
    QString title() const { return title_; }
    QString descr() const { return descr_; }
    QDate created() const { return created_; }
    QDate planStart() const { return ps_; }
    QDate planEnd() const { return pe_; }
    int planWorkers() const { return pw_; }
    int workTypeId() const { return wtId_; }
    void setId(int v) { id_ = v; }
    void setTitle(const QString& v) { title_ = v; }
    void setDescr(const QString& v) { descr_ = v; }
    void setCreated(const QDate& v) { created_ = v; }
    void setPlanStart(const QDate& v) { ps_ = v; }
    void setPlanEnd(const QDate& v) { pe_ = v; }
    void setPlanWorkers(int v) { pw_ = v; }
    void setWorkTypeId(int v) { wtId_ = v; }
    bool save();
    static QList<AdditionalWork*> findAll();
    static AdditionalWork* findById(int id);
private:
    int id_ = 0;
    QString title_, descr_;
    QDate created_, ps_, pe_;
    int pw_ = 1;
    int wtId_ = 0;
};

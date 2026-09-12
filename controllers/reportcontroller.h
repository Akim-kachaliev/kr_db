#pragma once
#include <QString>
#include <QList>

struct BonusRow {
    int id;
    QString name;
    QString position;
    QString qual;
    int count;
    double total;
};

class ReportController {
public:
    static ReportController* getInstance();
    QList<BonusRow> bonusSummary();
    QString lastError() const { return err_; }
private:
    ReportController() = default;
    static ReportController* inst_;
    QString err_;
};

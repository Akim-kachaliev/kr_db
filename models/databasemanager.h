#pragma once
#include <QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager* getInstance();
    bool openConnection(const QString& host, int port,
                        const QString& dbName, const QString& user,
                        const QString& password);
    void closeConnection();
    bool beginTransaction();
    bool commit();
    bool rollback();
    QSqlDatabase& db();
    QString lastError() const;
    void setLastError(const QString& err);
private:
    DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    static DatabaseManager* instance_;
    QSqlDatabase db_;
    bool connected_ = false;
    QString lastError_;
};

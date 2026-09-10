#include "databasemanager.h"
#include <QSqlError>

DatabaseManager* DatabaseManager::instance_ = nullptr;

DatabaseManager* DatabaseManager::getInstance() {
    if (!instance_) instance_ = new DatabaseManager();
    return instance_;
}

bool DatabaseManager::openConnection(const QString& host, int port,
                                     const QString& dbName,
                                     const QString& user,
                                     const QString& password) {
    if (connected_) return true;
    db_ = QSqlDatabase::addDatabase("QPSQL", "isrdo_conn");
    db_.setHostName(host);
    db_.setPort(port);
    db_.setDatabaseName(dbName);
    db_.setUserName(user);
    db_.setPassword(password);
    connected_ = db_.open();
    if (!connected_) lastError_ = db_.lastError().text();
    return connected_;
}

void DatabaseManager::closeConnection() {
    if (connected_) { db_.close(); connected_ = false; }
    QSqlDatabase::removeDatabase("isrdo_conn");
}

bool DatabaseManager::beginTransaction() { return connected_ ? db_.transaction() : false; }
bool DatabaseManager::commit()           { return connected_ ? db_.commit()    : false; }
bool DatabaseManager::rollback()         { return connected_ ? db_.rollback()  : false; }
QSqlDatabase& DatabaseManager::db()      { return db_; }
QString DatabaseManager::lastError() const { return lastError_; }
void DatabaseManager::setLastError(const QString& err) { lastError_ = err; }

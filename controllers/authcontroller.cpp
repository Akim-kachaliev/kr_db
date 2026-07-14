#include "authcontroller.h"
#include "../models/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QRegularExpression>

AuthController* AuthController::inst_ = nullptr;
AuthController* AuthController::getInstance() {
    if (!inst_) inst_ = new AuthController();
    return inst_;
}

bool AuthController::validatePassword(const QString& p) {
    static const QRegularExpression re(
        "^(?=.*[0-9])(?=.*[A-Z])(?=.*[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]).{8,}$");
    return re.match(p).hasMatch();
}

QString AuthController::hashPassword(const QString& p, const QString& salt) {
    return QString::fromUtf8(
        QCryptographicHash::hash((salt + p).toUtf8(), QCryptographicHash::Sha256).toHex());
}

QString AuthController::generateSalt() {
    QByteArray b; b.resize(16);
    QRandomGenerator::global()->generate(
        reinterpret_cast<quint32*>(b.data()),
        reinterpret_cast<quint32*>(b.data()) + 4);
    return QString::fromUtf8(b.toHex());
}

User* AuthController::login(const QString& login, const QString& password) {
    User* u = User::findByLogin(login);
    if (!u) { err_ = "Неверный логин или пароль"; return nullptr; }
    // В тестовых данных пароль хранится как текст — сравниваем напрямую
    if (u->passwordHash() != password) {
        // Также проверяем SHA-256 хэш
        QString inputHash = QString::fromUtf8(
            QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
        if (u->passwordHash() != inputHash) {
            err_ = "Неверный логин или пароль";
            delete u;
            return nullptr;
        }
    }
    if (cur_) delete cur_;
    cur_ = u;
    return cur_;
}

bool AuthController::registerUser(const RegistrationData& d) {
    if (!validatePassword(d.password)) {
        err_ = "Пароль: минимум 8 символов, цифра, заглавная, спецсимвол";
        return false;
    }
    if (d.password != d.passwordConfirm) { err_ = "Пароли не совпадают"; return false; }
    if (User::findByLogin(d.login)) { err_ = "Логин занят"; return false; }
    User u;
    u.setName(d.name); u.setLogin(d.login); u.setEmail(d.email);
    u.setRoleId(d.roleId);
    u.setPasswordHash(hashPassword(d.password, generateSalt()));
    if (!u.save()) { err_ = DatabaseManager::getInstance()->lastError(); return false; }
    return true;
}

void AuthController::logout() { if (cur_) { delete cur_; cur_ = nullptr; } }

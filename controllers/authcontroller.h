#pragma once
#include <QString>
#include "../models/user.h"

struct RegistrationData {
    QString login, password, passwordConfirm, name, email;
    int roleId = 3;
};

class AuthController {
public:
    static AuthController* getInstance();
    User* login(const QString& login, const QString& password);
    bool registerUser(const RegistrationData& d);
    void logout();
    User* currentUser() const { return cur_; }
    QString lastError() const { return err_; }
    static bool validatePassword(const QString& p);
    static QString hashPassword(const QString& p, const QString& salt);
    static QString generateSalt();
private:
    AuthController() = default;
    static AuthController* inst_;
    User* cur_ = nullptr;
    QString err_;
};

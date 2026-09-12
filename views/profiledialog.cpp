#include "profiledialog.h"
#include "../controllers/authcontroller.h"
#include "../models/databasemanager.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

ProfileDialog::ProfileDialog(QWidget* p) : QDialog(p) {
    setWindowTitle("Мой профиль");
    setMinimumWidth(400);
    setupUi();
    loadCurrentUser();
}

void ProfileDialog::setupUi() {
    auto* m = new QVBoxLayout(this);
    auto* title = new QLabel("<h3>Редактирование профиля</h3>", this);
    title->setAlignment(Qt::AlignCenter);
    m->addWidget(title);
    
    auto* f = new QFormLayout();
    nameEdit_ = new QLineEdit(this);
    emailEdit_ = new QLineEdit(this);
    pwdEdit_ = new QLineEdit(this);
    pwdEdit_->setEchoMode(QLineEdit::Password);
    pwdEdit_->setPlaceholderText("Оставьте пустым, чтобы не менять");
    pwd2Edit_ = new QLineEdit(this);
    pwd2Edit_->setEchoMode(QLineEdit::Password);
    pwd2Edit_->setPlaceholderText("Повторите новый пароль");
    
    f->addRow("Имя:", nameEdit_);
    f->addRow("Email:", emailEdit_);
    f->addRow("Новый пароль:", pwdEdit_);
    f->addRow("Повтор пароля:", pwd2Edit_);
    m->addLayout(f);
    
    auto* r = new QHBoxLayout();
    saveBtn_ = new QPushButton("Сохранить", this);
    cancelBtn_ = new QPushButton("Отмена", this);
    r->addStretch();
    r->addWidget(saveBtn_);
    r->addWidget(cancelBtn_);
    m->addLayout(r);
    
    connect(saveBtn_, &QPushButton::clicked, this, &ProfileDialog::onSave);
    connect(cancelBtn_, &QPushButton::clicked, this, &QDialog::reject);
}

void ProfileDialog::loadCurrentUser() {
    auto* u = AuthController::getInstance()->currentUser();
    if (!u) return;
    nameEdit_->setText(u->name());
    emailEdit_->setText(u->email());
}

void ProfileDialog::onSave() {
    auto* u = AuthController::getInstance()->currentUser();
    if (!u) {
        QMessageBox::critical(this, "Ошибка", "Пользователь не найден");
        return;
    }
    
    QString newName = nameEdit_->text().trimmed();
    QString newEmail = emailEdit_->text().trimmed();
    QString newPwd = pwdEdit_->text();
    QString newPwd2 = pwd2Edit_->text();
    
    if (newName.isEmpty() || newEmail.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Имя и Email обязательны");
        return;
    }
    
    // Если меняем пароль — проверяем
    if (!newPwd.isEmpty()) {
        if (newPwd != newPwd2) {
            QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
            return;
        }
        if (!AuthController::validatePassword(newPwd)) {
            QMessageBox::warning(this, "Ошибка",
                "Пароль должен содержать минимум 8 символов, "
                "хотя бы одну цифру, одну заглавную букву и один спецсимвол");
            return;
        }
    }
    
    // Формируем SQL UPDATE
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!newPwd.isEmpty()) {
        q.prepare("UPDATE List SET Имя=?, email=?, Хеш_пароля=? "
                  "WHERE Номер_пользователя=?");
        q.addBindValue(newName);
        q.addBindValue(newEmail);
        q.addBindValue(newPwd);
        q.addBindValue(u->id());
    } else {
        q.prepare("UPDATE List SET Имя=?, email=? WHERE Номер_пользователя=?");
        q.addBindValue(newName);
        q.addBindValue(newEmail);
        q.addBindValue(u->id());
    }
    
    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка",
            "Не удалось сохранить: " + q.lastError().text());
        return;
    }
    
    // Обновляем объект в памяти
    u->setName(newName);
    u->setEmail(newEmail);
    if (!newPwd.isEmpty()) u->setPasswordHash(newPwd);
    
    QMessageBox::information(this, "Успех", "Профиль обновлён");
    accept();
}
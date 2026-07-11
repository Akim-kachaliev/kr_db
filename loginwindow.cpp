#include "loginwindow.h"
#include "../controllers/authcontroller.h"
#include "../models/role.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget* p) : QDialog(p) {
    setWindowTitle("ИС РДО — Вход");
    setModal(true);
    setupUi();
    setMode(false);
}

void LoginWindow::setupUi() {
    auto* m = new QVBoxLayout(this);
    auto* t = new QLabel("<h2>ИС распределения<br>дополнительных обязанностей</h2>", this);
    t->setAlignment(Qt::AlignCenter);
    m->addWidget(t);
    auto* f = new QFormLayout();
    login_ = new QLineEdit(this);
    pwd_ = new QLineEdit(this); pwd_->setEchoMode(QLineEdit::Password);
    pwd2_ = new QLineEdit(this); pwd2_->setEchoMode(QLineEdit::Password);
    name_ = new QLineEdit(this);
    email_ = new QLineEdit(this);
    role_ = new QComboBox(this);
    f->addRow("Логин:", login_);
    f->addRow("Пароль:", pwd_);
    f->addRow("Повтор:", pwd2_);
    f->addRow("Имя:", name_);
    f->addRow("Email:", email_);
    f->addRow("Роль:", role_);
    m->addLayout(f);
    auto* r = new QHBoxLayout();
    btnLogin_ = new QPushButton("Войти", this);
    btnReg_ = new QPushButton("Регистрация", this);
    btnSwitch_ = new QPushButton("Нет аккаунта? Зарегистрироваться", this);
    r->addWidget(btnLogin_); r->addWidget(btnReg_);
    m->addLayout(r); m->addWidget(btnSwitch_);
    auto roles = Role::findAll();
    for (Role* rl : roles) {
        role_->addItem(rl->name(), rl->id());
        delete rl;
    }
    connect(btnLogin_, &QPushButton::clicked, this, &LoginWindow::onLogin);
    connect(btnReg_, &QPushButton::clicked, this, &LoginWindow::onRegister);
    connect(btnSwitch_, &QPushButton::clicked, this, &LoginWindow::onSwitch);
}

void LoginWindow::setMode(bool reg) {
    regMode_ = reg;
    pwd2_->setVisible(reg); name_->setVisible(reg);
    email_->setVisible(reg); role_->setVisible(reg);
    btnLogin_->setVisible(!reg); btnReg_->setVisible(reg);
    btnSwitch_->setText(reg ? "Уже есть аккаунт? Войти"
                           : "Нет аккаунта? Зарегистрироваться");
}

void LoginWindow::onSwitch() { setMode(!regMode_); }

void LoginWindow::onLogin() {
    if (login_->text().trimmed().isEmpty() || pwd_->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните логин и пароль");
        return;
    }
    auto* a = AuthController::getInstance();
    if (!a->login(login_->text().trimmed(), pwd_->text())) {
        QMessageBox::critical(this, "Ошибка", a->lastError());
        return;
    }
    accept();
}

void LoginWindow::onRegister() {
    RegistrationData d;
    d.login = login_->text().trimmed();
    d.password = pwd_->text();
    d.passwordConfirm = pwd2_->text();
    d.name = name_->text().trimmed();
    d.email = email_->text().trimmed();
    d.roleId = role_->currentData().toInt();
    auto* a = AuthController::getInstance();
    if (!a->registerUser(d)) {
        QMessageBox::critical(this, "Ошибка", a->lastError());
        return;
    }
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно.");
    setMode(false);
}

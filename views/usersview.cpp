#include "usersview.h"
#include "../models/databasemanager.h"
#include "../controllers/authcontroller.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>

// Диалог создания/редактирования пользователя

class UserDialog : public QDialog {
public:
    UserDialog(QWidget* p = nullptr) : QDialog(p) {
        setWindowTitle("Пользователь");
        setMinimumWidth(400);
        auto* f = new QFormLayout(this);

        nameEdit_ = new QLineEdit(this);
        loginEdit_ = new QLineEdit(this);
        emailEdit_ = new QLineEdit(this);
        pwdEdit_ = new QLineEdit(this);
        pwdEdit_->setEchoMode(QLineEdit::Password);
        pwdEdit_->setPlaceholderText("Оставьте пустым, чтобы не менять");

        roleCombo_ = new QComboBox(this);
        QSqlQuery q(DatabaseManager::getInstance()->db());
        if (q.exec("SELECT Номер_роли, Наименование FROM Роль ORDER BY Номер_роли")) {
            while (q.next()) {
                roleCombo_->addItem(q.value(1).toString(), q.value(0).toInt());
            }
        }

        f->addRow("Имя:", nameEdit_);
        f->addRow("Логин:", loginEdit_);
        f->addRow("Email:", emailEdit_);
        f->addRow("Пароль:", pwdEdit_);
        f->addRow("Роль:", roleCombo_);

        auto* b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        f->addRow(b);
        connect(b, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(b, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

    void setData(const QString& name, const QString& login,
                 const QString& email, int roleId) {
        nameEdit_->setText(name);
        loginEdit_->setText(login);
        emailEdit_->setText(email);
        int idx = roleCombo_->findData(roleId);
        if (idx >= 0) roleCombo_->setCurrentIndex(idx);
    }

    QString name() const { return nameEdit_->text().trimmed(); }
    QString login() const { return loginEdit_->text().trimmed(); }
    QString email() const { return emailEdit_->text().trimmed(); }
    QString password() const { return pwdEdit_->text(); }
    int roleId() const { return roleCombo_->currentData().toInt(); }

private:
    QLineEdit *nameEdit_, *loginEdit_, *emailEdit_, *pwdEdit_;
    QComboBox* roleCombo_;
};


UsersView::UsersView(QWidget* p) : QWidget(p) {
    setupUi();
    refresh();
}

void UsersView::setupUi() {
    auto* m = new QVBoxLayout(this);

    table_ = new QTableView(this);
    model_ = new QStandardItemModel(this);
    model_->setHorizontalHeaderLabels(
        {"ID", "Имя", "Логин", "Email", "Роль", "Дата регистрации"});
    table_->setModel(model_);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    m->addWidget(table_);

    auto* r = new QHBoxLayout();
    createBtn_ = new QPushButton("Создать", this);
    editBtn_ = new QPushButton("Редактировать", this);
    deleteBtn_ = new QPushButton("Удалить", this);
    refreshBtn_ = new QPushButton("Обновить", this);
    status_ = new QLabel(this);

    r->addWidget(createBtn_);
    r->addWidget(editBtn_);
    r->addWidget(deleteBtn_);
    r->addWidget(refreshBtn_);
    r->addStretch();
    r->addWidget(status_);
    m->addLayout(r);

    connect(createBtn_, &QPushButton::clicked, this, &UsersView::onCreate);
    connect(editBtn_, &QPushButton::clicked, this, &UsersView::onEdit);
    connect(deleteBtn_, &QPushButton::clicked, this, &UsersView::onDelete);
    connect(refreshBtn_, &QPushButton::clicked, this, &UsersView::refresh);
}

void UsersView::refresh() {
    model_->removeRows(0, model_->rowCount());
    QSqlQuery q(DatabaseManager::getInstance()->db());
    if (!q.exec("SELECT п.Номер_пользователя, п.Имя, п.Логин, п.email, "
                "р.Наименование, п.Дата_регистрации "
                "FROM List п JOIN Роль р ON р.Номер_роли = п.Номер_роли "
                "ORDER BY п.Номер_пользователя")) {
        QMessageBox::critical(this, "Ошибка", q.lastError().text());
        return;
    }
    while (q.next()) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(q.value(0).toInt()))
            << new QStandardItem(q.value(1).toString())
            << new QStandardItem(q.value(2).toString())
            << new QStandardItem(q.value(3).toString())
            << new QStandardItem(q.value(4).toString())
            << new QStandardItem(q.value(5).toDate().toString("dd.MM.yyyy"));
        model_->appendRow(row);
    }
    status_->setText(QString("Записей: %1").arg(model_->rowCount()));
}

void UsersView::onCreate() {
    UserDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    if (dlg.name().isEmpty() || dlg.login().isEmpty() || dlg.password().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните имя, логин и пароль");
        return;
    }
    if (!AuthController::validatePassword(dlg.password())) {
        QMessageBox::warning(this, "Ошибка",
                             "Пароль должен содержать минимум 8 символов, "
                             "хотя бы одну цифру, одну заглавную букву и один спецсимвол");
        return;
    }

    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("INSERT INTO List (Номер_роли, Имя, Логин, email, Хеш_пароля) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(dlg.roleId());
    q.addBindValue(dlg.name());
    q.addBindValue(dlg.login());
    q.addBindValue(dlg.email());
    q.addBindValue(dlg.password());

    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка", q.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Пользователь создан");
    refresh();
}

void UsersView::onEdit() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Внимание", "Выберите пользователя");
        return;
    }
    int id = model_->item(idx.row(), 0)->text().toInt();

    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("SELECT Имя, Логин, email, Номер_роли FROM List "
              "WHERE Номер_пользователя=?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return;

    UserDialog dlg(this);
    dlg.setData(q.value(0).toString(), q.value(1).toString(),
                q.value(2).toString(), q.value(3).toInt());
    if (dlg.exec() != QDialog::Accepted) return;

    QSqlQuery upd(DatabaseManager::getInstance()->db());
    if (!dlg.password().isEmpty()) {
        upd.prepare("UPDATE List SET Имя=?, Логин=?, email=?, Номер_роли=?, "
                    "Хеш_пароля=? WHERE Номер_пользователя=?");
        upd.addBindValue(dlg.name());
        upd.addBindValue(dlg.login());
        upd.addBindValue(dlg.email());
        upd.addBindValue(dlg.roleId());
        upd.addBindValue(dlg.password());
        upd.addBindValue(id);
    } else {
        upd.prepare("UPDATE List SET Имя=?, Логин=?, email=?, Номер_роли=? "
                    "WHERE Номер_пользователя=?");
        upd.addBindValue(dlg.name());
        upd.addBindValue(dlg.login());
        upd.addBindValue(dlg.email());
        upd.addBindValue(dlg.roleId());
        upd.addBindValue(id);
    }
    if (!upd.exec()) {
        QMessageBox::critical(this, "Ошибка", upd.lastError().text());
        return;
    }
    QMessageBox::information(this, "Успех", "Пользователь обновлён");
    refresh();
}

void UsersView::onDelete() {
    QModelIndex idx = table_->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Внимание", "Выберите пользователя");
        return;
    }
    int id = model_->item(idx.row(), 0)->text().toInt();
    QString login = model_->item(idx.row(), 2)->text();

    // Нельзя удалить самого себя
    auto* u = AuthController::getInstance()->currentUser();
    if (u && u->id() == id) {
        QMessageBox::warning(this, "Ошибка",
                             "Нельзя удалить собственную учётную запись");
        return;
    }

    if (QMessageBox::question(this, "Подтверждение",
                              QString("Удалить пользователя «%1» (ID=%2)?").arg(login).arg(id))
        != QMessageBox::Yes) return;

    QSqlQuery q(DatabaseManager::getInstance()->db());
    q.prepare("DELETE FROM List WHERE Номер_пользователя=?");
    q.addBindValue(id);
    if (!q.exec()) {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось удалить: " + q.lastError().text() +
                                  "\n\nВозможно, пользователь связан с сотрудником.");
        return;
    }
    QMessageBox::information(this, "Успех", "Пользователь удалён");
    refresh();
}
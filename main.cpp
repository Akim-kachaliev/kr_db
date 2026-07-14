#include <QApplication>
#include <QDialog>
#include <QWidget>
#include <QMessageBox>
#include <QStyleFactory>
#include "models/databasemanager.h"
#include "controllers/authcontroller.h"
#include "views/loginwindow.h"
#include "views/mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("ИС РДО");
    QApplication::setOrganizationName("МИЭТ");
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    auto* dbm = DatabaseManager::getInstance();
    if (!dbm->openConnection("localhost", 5432, "kr_db",
                             "postgres", "postgres")) {
        QMessageBox::critical(nullptr, "Ошибка",
            "Не удалось подключиться к БД:\n" + dbm->lastError());
        return 1;
    }

    AuthController* auth = AuthController::getInstance();
    LoginWindow loginWnd;
    while (loginWnd.exec() == QDialog::Accepted) {
        if (!auth->currentUser()) continue;
        MainWindow mainWnd;
        mainWnd.show();
        int rc = app.exec();
        dbm->closeConnection();
        return rc;
    }
    dbm->closeConnection();
    return 0;
}

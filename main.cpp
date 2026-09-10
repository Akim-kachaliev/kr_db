#include <QApplication>
#include <QDialog>
#include <QWidget>
#include <QMessageBox>
#include <QStyleFactory>
#include <QSqlDatabase>
#include <QSettings>
#include "models/databasemanager.h"
#include "controllers/authcontroller.h"
#include "views/loginwindow.h"
#include "views/mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("ИС РДО");
    QApplication::setOrganizationName("МИЭТ");
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        QMessageBox::critical(nullptr, "Ошибка",
            "Драйвер QPSQL не доступен. Доступные драйверы:\n" +
            QSqlDatabase::drivers().join(", ") +
            "\n\nУбедитесь, что плагин драйвера (qsqlpsql.dll) и клиентская библиотека PostgreSQL (libpq.dll) установлены и доступны.");
        return 1;
    }

    // Загрузка настроек подключения из config.ini
    QString iniPath = QApplication::applicationDirPath() + "/config.ini";
    QSettings settings(iniPath, QSettings::IniFormat);
    if (!settings.contains("database/host")) settings.setValue("database/host", "localhost");
    if (!settings.contains("database/port")) settings.setValue("database/port", 5432);
    if (!settings.contains("database/dbname")) settings.setValue("database/dbname", "kr_db");
    if (!settings.contains("database/user")) settings.setValue("database/user", "postgres");
    if (!settings.contains("database/password")) settings.setValue("database/password", "postgres");
    settings.sync();

    QString host = settings.value("database/host").toString();
    int port = settings.value("database/port").toInt();
    QString dbName = settings.value("database/dbname").toString();
    QString user = settings.value("database/user").toString();
    QString password = settings.value("database/password").toString();

    auto* dbm = DatabaseManager::getInstance();
    if (!dbm->openConnection(host, port, dbName, user, password)) {
        QMessageBox::critical(nullptr, "Ошибка",
            "Не удалось подключиться к БД:\n" + dbm->lastError() +
            "\n\nНастройки подключения загружаются из:\n" + iniPath);
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

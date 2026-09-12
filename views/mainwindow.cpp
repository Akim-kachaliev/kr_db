#include "mainwindow.h"
#include "worklistview.h"
#include "reportview.h"
#include "profiledialog.h"
#include "usersview.h"
#include "positionsview.h"
#include "qualificationsview.h"
#include "worktypesview.h"
#include "../controllers/authcontroller.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* p) : QMainWindow(p) {
    setWindowTitle("ИС РДО — Распределение дополнительных обязанностей");
    resize(1200, 750);
    setupUi();
    setupMenu();
    setupTabs();
}

void MainWindow::setupUi() {
    tabs_ = new QTabWidget(this);
    setCentralWidget(tabs_);
}

void MainWindow::setupMenu() {
    auto* fm = menuBar()->addMenu("&Файл");

    connect(fm->addAction("&Мой профиль..."), &QAction::triggered,
            this, &MainWindow::onProfile);
    fm->addSeparator();

    connect(fm->addAction("В&ыход"), &QAction::triggered,
            this, &MainWindow::onExit);

    auto* hm = menuBar()->addMenu("&Справка");
    connect(hm->addAction("&О программе"), &QAction::triggered,
            this, &MainWindow::onAbout);
}

void MainWindow::setupTabs() {
    auto* u = AuthController::getInstance()->currentUser();
    if (!u) return;

    // Вкладка «Работы и назначения» — для всех
    workView_ = new WorkListView(this);
    tabs_->addTab(workView_, "Работы и назначения");

    // Вкладка «Отчёты» — для Администратора и Руководителя
    if (u->hasRole("Администратор") || u->hasRole("Руководитель")) {
        reportView_ = new ReportView(this);
        tabs_->addTab(reportView_, "Отчёты");
    }

    // Вкладки только для Администратора
    if (u->hasRole("Администратор")) {
        usersView_ = new UsersView(this);
        tabs_->addTab(usersView_, "Пользователи");

        positionsView_ = new PositionsView(this);
        tabs_->addTab(positionsView_, "Должности");

        qualificationsView_ = new QualificationsView(this);
        tabs_->addTab(qualificationsView_, "Квалификации");

        workTypesView_ = new WorkTypesView(this);
        tabs_->addTab(workTypesView_, "Виды работ");
    }
}

void MainWindow::onExit() {
    close();
}

void MainWindow::onAbout() {
    QMessageBox::information(this, "О программе",
                             "<b>ИС распределения дополнительных обязанностей</b><br><br>"
                             "Вариант 11. Курсовой проект.<br>"
                             "Качалиев И.В., гр. 251-951<br>МИЭТ, 2026");
}

void MainWindow::onProfile() {
    ProfileDialog dlg(this);
    dlg.exec();
}

void MainWindow::closeEvent(QCloseEvent* e) {
    AuthController::getInstance()->logout();
    e->accept();
}
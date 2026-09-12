#pragma once
#include <QMainWindow>
class QTabWidget;
class WorkListView;
class ReportView;
class UsersView;
class PositionsView;
class QualificationsView;
class WorkTypesView;
class QCloseEvent;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* p = nullptr);
protected:
    void closeEvent(QCloseEvent* e) override;
private slots:
    void onExit();
    void onAbout();
    void onProfile();
private:
    void setupUi();
    void setupMenu();
    void setupTabs();
    QTabWidget* tabs_ = nullptr;
    WorkListView* workView_ = nullptr;
    ReportView* reportView_ = nullptr;
    UsersView* usersView_ = nullptr;
    PositionsView* positionsView_ = nullptr;
    QualificationsView* qualificationsView_ = nullptr;
    WorkTypesView* workTypesView_ = nullptr;
};
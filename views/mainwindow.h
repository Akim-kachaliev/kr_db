#pragma once
#include <QMainWindow>
class QTabWidget;
class WorkListView;
class ReportView;
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
private:
    void setupUi();
    void setupMenu();
    void setupTabs();
    QTabWidget* tabs_ = nullptr;
    WorkListView* workView_ = nullptr;
    ReportView* reportView_ = nullptr;
};

#pragma once
#include <QDialog>
class QLineEdit; class QPushButton; class QComboBox;

class LoginWindow : public QDialog {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget* p = nullptr);
private slots:
    void onLogin();
    void onRegister();
    void onSwitch();
private:
    void setupUi();
    void setMode(bool reg);
    QLineEdit *login_, *pwd_, *pwd2_, *name_, *email_;
    QComboBox* role_;
    QPushButton *btnLogin_, *btnReg_, *btnSwitch_;
    bool regMode_ = false;
};

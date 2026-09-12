#pragma once
#include <QDialog>
class QLineEdit;
class QPushButton;

class ProfileDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProfileDialog(QWidget* p = nullptr);
private slots:
    void onSave();
private:
    void setupUi();
    void loadCurrentUser();
    QLineEdit* nameEdit_;
    QLineEdit* emailEdit_;
    QLineEdit* pwdEdit_;
    QLineEdit* pwd2Edit_;
    QPushButton* saveBtn_;
    QPushButton* cancelBtn_;
};
#pragma once
#include <QWidget>
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDateEdit>
#include <QSpinBox>

class WorkDialog : public QDialog {
    Q_OBJECT
public:
    explicit WorkDialog(QWidget* p = nullptr);
    int workTypeId() const { return wtCombo_->currentData().toInt(); }
    QString title() const { return titleEdit_->text().trimmed(); }
    QString descr() const { return descrEdit_->toPlainText().trimmed(); }
    QDate planStart() const { return startEdit_->date(); }
    QDate planEnd() const { return endEdit_->date(); }
    int workers() const { return workersSpin_->value(); }
private:
    void setupUi();
    QComboBox* wtCombo_;
    QLineEdit* titleEdit_;
    QPlainTextEdit* descrEdit_;
    QDateEdit* startEdit_;
    QDateEdit* endEdit_;
    QSpinBox* workersSpin_;
};

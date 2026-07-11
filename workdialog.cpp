#include "workdialog.h"
#include "../models/worktype.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDate>

WorkDialog::WorkDialog(QWidget* p) : QDialog(p) {
    setWindowTitle("Создание дополнительной работы");
    setupUi();
}

void WorkDialog::setupUi() {
    auto* f = new QFormLayout(this);
    wtCombo_ = new QComboBox(this);
    titleEdit_ = new QLineEdit(this);
    descrEdit_ = new QPlainTextEdit(this);
    descrEdit_->setFixedHeight(60);
    startEdit_ = new QDateEdit(QDate::currentDate(), this);
    endEdit_ = new QDateEdit(QDate::currentDate().addDays(7), this);
    workersSpin_ = new QSpinBox(this);
    workersSpin_->setRange(1, 20);
    workersSpin_->setValue(1);
    startEdit_->setCalendarPopup(true);
    endEdit_->setCalendarPopup(true);
    f->addRow("Вид работы:", wtCombo_);
    f->addRow("Заголовок:", titleEdit_);
    f->addRow("Описание:", descrEdit_);
    f->addRow("Начало:", startEdit_);
    f->addRow("Окончание:", endEdit_);
    f->addRow("Исполнителей:", workersSpin_);
    auto* b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    f->addRow(b);
    connect(b, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(b, &QDialogButtonBox::rejected, this, &QDialog::reject);
    auto wts = WorkType::findAll();
    for (WorkType* w : wts) {
        wtCombo_->addItem(w->name(), w->id());
        delete w;
    }
}

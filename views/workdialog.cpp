#include "workdialog.h"
#include "../models/worktype.h"
#include "../models/additionalwork.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDate>
#include <QMessageBox>

WorkDialog::WorkDialog(QWidget* p) : QDialog(p) {
    setWindowTitle("Создание дополнительной работы");
    setupUi();
}

WorkDialog::WorkDialog(AdditionalWork* work, QWidget* p) : QDialog(p) {
    setWindowTitle("Редактирование работы");
    setupUi();
    if (work) {
        titleEdit_->setText(work->title());
        descrEdit_->setPlainText(work->descr());
        startEdit_->setDate(work->planStart());
        endEdit_->setDate(work->planEnd());
        workersSpin_->setValue(work->planWorkers());

        int idx = wtCombo_->findData(work->workTypeId());
        if (idx >= 0) wtCombo_->setCurrentIndex(idx);
    }
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

    connect(b, &QDialogButtonBox::accepted, this, [this]() {
        if (titleEdit_->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите заголовок работы");
            return;
        }
        if (endEdit_->date() < startEdit_->date()) {
            QMessageBox::warning(this, "Ошибка",
                                 "Дата окончания не может быть раньше даты начала");
            return;
        }
        accept();
    });
    connect(b, &QDialogButtonBox::rejected, this, &QDialog::reject);

    loadWorkTypes();
}

void WorkDialog::loadWorkTypes() {
    wtCombo_->clear();
    auto wts = WorkType::findAll();
    for (WorkType* w : wts) {
        wtCombo_->addItem(w->name(), w->id());
        delete w;
    }
}
#include "reportview.h"
#include "../controllers/reportcontroller.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>

ReportView::ReportView(QWidget* p) : QWidget(p) {
    setupUi();
    onGenerate();
}

void ReportView::setupUi() {
    auto* m = new QVBoxLayout(this);
    auto* r = new QHBoxLayout();
    genBtn_ = new QPushButton("Сформировать", this);
    expBtn_ = new QPushButton("Экспорт в CSV", this);
    r->addWidget(genBtn_);
    r->addWidget(expBtn_);
    r->addStretch();
    m->addLayout(r);
    table_ = new QTableView(this);
    model_ = new QStandardItemModel(this);
    model_->setHorizontalHeaderLabels(
        {"ID", "Сотрудник", "Должность", "Квалификация",
         "Назначений", "Сумма доплат"});
    table_->setModel(model_);
    table_->horizontalHeader()->setStretchLastSection(true);
    m->addWidget(table_);
    status_ = new QLabel(this);
    m->addWidget(status_);
    connect(genBtn_, &QPushButton::clicked, this, &ReportView::onGenerate);
    connect(expBtn_, &QPushButton::clicked, this, &ReportView::onExportCsv);
}

void ReportView::onGenerate() {
    model_->removeRows(0, model_->rowCount());
    fillBonus();
}

void ReportView::fillBonus() {
    auto rows = ReportController::getInstance()->bonusSummary();
    for (const auto& r : rows) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(r.id))
            << new QStandardItem(r.name)
            << new QStandardItem(r.position)
            << new QStandardItem(r.qual)
            << new QStandardItem(QString::number(r.count))
            << new QStandardItem(QString::number(r.total, 'f', 2));
        model_->appendRow(row);
    }
    status_->setText(QString("Записей: %1").arg(rows.size()));
}

void ReportView::onExportCsv() {
    if (model_->rowCount() == 0) {
        QMessageBox::information(this, "Экспорт", "Нет данных");
        return;
    }
    QString fn = QFileDialog::getSaveFileName(this, "Экспорт в CSV",
        QString("report_%1.csv").arg(
            QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")),
        "CSV (*.csv)");
    if (fn.isEmpty()) return;
    QFile f(fn);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out.setGenerateByteOrderMark(true);
    for (int c = 0; c < model_->columnCount(); ++c) {
        if (c > 0) out << ";";
        out << "\"" << model_->headerData(c, Qt::Horizontal).toString() << "\"";
    }
    out << "\n";
    for (int r = 0; r < model_->rowCount(); ++r) {
        for (int c = 0; c < model_->columnCount(); ++c) {
            if (c > 0) out << ";";
            out << "\"" << model_->item(r, c)->text() << "\"";
        }
        out << "\n";
    }
    f.close();
    QMessageBox::information(this, "Экспорт", "Файл сохранён: " + fn);
}

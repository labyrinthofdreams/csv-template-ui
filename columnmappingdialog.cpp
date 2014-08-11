#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <QChar>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTableWidgetItem>
#include "columnmappingdialog.hpp"
#include "sfcsv/sfcsv.h"
#include "ui_columnmappingdialog.h"

static const auto validChars = [](QChar c){ return (c >= 'a' && c <= 'z') ||
                                                   (c >= 'A' && c <= 'Z') || c == '_'; };

extern QStringList parseCsv(const QString& in) {
    QStringList result;
    std::vector<std::string> parsed;
    const std::string line = in.toStdString();
    sfcsv::parse_line(line, std::back_inserter(parsed), ',');
    for(auto&& res : parsed) {
        result.append(QString::fromStdString(res));
    }

    return result;
}

static QString getValidField(const QString& text) {
    QString validText;
    std::copy_if(text.cbegin(), text.cend(), std::back_inserter(validText),
                 std::cref(validChars));
    return validText.toLower();
}

ColumnMappingDialog::ColumnMappingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnMappingDialog),
    config("config.ini", QSettings::IniFormat),
    source(),
    mapped()
{
    ui->setupUi(this);

    // Read first row from CSV file
    QString firstRow;
    QFile inFile(config.value("last_opened").toString());
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Unable to open file");
    }

    firstRow = inFile.readLine();
    firstRow.remove('\n');

    const bool hasHeader = config.value("has_header", false).toBool();
    ui->checkBoxHeader->setChecked(hasHeader);

    source = parseCsv(firstRow);

    ui->tableWidget->setRowCount(source.size());
    for(int i = 0, size = source.size(); i < size; ++i) {
        const QString rawText = source.at(i);
        const QString validText = getValidField(rawText);

        auto itemSrc = new QTableWidgetItem(rawText);
        auto itemMap = new QTableWidgetItem(validText);

        ui->tableWidget->setItem(i, 0, itemSrc);
        ui->tableWidget->setItem(i, 1, itemMap);

        mapped << validText;
    }

    config.setValue("mapped_keys", mapped);
}

ColumnMappingDialog::~ColumnMappingDialog()
{
    delete ui;
}

void ColumnMappingDialog::on_tableWidget_cellChanged(int row, int column)
{
    if(column == 0) {
        // User can't change the source fields
        ui->tableWidget->item(row, column)->setText(source.at(row));
    }
    else if(column == 1) {
        // Only certain characters are allowed
        const QString text = ui->tableWidget->item(row, column)->text();
        const bool valid = std::all_of(text.cbegin(), text.cend(), std::cref(validChars));
        if(!valid) {
            // Remove all bad characters
            const QString validText = getValidField(text);
            ui->tableWidget->item(row, column)->setText(validText);

            mapped.replace(row, validText);
            config.setValue("mapped_keys", mapped);
        }
    }
}

void ColumnMappingDialog::on_checkBoxHeader_clicked(bool checked)
{
    config.setValue("has_header", checked);
}

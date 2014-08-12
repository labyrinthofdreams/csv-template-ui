#include <exception>
#include <QAction>
#include <QClipboard>
#include <QCursor>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMap>
#include <QMapIterator>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QTableWidgetItem>
#include <QTableWidgetSelectionRange>
#include <QTextStream>
#include "columnmappingdialog.hpp"
#include "mainwindow.hpp"
#include "sfcsv/sfcsv.h"
#include "ui_mainwindow.h"

static QStringList parseCsv(const QString& in) {
    QStringList result;
    std::vector<std::string> parsed;
    const std::string line = in.toStdString();
    sfcsv::parse_line(line, std::back_inserter(parsed), ',');
    for(auto&& res : parsed) {
        result.append(QString::fromStdString(res));
    }

    return result;
}

QStringList parseFile(const QString& in) {
    QStringList results;
    QFile inFile(in);
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Unable to open file");
    }

    QTextStream stream(&inFile);
    while(!stream.atEnd()) {
        const QString line = stream.readLine().remove("\n");
        results << line;
    }

    return results;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    config("config.ini", QSettings::IniFormat)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;

    config.remove("has_header");
    config.remove("source_keys");
    config.remove("mapped_keys");
}

void MainWindow::on_actionOpen_triggered()
{
    const QString openedFile =
            QFileDialog::getOpenFileName(this, tr("Open..."),
                                         config.value("last_opened").toString());
    if(openedFile.isEmpty()) {
        return;
    }

    config.setValue("last_opened", openedFile);

    // Read first row from CSV file
    QString firstRow;
    QFile inFile(config.value("last_opened").toString());
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Unable to open file");
    }

    firstRow = inFile.readLine();
    firstRow.remove('\n');

    const QStringList source = parseCsv(firstRow);
    config.setValue("source_keys", source);

    try {
        ColumnMappingDialog dlg;
        const auto ok = dlg.exec();
        if(!ok) {
            return;
        }

        // Get all rows
        QStringList lines = parseFile(openedFile);
        if(lines.isEmpty()) {
            QMessageBox::critical(this, tr("Empty CSV"),
                                  tr("CSV file does not contain any data"));
            return;
        }

        // Set header
        ui->tableCsv->clear();
        ui->tableCsv->setRowCount(lines.size());
        ui->tableCsv->setColumnCount(lines.size());
        const bool hasHeader = config.value("has_header", false).toBool();
        if(hasHeader) {
            const QStringList labels = parseCsv(lines.at(0));
            ui->tableCsv->setColumnCount(labels.size());
            ui->tableCsv->setHorizontalHeaderLabels(labels);
            lines.removeAt(0);
            if(lines.isEmpty()) {
                QMessageBox::critical(this, tr("Empty CSV"),
                                      tr("CSV file contains only a header"));
                return;
            }
        }

        // Display data
        for(int i = 0, size = lines.size(); i < size; ++i) {
            const QStringList parsed = parseCsv(lines.at(i));
            for(int j = 0, jsize = parsed.size(); j < jsize; ++j) {
                auto item = new QTableWidgetItem(parsed.at(j));
                ui->tableCsv->setItem(i, j, item);
            }
        }

    }
    catch(std::exception &ex) {
        QMessageBox::critical(this, tr("Error"), QString(ex.what()));
    }
}

void MainWindow::on_tableCsv_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);

    QMenu menu;
    auto cbAction = new QAction(tr("Copy to clipboard"), this);
    cbAction->setData("clipboard");
    menu.addAction(cbAction);
    auto selected = menu.exec(QCursor::pos());
    if(selected && selected->data().toString() == "clipboard") {
        const auto ranges = ui->tableCsv->selectedRanges();
        QString parsedTpl;
        for(const auto& range : ranges) {
            const QStringList list = config.value("mapped_keys").toStringList();
            QMap<QString, QString> mappings;
            for(int rIdx = range.topRow(), rEnd = range.bottomRow(); rIdx <= rEnd; ++rIdx) {
                for(int i = 0, end = ui->tableCsv->columnCount(); i != end; ++i) {
                    mappings.insert(list.at(i), ui->tableCsv->item(rIdx, i)->text());
                }

                QString tpl = ui->templateEdit->toPlainText();
                QMapIterator<QString, QString> iter(mappings);
                while(iter.hasNext()) {
                    iter.next();
                    const QString var = tr("$%1").arg(iter.key());
                    tpl.replace(var, iter.value());
                }

                parsedTpl.append(tpl);
            }
        }

        auto clipboard = QApplication::clipboard();
        clipboard->setText(parsedTpl);
    }
}

void MainWindow::on_actionMappings_triggered()
{
    ColumnMappingDialog dlg;
    dlg.exec();
}

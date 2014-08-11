#include <exception>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTableWidgetItem>
#include <QTextStream>
#include "columnmappingdialog.hpp"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

extern QStringList parseCsv(const QString& in);

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

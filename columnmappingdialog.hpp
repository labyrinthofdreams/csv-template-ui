#ifndef COLUMNMAPPINGDIALOG_HPP
#define COLUMNMAPPINGDIALOG_HPP

#include <QDialog>
#include <QSettings>
#include <QStringList>

namespace Ui {
class ColumnMappingDialog;
}

class ColumnMappingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnMappingDialog(QWidget *parent = 0);
    ~ColumnMappingDialog();

private slots:
    void on_tableWidget_cellChanged(int row, int column);

    void on_checkBoxHeader_clicked(bool checked);

    void on_buttonBox_accepted();

private:
    Ui::ColumnMappingDialog *ui;

    QSettings config;
    QStringList source;
    QStringList mapped;
};

#endif // COLUMNMAPPINGDIALOG_HPP

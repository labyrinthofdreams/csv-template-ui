#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QSettings>

class QString;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_tableCsv_customContextMenuRequested(const QPoint &pos);

    void on_actionMappings_triggered();

private:
    Ui::MainWindow *ui;

    QSettings config;
};

#endif // MAINWINDOW_HPP

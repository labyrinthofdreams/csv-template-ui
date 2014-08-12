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

    void on_actionMappings_triggered();

    void on_actionOpen_template_triggered();

    void on_actionSave_triggered();

    void on_templateEdit_textChanged();

private:
    Ui::MainWindow *ui;

    QSettings config;
};

#endif // MAINWINDOW_HPP

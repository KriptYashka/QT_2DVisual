#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <fstream>
#include <QShortcut>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QStandardItemModel *csvModel;

private slots:
    void on_btn_load_clicked();
    void on_btn_loadfile_clicked();
    void on_btn_metric_clicked();
    void closeApp();

private:
    Ui::MainWindow *ui;
    QShortcut *keyEnter;
    QShortcut *keyEsc;

};
#endif // MAINWINDOW_H

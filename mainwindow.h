#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <fstream>
#include <QShortcut>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
using namespace std;
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
    void draw_picture(vector<int> years, vector<double> metric, QString metric_name, double min, double max, double med);

private:
    Ui::MainWindow *ui;
    QShortcut *keyEnter;
    QShortcut *keyEsc;

};
#endif // MAINWINDOW_H

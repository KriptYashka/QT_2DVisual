#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "technical.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_loadfile_clicked(){
    QString file_path = QFileDialog::getOpenFileName(this, tr("Open file"));
    read_csv_file(file_path.toStdString());
}

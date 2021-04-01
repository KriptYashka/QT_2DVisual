#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "technical.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    csvModel = new QStandardItemModel(this);
    csvModel->setColumnCount(7);

    keyEnter = new QShortcut(this);
    keyEnter->setKey(Qt::Key_Enter);
    connect(keyEnter, SIGNAL(activated()), this, SLOT(on_btn_metric_clicked()));

    keyEsc = new QShortcut(this);
    keyEsc->setKey(Qt::Key_Escape);
    connect(keyEsc, SIGNAL(activated()), this, SLOT(closeApp()));
}

MainWindow::~MainWindow(){
    delete ui;
}

/* Глобальные переменные */
QStandardItemModel *general_model = new QStandardItemModel;
QStringList headers;

void MainWindow::closeApp(){
    QApplication::exit();
}

QList<QStandardItem *> get_row(QStandardItemModel* model, int row){
    /* Возвращает QList из ячеек таблицы */
    QList<QStandardItem *> res;
    for (int i = 0; i < model->columnCount(); ++i){
        res.append(model->item(row, i));
    }
    return res;
}

void model_cpy(QStandardItemModel* from, QStandardItemModel* to){
    /* Копирует данные из одной модели в другую */
    to->clear();
    for (int row = 0 ; row < from->rowCount() ; row++){
        QList<QStandardItem *> res;
        for (int i = 0; i < from->columnCount(); ++i){
            res.append(new QStandardItem(from->item(row, i)->text()));
        }
       to->appendRow(res);
    }
}

void MainWindow::on_btn_loadfile_clicked(){
    /* Загрузка файла */
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open file"));
    vector<vector<string>> csv_read = read_csv_file(filePath.toStdString());

    if (csv_read.size() == 0){
        ui->label_title->setText("Нет таблицы");
        ui->label_result->setText("Невозможно открыть файл");
        ui->line_region->setText("");
        ui->line_col->setText("");
    } else {
        // Устанавливаем CSV модель

        csvModel->clear();
        csvModel->setColumnCount(csv_read.at(0).size());
        // Устанавливаем заголовки
        headers.clear();
        for (string str : csv_read.at(0)) {
          headers.push_back(QString::fromStdString(str));
        }
        csvModel->setHorizontalHeaderLabels(headers);

        bool is_header = true;
        for (vector<string> item_list : csv_read) {
            if (is_header){ // Пропускаем заголовки
                is_header = false;
                continue;
            }
            QList<QStandardItem *> standardItemsList;
            for (string item_str : item_list){
                QString q_item_str = QString::fromStdString(item_str);
                standardItemsList.append(new QStandardItem(q_item_str));
            }
            csvModel->insertRow(csvModel->rowCount(), standardItemsList);
        }
        model_cpy(csvModel, general_model);
        ui->label_title->setText(filePath);
    }
}


void MainWindow::on_btn_load_clicked(){
    ui->table_metric->setModel(csvModel);
}

int check_column(QString col){
    /* Проверяет колонку на целочисленный формат */
    bool flag = true;
    int res = col.toInt(&flag);
    if (flag)
        return res;
    return -1;
}

void MainWindow::on_btn_metric_clicked(){
    /* Поиск нужный метрик и их вычисление */
    QString region = ui->line_region->text();
    QString column = ui->line_col->text();
    int col_metric = check_column(column);
    if (col_metric == -1 || col_metric > csvModel->columnCount() || col_metric < 1){
        ui->label_result->setText("Переданы некорректные значение колонки.\nКолонка должна быть в диапозоне от 1 до 7.");
        return;
    }
    col_metric--;

    double minimum = 0;
    double maximum = 0;
    double average = 0;

    general_model->clear();
    general_model->setColumnCount(7);
    general_model->setHorizontalHeaderLabels(headers);

    // Выборка нужных записей
    for (int row = 0; row < csvModel->rowCount(); ++row){
        if ((csvModel->item(row, 1)->text() == region || region == "")){
            QList<QStandardItem *> res;
            for (int i = 0; i < csvModel->columnCount(); ++i){
                res.append(new QStandardItem(csvModel->item(row, i)->text()));
            }
            general_model->insertRow(general_model->rowCount(), res);
        }
    }

    ui->table_metric->setModel(general_model);

    std::vector<double> arr;
    // Просчет метрик
    for (int row = 0; row < general_model->rowCount(); ++row){
        QString str = general_model->item(row, col_metric)->text();
        if (is_normal_metric(str.toStdString())){
            arr.push_back(general_model->item(row, col_metric)->text().toDouble());
        }
    }

    calc_metric(arr, col_metric, &minimum, &maximum, &average);
    QString result_text = "Минимум: "+ QString::number(minimum) +"\nМаксимум: "+ QString::number(maximum)
            +"\nМедиана: "+ QString::number(average);
    if (arr.size() == 0){
        result_text = "Нет результатов. Проверьте название региона или выбранную колонку.";
    }
    ui->label_result->setText(result_text);
}

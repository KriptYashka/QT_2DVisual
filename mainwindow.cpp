#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "technical.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    QIcon winIcon("favicon.ico");
    this->setWindowIcon(winIcon);
    this->setWindowTitle("KriptYashka - 2D Visual");

    csv_model = new QStandardItemModel(this);
    csv_model->setColumnCount(7);

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
vector<vector<string>> csv_read;

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
    if (is_csv_file(filePath.toStdString())){
        csv_read = read_csv_file(filePath.toStdString());
    }

    if (csv_read.size() == 0 || !is_csv_file(filePath.toStdString())){
        ui->label_title->setText("Нет таблицы");
        ui->label_result->setText("Невозможно открыть файл");
        ui->line_region->setText("");
        ui->line_col->setText("");
    } else {
        // Устанавливаем CSV модель

        csv_model->clear();
        csv_model->setColumnCount(csv_read.at(0).size());
        // Устанавливаем заголовки
        headers.clear();
        for (string str : csv_read.at(0)) {
          headers.push_back(QString::fromStdString(str));
        }
        csv_model->setHorizontalHeaderLabels(headers);

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
            csv_model->insertRow(csv_model->rowCount(), standardItemsList);
        }
        model_cpy(csv_model, general_model);
        ui->label_title->setText(filePath);
    }
}


void MainWindow::on_btn_load_clicked(){
    ui->table_metric->setModel(csv_model);
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
    if (col_metric == -1 || col_metric > csv_model->columnCount() || col_metric < 1){
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
    for (int row = 0; row < csv_model->rowCount(); ++row){
        if ((csv_model->item(row, 1)->text() == region || region == "")){
            QList<QStandardItem *> res;
            for (int i = 0; i < csv_model->columnCount(); ++i){
                res.append(new QStandardItem(csv_model->item(row, i)->text()));
            }
            general_model->insertRow(general_model->rowCount(), res);
        }
    }

    ui->table_metric->setModel(general_model);

    vector<double> arr;
    vector<int> years;
    // Просчет метрик
    for (int row = 0; row < general_model->rowCount(); ++row){
        QString str = general_model->item(row, col_metric)->text();
        QString str_year = general_model->item(row, 0)->text();
        if (is_normal_metric(str.toStdString()) && is_normal_metric(str_year.toStdString())){
            arr.push_back(general_model->item(row, col_metric)->text().toDouble());
            years.push_back(general_model->item(row, 0)->text().toInt());
        }
    }

    calc_metric(arr, col_metric, &minimum, &maximum, &average);
    QString result_text = "Минимум: "+ QString::number(minimum) +"\nМаксимум: "+ QString::number(maximum)
            +"\nМедиана: "+ QString::number(average);
    if (arr.size() == 0){
        result_text = "Нет результатов. Проверьте название региона или выбранную колонку.";
         ui->label_result->setText(result_text);
         ui->graphic->clear();
         return;
    }
    ui->label_result->setText(result_text);

    QString col_name = headers.at(col_metric);
    draw_picture(years, arr, col_name, minimum, maximum, average);

}
 /* Глобальные переменные для отрисовки */


void draw_dot(vector<QPen> pens, QPainter &painter, double met, int year, double max, double min, vector<double> positions){
    double posX = positions.at(0),
           posY = positions.at(1),
           horYPos = positions.at(2),
           verXPos = positions.at(3),
           sizeY = positions.at(4),
           padding = positions.at(5);
    painter.setPen(pens.at(0));
    painter.drawLine(posX + 1, horYPos + 2, posX + 1, horYPos - 2);

    painter.rotate(-90); //Поворачивает систему координат по часовой стрелке
    painter.drawText(-(sizeY - padding/2), posX + 5, QString::number(year, 'g', 6)); // текст под осью икс
    painter.rotate(90);

    /* Для особых значений */
    if (met == min || met == max)
    {
        painter.drawLine(verXPos - 2, posY, verXPos + 2, posY);
        painter.drawText(0, posY - 1, QString::number(met, 'g', 4));
        painter.setPen(pens.at(1));
        painter.drawLine(verXPos, posY, posX, posY);
    }

    double color_max = max - min;
    double delta = met - min;
    int colorR = (delta / color_max) * 255;

    pens.at(2).setColor(QColor(colorR, 30, 200));

    painter.setPen(pens.at(2));
    painter.drawEllipse(posX, posY, 3, 3); // точки
}

void MainWindow::draw_picture(vector<int> years, vector<double> metric, QString metric_name, double min, double max, double med){
    /* Отрисовывает график на основе вектора years и metric */
    double min_year = years.at(0), max_year = years.at(years.size()-1);

    QPainter painter;

    QPen dot_pen = painter.pen();
    QPen default_pen = painter.pen();
    QPen horizontal_pen = painter.pen();
    dot_pen.setWidth(4);
    dot_pen.setColor(QColor(125, 30, 255));

    default_pen.setWidth(2);

    horizontal_pen.setStyle(Qt::DotLine);
    vector<QPen> pens = {default_pen, horizontal_pen, dot_pen};

    const int sizeX = 800,
        sizeY = 480,
        padding = 80,
        graphicYOffset = 20,
        graphicXOffset = 10;

    QPicture picture;
    picture.setBoundingRect(QRect(QPoint(0, 0), QPoint(sizeX, sizeY)));



    painter.begin(&picture);

    painter.setPen(default_pen);

    const int horYPos = sizeY - padding - graphicYOffset / 2;
    const int verXPos = padding + graphicXOffset;

    painter.drawLine(verXPos, horYPos, sizeX - padding, horYPos); // Ox
    painter.drawLine(sizeX - padding, horYPos, sizeX - padding - 5, horYPos - 3);
    painter.drawLine(sizeX - padding, horYPos, sizeX - padding - 5, horYPos + 3);

    painter.drawLine(verXPos, horYPos, verXPos, padding); // Oy
    painter.drawLine(verXPos, padding, verXPos - 3, padding + 5); // стрелочки
    painter.drawLine(verXPos, padding, verXPos + 3, padding + 5);

    int size = years.size();
    double diffX = max_year - min_year,
    diffY = max - min;

    double posX, posY;
    for(int i = 0; i < size; i++)
    {
        posX = verXPos + 5 + ((years[i] - min_year) / diffX) * (sizeX - 3 * padding - verXPos);
        posY = horYPos - padding - ((metric[i] - min) / diffY) * (horYPos - 4 * padding) * 1.5;

        vector<double> positions = {posX, posY, horYPos, verXPos, sizeY, padding};
        draw_dot(pens, painter, metric[i], years[i], max, min, positions);
    }

    posX = verXPos + 5 + ((max_year - min_year) / diffX) * (sizeX - 3 * padding - verXPos);
    posY = horYPos - padding - ((med - min) / diffY) * (horYPos - 4 * padding) / 2;

    painter.setPen(default_pen);
    painter.drawLine(verXPos - 2, posY, verXPos + 2, posY);
    painter.drawText(0, posY - 1, QString::number(med, 'g', 4));

    painter.setPen(horizontal_pen);
    painter.drawLine(verXPos, posY, posX, posY);
    painter.drawText(0, 50, metric_name);

    painter.end();

    ui->graphic->setPicture(picture);

}

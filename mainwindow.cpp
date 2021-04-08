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

QPicture MainWindow::get_picture(double min, double max, double med){
    vector<int> year;
    double minY = min, maxX = max;

           const int sizeX = 500,
                   sizeY = 480,
                   padding = 50,
                   graphicYOffset = 30,
                   graphicXOffset = 10;

           QPicture picture;
           picture.setBoundingRect(QRect(QPoint(0, 0), QPoint(sizeX, sizeY))); //Устанавливает ограничивающий прямоугольник изображения на r (QRect &r): 1)QRect(int x, int y, int width, int height) 2)QRect(const QPoint &topLeft, const QSize &size) 3)QRect(const QPoint &topLeft, const QPoint &bottomRight)
           QPainter painter;

           painter.begin(&picture); // paint in picture

           QPen dotsPen = painter.pen(); //The QPen class defines how a QPainter should draw lines and outlines of shapes.
           dotsPen.setWidth(2); //толщина ручки
           dotsPen.setColor(QColor(255, 0, 255)); //color

           QPen defPen = painter.pen(); //ручка для осей
           defPen.setWidth(2);

           QPen horPen = painter.pen();//ручка для горизонтальных линий метрики
           horPen.setStyle(Qt::DotLine);

           painter.setPen(defPen);

           const int horYPos = sizeY - padding - graphicYOffset / 2;
           const int verXPos = padding + graphicXOffset;

           painter.drawLine(verXPos, horYPos, sizeX - padding, horYPos); // horizontal line
           painter.drawLine(sizeX - padding, horYPos, sizeX - padding - 5, horYPos - 3); // стрелочки
           painter.drawLine(sizeX - padding, horYPos, sizeX - padding - 5, horYPos + 3);

           painter.drawLine(verXPos, horYPos, verXPos, padding); // vertical line
           painter.drawLine(verXPos, padding, verXPos - 3, padding + 5); // стрелочки
           painter.drawLine(verXPos, padding, verXPos + 3, padding + 5);

           int size = year.size();
           double diffX = maxY - minX, // года
                   diffY = maxX - minY;

           double posX, posY;
           for(int i = 0; i < size; i++)
           {
               posX = verXPos + 5 + ((year[i] - minX) / diffX) * (sizeX - 3 * padding - verXPos);
               posY = horYPos - padding - ((OY[i] - minY) / diffY) * (horYPos - 4 * padding) / 2;

               painter.setPen(defPen);
               painter.drawLine(posX + 1, horYPos + 2, posX + 1, horYPos - 2); // segments on horizontal line

               painter.rotate(-90); //Поворачивает систему координат по часовой стрелке
               painter.drawText(-(sizeY - padding/2), posX + 5, QString::number(year[i], 'g', 6)); // текст под осью икс
               painter.rotate(90);

               if(OY[i] == min || OY[i] == max)
               {
                   painter.drawLine(verXPos - 2, posY, verXPos + 2, posY);
                   painter.drawText(0, posY - 1, QString::number(OY[i], 'g', 4));
                   painter.setPen(horPen);
                   painter.drawLine(verXPos, posY, posX, posY);
               }
               painter.setPen(dotsPen);
               painter.drawEllipse(posX, posY, 2, 2); // точки
           }

           posX = verXPos + 5 + ((maxY - minX) / diffX) * (sizeX - 3 * padding - verXPos);
           posY = horYPos - padding - ((med - minY) / diffY) * (horYPos - 4 * padding) / 2;

           painter.drawLine(verXPos - 2, posY, verXPos + 2, posY);
           painter.drawText(0, posY - 1, QString::number(med, 'g', 4));
           painter.setPen(horPen);
           painter.drawLine(verXPos, posY, posX, posY);
           painter.drawText(0, 50, Oy);

           painter.end();

           //ui->Grafic->setPicture(picture);
}

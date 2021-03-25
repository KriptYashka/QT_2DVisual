#ifndef TECHNICAL_H
#define TECHNICAL_H

#include <stdio.h>
#include <vector>
#include <string>

#include "mainwindow.h" // удалить, как только найду библиотеку под sort
using namespace std;

void calc_metric(vector<double> arr, int col_metric, double* min, double* max, double* avg);
vector<vector<double>> read_csv_file(string path);



#endif // TECHNICAL_H

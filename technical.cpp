#include "technical.h"
using namespace std;

void calc_metric(vector<double> arr, int col_metric, double* min, double* max, double* avg){
    sort(arr.begin(), arr.end());
    *min = 0;
    *max = 0;
    *avg = 0;
    if (arr.size() != 0){
        *min = arr[0];
        *max = arr[arr.size() - 1];
        if (arr.size() % 2 == 0){
            *avg = (arr[arr.size() / 2] + arr[arr.size() / 2 - 1]) / 2.0;
        } else {
            *avg = arr[arr.size() / 2];
        }
    }
}

bool is_csv_file(string path){
    int index = path.find(".csv");
    if (index == -1)
        return false;
    return true;
}

vector<vector<double>> read_csv_file(string path){
    /* Считывает данные из файла и записывает в модель
    Возвращает код ошибки*/
    FILE *f = fopen(path.c_str(), "r");
    vector<double> row;
    string *row_str = new string();
    fscanf(f, "%s", row_str);
    printf("%s", row_str);

    fclose(f);
}

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

bool is_digit(char c){
    if (c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9' || c == '0')
        return true;
    return false;
}

bool is_normal_metric(string text){
    /* Проверяет метрику на числовой формат */
    if (text == "")
        return false;
    for (int i = 0; i < text.length(); ++i){
        if (!is_digit(text[i]) && (text[i] != '-' && text[i] != '.')){
            return false;
        }
    }
    return true;
}

vector<string> split_line(string line){
    vector<string> result;
    string word = "";
    for (int i = 0; i < line.length(); ++i){
        char symbol = line[i];
        if ((symbol == ',') || symbol == '\n'){
            result.push_back(word);
            word = "";
            continue;
        }
        word += symbol;
    }
    result.push_back(word);
    return result;
}

vector<vector<string>> read_csv_file(string path){
    /* Считывает данные из файла и записывает в модель
    Возвращает код ошибки*/

    vector<vector<string>> result;
    string line;
    ifstream myFile(path);
    if (!myFile.is_open()) throw runtime_error("Could not open file");

    //getline(myFile, line);

    while(getline(myFile, line)){
        vector<string> line_model = split_line(line);
        result.push_back(line_model);
    }
    return result;
}

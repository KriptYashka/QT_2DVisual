#include "technical.h"
using namespace std;

vector<string> split_line(string line){
    vector<string> result;
    string word = "";
    for (int i = 0; i < (int)line.length(); ++i){
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

bool is_csv_file(string path){
    int index = path.find(".csv");
    if (index == -1)
        return false;
    else
        return true;
}

Response* execute(Request* request){
    Response* response = new Response;

    switch (request->operation){
        case Operations::READ:

            if (!is_csv_file(request->path)){
                response->done = false;
            } else {
                ifstream myFile(request->path);
                string line;
                while(getline(myFile, line)){
                    vector<string> line_model = split_line(line);
                    response->csv.push_back(line_model);
                }
            }
            break;

        case Operations::METRIC:
            sort(request->arr.begin(), request->arr.end());
            response->min = 0;
            response->max = 0;
            response->avg = 0;
            if (request->arr.size() != 0){
                response->min = request->arr[0];
                response->max = request->arr[request->arr.size() - 1];
                if (request->arr.size() % 2 == 0){
                    response->avg = (request->arr[request->arr.size() / 2] + request->arr[request->arr.size() / 2 - 1]) / 2.0;
                } else {
                    response->avg = request->arr[request->arr.size() / 2];
                }
            }
            break;

        case Operations::IS_METRIC:
            /* Проверяет метрику на числовой формат */
            if (request->text == ""){
                response->flag = false;
                break;
            }
            for (int i = 0; i < (int)request->text.length(); ++i){
                if (!isdigit(request->text[i]) && (request->text[i] != '-' && request->text[i] != '.')){
                    response->flag = false;
                    break;
                }
            }
            response->flag = true;
    }
    return response;
}

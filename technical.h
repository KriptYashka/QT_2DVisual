#ifndef TECHNICAL_H
#define TECHNICAL_H
using namespace std;
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

enum Operations{
    METRIC,
    READ,
    IS_METRIC,
};

struct Request{
    int operation;
    string path;
    vector<double> arr;
    string text;
};

struct Response{
    bool done = true;
    string message;
    vector<vector<string>> csv;
    double min, max, avg;
    bool flag = false;
};

Response* execute(Request* request);

#endif // TECHNICAL_H

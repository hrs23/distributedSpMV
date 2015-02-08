
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
using namespace std;
int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <log file>\n", argv[0]);
        exit(1);
    }
    ifstream ifs(argv[1]);
    string line;
    string matrix;
    string performance;
    map<string, string> data;
    while (getline(ifs, line)) {
        if (line == "----------------------------------------") {
            cout << data["Matrix"] << " " << data["GFLOPS"] << " " << data["NumberOfRows"] << endl;
            continue;
        }
        stringstream ss(line);
        string key, val;
        ss >> key >> val;
        data[key] = val;
    }
}

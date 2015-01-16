
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <log file>\n", argv[0]);
    }
    ifstream ifs(argv[1]);
    string line;
    string matrix;
    string performance;
    while (getline(ifs, line)) {
        if (line.find(".mtx") != string::npos) {
            matrix = line;
        }
        if (line.find("GFLOPS") != string::npos) {
            performance = line;
            cout << matrix.substr(0, matrix.size()-1) + " " << performance.substr(performance.find("GFLOPS")+6) << endl;
        }
    }
}

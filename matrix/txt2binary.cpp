#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;
int main (int argc, char ** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./translate src dst" << std::endl;
        std::exit(1);
    }
    ifstream ifs( argv[1] );
    ofstream ofs( argv[2], ios::out|ios::binary );
    std::string line;
    do {
        getline(ifs, line);
    } while (line[0] == '#');
    int nrow, ncol, nnonzero;
    stringstream ss(line);
    ss >> nrow >> ncol >> nnonzero;
    ofs.write((char*)&nrow, sizeof(int));
    ofs.write((char*)&ncol, sizeof(int));
    ofs.write((char*)&nnonzero, sizeof(int));
    for (int i = 0; i < nnonzero; i++) {
        int row, col;
        double val;
        ifs >> row >> col >> val;
        ofs.write((char*)&row, sizeof(int));
        ofs.write((char*)&col, sizeof(int));
        ofs.write((char*)&val, sizeof(double));
    }
    ifs.close();
    ofs.close();
    
    return 0;
}

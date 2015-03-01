#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <vector>
#include "util.h"
using namespace std;
int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <partition stat file>\n", argv[0]);
        exit(1);
    }
    ifstream ifs(argv[1]);
    string line;
    string key;
    int val;
    printf("%s\t", GetBasename(argv[1]).c_str());
    for (int i = 0; i < 5; i++) {
        if (i) ifs.ignore();
        getline(ifs, line); 
        ifs >> key >> val; assert(key == "max");
        printf("%d\t", val);
        ifs >> key >> val; assert(key == "min");
        printf("%d\t", val);
        ifs >> key >> val; assert(key == "ave");
        printf("%d\t", val);
    }
    puts("");
}

#include "util.h"
using namespace std;
string GetBasename (const string &path) {
    size_t p = path.rfind("/");
    return path.substr(p+1);
}


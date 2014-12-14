#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "patoh.h"

using namespace std;
int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <hypergraph file> <number of parts>\n", argv[0]);
        exit(1);
    }
    return 0;
}

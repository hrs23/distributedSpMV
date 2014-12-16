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
    int nPart = atoi(argv[2]);
    int nCell, nNet, nConst;
    int *weights, *costs, *xpins, *pins;

    PaToH_Parameters params;
    PaToH_Read_Hypergraph(argv[1], &nCell, &nNet, &nConst, &weights, &costs, &xpins, &pins);
    printf("Hypergraph %10s -- #Cells=%d #Nets=%d #Pins=%d #Const=%d\n", argv[1], nCell, nNet, xpins[nNet], nConst);
    PaToH_Initialize_Parameters(&params, PATOH_CONPART, PATOH_SUGPARAM_DEFAULT);
    params._k = nPart;
    int cut;
    int *partvec = new int[nCell];
    int *partweights = new int[params._k * nConst];
    PaToH_Alloc(&params, nCell, nNet, nConst, weights, costs, xpins, pins);
    PaToH_Part(&params, nCell, nNet, nConst, 0, weights, costs, xpins, pins, NULL, partvec, partweights, &cut);
    printf("%d-way cutsize is: %d\n", params._k, cut);
    
    delete [] partvec;
    delete [] partweights;
    PaToH_Free();
    return 0;
}

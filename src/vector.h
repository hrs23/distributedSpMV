#pragma once
struct Vector {
    int localLength;
    int externalLength;
    double *values;

    // internal values
    double *denseInternalValues;
};

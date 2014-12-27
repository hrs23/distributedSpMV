#pragma once
struct Vector {
    int localLength;
    int externalLength;
    double *values;
    void *optimizationData;
};

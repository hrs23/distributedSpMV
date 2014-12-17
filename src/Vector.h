#pragma once
struct Vector {
    int localLength;
    int externalLength;
    double *value;
    void *optimizationData;
};

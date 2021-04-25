#ifndef ALGORITHMOPTIONS_H
#define ALGORITHMOPTIONS_H

#include "gl_const.h"

class AlgorithmOptions {
public:
    int algorithm;
    AlgorithmOptions(int AL): algorithm(AL) {}
    virtual ~AlgorithmOptions() {}
};

class BOAstarAlgorithmOptions : public AlgorithmOptions {
public:
    int metrictype;     //Can be chosen Euclidean, Manhattan, Chebyshev and Diagonal distance
    double  hweight;
    BOAstarAlgorithmOptions(int AL = CN_SP_ST_ASTAR, int MT = CN_SP_MT_EUCL, double HW = 1.0): 
        AlgorithmOptions{AL}, 
        metrictype{MT}, 
        hweight{HW} {}
    ~BOAstarAlgorithmOptions() override {}
};

class GeneticAlgorithmOptions : public AlgorithmOptions {
public:
    int epoch_number;
    int parents_remain;
    int child_create;
    GeneticAlgorithmOptions(int EN = 50, int PR = 25, int CC = 25): 
        AlgorithmOptions{CN_SP_ST_GAMOPP}, 
        epoch_number{EN}, 
        parents_remain{PR}, 
        child_create{CC} {}
    ~GeneticAlgorithmOptions() override {}
};

#endif
#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "cell.h"
#include <cmath>

class Heuristic {
protected:
    double w;
public:
    Heuristic(double w): w(w) {}
    virtual double metric(Cell) const = 0;
    double operator()(Cell c) const {
        return metric(c) * w;
    }
};

class ZeroHeuristic : public Heuristic {
public:
    ZeroHeuristic(): Heuristic(1.) {}
    double metric(Cell) const override {
        return 0;
    }
};

class EuclidianHeuristic : public Heuristic {
    Cell goal;
public:
    EuclidianHeuristic(Cell goal, double w): Heuristic(w), goal{goal} {}
    double metric(Cell c) const override {
        c -= goal;
        return sqrt(c.first * c.first + c.second * c.second);
    }
};

class ChebishevHeuristic : public Heuristic {
    Cell goal;
public:
    ChebishevHeuristic(Cell goal, double w): Heuristic(w), goal{goal} {}
    double metric(Cell c) const override {
        c -= goal;
        return std::max(std::abs(c.first), std::abs(c.second));
    }
};

class OctileHeuristic : public Heuristic {
    Cell goal;
public:
    OctileHeuristic(Cell goal, double w): Heuristic(w), goal{goal} {}
    double metric(Cell c) const override {
        c -= goal;
        c.first = std::abs(c.first);
        c.second = std::abs(c.second);
        return (std::min(c.first, c.second) * sqrt(2.) + std::abs(c.first - c.second));
    }
};

class ManhattanEuristic : public Heuristic {
    Cell goal;
public:
    ManhattanEuristic(Cell goal, double w): Heuristic(w), goal{goal} {}
    double metric(Cell c) const override {
        c -= goal;
        return (std::abs(c.first) + std::abs(c.second));
    }
};

#endif
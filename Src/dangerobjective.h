#ifndef DANGEROBJECTIVE_H
#define DANGEROBJECTIVE_H

#include <algorithm>
#include <cmath>

class DangerObjective {
public:
    virtual double operator()(int) const = 0;
    virtual ~DangerObjective() {};
};

class LinearDangerObjective : public DangerObjective {
    int max_danger;
public:
    LinearDangerObjective(int max_danger): max_danger(max_danger) {}
    double operator()(int x) const override {
        return std::max(max_danger - x + 1, 0);
    }
};

class InvertDangerObjective : public DangerObjective {
public:
    InvertDangerObjective() {}
    double operator()(int x) const override {
        return 1.0 / x;
    }
};

class ExponentialDangerObjective : public DangerObjective {
public:
    ExponentialDangerObjective() {}
    double operator()(int x) const override {
        return std::exp(-static_cast<double>(x));
    }
};

class ZeroDangerObjective : public DangerObjective {
public:
    ZeroDangerObjective() {}
    double operator()(int) const override {
        return 0;
    }
};

#endif
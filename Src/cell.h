#ifndef CELL_H
#define CELL_H

#include <utility>

using Cell = std::pair<int, int>;

inline Cell operator+(Cell c1, Cell c2) {
    return {c1.first + c2.first, c1.second + c2.second};
}

inline Cell operator-(Cell c1, Cell c2) {
    return {c1.first - c2.first, c1.second + c2.second};
}

inline Cell operator*(Cell c, int x) {
    return {c.first * x, c.second * x};
}

inline Cell operator*(int x, Cell c) {
    return {x * c.first, x * c.second};
}

inline Cell& operator+=(Cell& c1, Cell c2) {
    c1 = c1 + c2;
    return c1;
}

inline Cell& operator-=(Cell& c1, Cell c2) {
    c1 = c1 - c2;
    return c1;
}

inline Cell& operator*=(Cell& c, int x) {
    c = c * x;
    return c;
}

#endif
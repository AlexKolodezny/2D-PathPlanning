#ifndef SECTION_H
#define SECTION_H

#include <list>
#include "cell.h"

struct Section : public Cell {
    double length;
    Section(Cell c, double length): Cell{c}, length{length} {}
};

std::list<Section> make_secondary_path(const std::list<Cell>& path);

#endif


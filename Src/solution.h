#ifndef SOLUTION_H
#define SOLUTION_H

#include "cell.h"
#include "section.h"
#include <list>

struct Solution {
   std::list<Cell> lppath;
   std::list<Section> hppath;
   double length;
   double danger;
};

#endif
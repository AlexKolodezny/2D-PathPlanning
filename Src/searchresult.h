#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H
#include <list>
#include <vector>
#include "solution.h"

//That's the output structure for the search algorithms.

struct SearchResult
{
        std::vector<Solution> paths;
        unsigned int nodescreated; //|OPEN| + |CLOSE| = total number of nodes saved in memory during search process.
        unsigned int numberofsteps; //number of iterations made by algorithm to find a solution
        double time; //runtime of the search algorithm (expanding nodes + reconstructing the path)
        SearchResult()
            : paths{}
            , nodescreated{0}
            , numberofsteps{0}
            , time{0} {}

};

#endif

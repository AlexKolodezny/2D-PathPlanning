
#ifndef ASTAR_H
#define ASTAR_H

#include "searchresult.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include <list>
#include "map.h"
#include "search.h"
#include "dangerobjective.h"

class AstarSearch : public Search
{
    Cell start, goal;
    int min_danger;
    public:
        AstarSearch(Cell start, Cell goal, int min_danger);
        ~AstarSearch() override;
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) override;
};

#endif
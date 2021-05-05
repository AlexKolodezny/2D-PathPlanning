
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
    public:
        AstarSearch();
        ~AstarSearch() override;
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) override;
};

#endif
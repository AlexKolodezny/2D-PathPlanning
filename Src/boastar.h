#ifndef BOASTAR_H
#define BOASTAR_H

#include "searchresult.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include <list>
#include "map.h"
#include "search.h"

class BOAstarSearch : public Search
{
    public:
        BOAstarSearch();
        ~BOAstarSearch() override;
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) override;
};

#endif
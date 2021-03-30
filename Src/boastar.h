#ifndef BOASTAR_H
#define BOASTAR_H

#include "searchresult.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include <list>
#include "map.h"
#include "search.h"
#include "dangerobjective.h"

class BOAstarSearch : public Search
{
    std::unique_ptr<DangerObjective> obj;
    public:
        BOAstarSearch(std::unique_ptr<DangerObjective>&& obj);
        ~BOAstarSearch() override;
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) override;
};

#endif
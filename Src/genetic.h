#ifndef GENETIC_H
#define GENETIC_H

#include "search.h"
#include "map.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include "ilogger.h"

class GeneticAlgorithm : public Search
{
    public:
        GeneticAlgorithm();
        ~GeneticAlgorithm() override;
        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) override;
};

#endif
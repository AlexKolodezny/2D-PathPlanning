#include "genetic.h"
#include "searchresult.h"

GeneticAlgorithm::GeneticAlgorithm() {}

GeneticAlgorithm::~GeneticAlgorithm() {}

SearchResult GeneticAlgorithm::startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) {
    sresult.nodescreated = 0;
    sresult.numberofsteps = 0;
    sresult.pathfound = false;
    sresult.time = 0;
    return sresult;
}
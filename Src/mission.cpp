#include "mission.h"
#include "boastar.h"
#include "genetic.h"
#include "astar.h"
#include <iostream>
#include <memory>

Mission::Mission()
{
    logger = nullptr;
    fileName = nullptr;
}

Mission::Mission(const char *FileName)
{
    fileName = FileName;
    logger = nullptr;
}

Mission::~Mission()
{
    if (logger)
        delete logger;
}

bool Mission::getMap()
{
    return map.setMap(fileName);
}

bool Mission::getConfig()
{
    return config.getConfig(fileName);
}

bool Mission::createLog()
{
    if (logger != nullptr) delete logger;
    logger = new XmlLogger(config.LogParams[CN_LP_LEVEL]);
    return logger->getLog(fileName, config.LogParams);
}

bool Mission::createEnvironmentOptions()
{
    return options.setEnvironmentOptions(fileName);
}

void Mission::createSearch()
{
    map.setDistanceMap();
    std::unique_ptr<DangerObjective> obj;
    if (options.dangerobjective == CN_SP_DO_LINEAR) {
        obj = std::make_unique<LinearDangerObjective>(options.dangerlevel);
    } else if (options.dangerobjective == CN_SP_DO_INVERT) {
        obj = std::make_unique<InvertDangerObjective>();
    } else {
        obj = std::make_unique<ExponentialDangerObjective>();
    }
    if (options.algorithm_options->algorithm == CN_SP_ST_GAMOPP) {
        search = std::make_unique<GeneticAlgorithm>(map, options, std::move(obj));
    } else if (options.algorithm_options->algorithm == CN_SP_ST_BOASTAR) {
        search = std::make_unique<BOAstarSearch>(std::move(obj));
    } else {
        search = std::make_unique<AstarSearch>();
    }
}

void Mission::startSearch()
{
    if (!search) {
        std::cerr << "Error! Search has not been created!" << std::endl;
        return;
    }
    sr = search->startSearch(logger, map, options);
}

void Mission::printSearchResultsToConsole()
{
    std::cout << "Path ";
    if (sr.paths.empty())
        std::cout << "NOT ";
    std::cout << "found!" << std::endl;
    std::cout << "numberofsteps=" << sr.numberofsteps << std::endl;
    std::cout << "nodescreated=" << sr.nodescreated << std::endl;
    if (!sr.paths.empty()) {
        std::cout << "numberofpaths=" << sr.paths.size() << std::endl;
    }
    std::cout << "time=" << sr.time << std::endl;
}

void Mission::saveSearchResultsToLog()
{
    logger->writeToLogSummary(sr.paths.size(), sr.numberofsteps, sr.nodescreated, sr.time);
    if (!sr.paths.empty()) {
        logger->writeToLogPaths(map, sr.paths);
    } else
        logger->writeToLogNotFound();
    logger->saveLog();
}

SearchResult Mission::getSearchResult()
{
    return sr;
}


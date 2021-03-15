#include "mission.h"
#include "boastar.h"
#include "genetic.h"
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
    map.setDistanceMap(options.dangerlevel);
    if (options.algorithm == CN_SP_ST_GAMOPP) {
        search = std::unique_ptr<Search>(new GeneticAlgorithm());
    } else {
        search = std::unique_ptr<Search>(new BOAstarSearch());
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
    if (!sr.pathfound)
        std::cout << "NOT ";
    std::cout << "found!" << std::endl;
    std::cout << "numberofsteps=" << sr.numberofsteps << std::endl;
    std::cout << "nodescreated=" << sr.nodescreated << std::endl;
    if (sr.pathfound) {
        std::cout << "numberofpaths=" << sr.pathlength.size() << std::endl;
    }
    std::cout << "time=" << sr.time << std::endl;
}

void Mission::saveSearchResultsToLog()
{
    logger->writeToLogSummary(sr.numberofsteps, sr.nodescreated, sr.time);
    if (sr.pathfound) {
        logger->writeToLogPaths(map, sr.lppaths, sr.hppaths);
    } else
        logger->writeToLogNotFound();
    logger->saveLog();
}

SearchResult Mission::getSearchResult()
{
    return sr;
}


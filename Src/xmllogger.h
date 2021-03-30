#ifndef XMLLOGGER_H
#define	XMLLOGGER_H
#include "tinyxml2.h"
#include "ilogger.h"
#include "cell.h"
#include "section.h"
#include "solution.h"


//That's the class that flushes the data to the output XML


class XmlLogger : public ILogger {

public:
    XmlLogger(std::string loglevel):ILogger(loglevel){}

    virtual ~XmlLogger() {};

    bool getLog(const char *FileName, const std::string *LogParams);

    void saveLog();

    //void writeToLogOpenClose(const typename &open, const typename &close);
    void writeToLogNotFound();

    void writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, double time);

    void writeToLogPaths(const Map& map, const std::vector<Solution>& sol);
    void writeToLogGeneration(const Map& map, const std::vector<Solution>&);
private:
    void writeToLogMap(tinyxml2::XMLElement *node, const Map &Map, const std::list<Cell> &path);
    void writeToLogPath(tinyxml2::XMLElement *node, const std::list<Cell> &path);
    void writeToLogHPath(tinyxml2::XMLElement *node, const std::list<Section> &hppath);
    void writeToLogPathSummary(tinyxml2::XMLElement *node, float length, float danger, float cell_size);

    std::string LogFileName;
    tinyxml2::XMLDocument doc;
};

#endif


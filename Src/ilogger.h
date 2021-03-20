#ifndef ILOGGER_H
#define	ILOGGER_H
#include "map.h"
#include <unordered_map>
#include <list>
#include "solution.h"
#include <vector>

class ILogger
{
    public:
        ILogger(std::string loglevel) {this->loglevel = loglevel;}
        virtual bool getLog(const char* FileName, const std::string* LogParams) = 0;
        virtual void saveLog() = 0;
        //virtual void writeToLogOpenClose(const typename &open, const typename &close) = 0;
        virtual void writeToLogPaths(const Map&, const std::vector<Solution>&) = 0;
        virtual void writeToLogNotFound() = 0;
        virtual void writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, double time) = 0;
        virtual ~ILogger() {};
    protected:
        std::string loglevel;
};

#endif


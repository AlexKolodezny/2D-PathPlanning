#ifndef SEARCH_H
#define SEARCH_H
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"

class Search
{
    public:
        virtual ~Search() {}
        virtual SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) = 0;

    protected:

        SearchResult                    sresult; //This will store the search result
};
#endif

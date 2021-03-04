#ifndef CONFIG_H
#define	CONFIG_H
#include <string>

class Config
{
    public:
        Config();
        Config(const Config& orig) = delete;
        Config(Config&&) = delete;
        Config& operator=(const Config&) = delete;
        Config& operator=(Config&&) = delete;
        ~Config();
        bool getConfig(const char *FileName);

    public:
        std::string*    LogParams;
        unsigned int    N;

};

#endif


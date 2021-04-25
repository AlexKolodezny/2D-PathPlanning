#ifndef ENVIRONMENTOPTIONS_H
#define ENVIRONMENTOPTIONS_H
#include "gl_const.h"
#include "algorithmoptions.h"
#include <memory>

class EnvironmentOptions
{
public:
    EnvironmentOptions(bool AD, bool CC, bool AS, int DO = CN_SP_DO_EXP, int DL = 0);
    EnvironmentOptions();
    bool    allowdiagonal;  //Option that allows to make diagonal moves
    bool    cutcorners;     //Option that allows to make diagonal moves, when one adjacent cell is untraversable
    bool    allowsqueeze;   //Option that allows to move throught "bottleneck"
    int     dangerobjective;
    int     dangerlevel;
    std::unique_ptr<AlgorithmOptions> algorithm_options;

    bool setEnvironmentOptions(const char* FileName);
};

#endif // ENVIRONMENTOPTIONS_H

#include "environmentoptions.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "tinyxml2.h"

EnvironmentOptions::EnvironmentOptions():
    allowdiagonal{false},
    cutcorners{false},
    allowsqueeze{false},
    dangerobjective{CN_SP_DO_EXP},
    dangerlevel{0},
    hppath{true},
    algorithm_options{std::make_unique<BOAstarAlgorithmOptions>()} {}

EnvironmentOptions::EnvironmentOptions(bool AD, bool CC, bool AS, int DO,int DL, bool HP):
    allowdiagonal{AD},
    cutcorners{CC},
    allowsqueeze{AS},
    dangerobjective{DO},
    dangerlevel{DL},
    hppath{HP},
    algorithm_options{} {}

bool EnvironmentOptions::setEnvironmentOptions(const char *FileName)
{
    std::string value;
    std::stringstream stream;
    tinyxml2::XMLElement *root = nullptr, *algorithm = nullptr, *element = nullptr;

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }

    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' element found in XML file!" << std::endl;
        return false;
    }

    algorithm = root->FirstChildElement(CNS_TAG_ALG);
    if (!algorithm) {
        std::cout << "Error! No '" << CNS_TAG_ALG << "' tag found in XML file!" << std::endl;
        return false;
    }

    element = algorithm->FirstChildElement(CNS_TAG_ST);
    if (!element) {
        std::cout << "Error! No '" << CNS_TAG_ST << "' tag found in XML file!" << std::endl;
        return false;
    }
    if (element->GetText())
        value = element->GetText();
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);

    if (value == CNS_SP_ST_DIJK) {
        algorithm_options = std::make_unique<BOAstarAlgorithmOptions>(CN_SP_ST_DIJK);
    } else if (value == CNS_SP_ST_ASTAR) {
        algorithm_options = std::make_unique<BOAstarAlgorithmOptions>(CN_SP_ST_ASTAR);
    } else if (value == CNS_SP_ST_BOASTAR) {
        algorithm_options = std::make_unique<BOAstarAlgorithmOptions>(CN_SP_ST_BOASTAR);
    } else if (value == CNS_SP_ST_GAMOPP) {
        algorithm_options = std::make_unique<GeneticAlgorithmOptions>();
    } else {
        std::cout << "Error! Value of '" << CNS_TAG_ST << "' tag (algorithm name) is not correctly specified."
                  << std::endl;
        std::cout << "Supported algorithm's names are: '" << CNS_SP_ST_DIJK << "', '"
                  << CNS_SP_ST_ASTAR << "', " << CNS_SP_ST_BOASTAR << "', '" CNS_SP_ST_GAMOPP << "'." << std::endl;
        return false;
    }
    if (algorithm_options->algorithm == CN_SP_ST_ASTAR || algorithm_options->algorithm == CN_SP_ST_BOASTAR) {
        BOAstarAlgorithmOptions *boastar_algorithm_options = dynamic_cast<BOAstarAlgorithmOptions*>(algorithm_options.get());
        element = algorithm->FirstChildElement(CNS_TAG_MT);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_MT << "' tag found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'." << std::endl;
            boastar_algorithm_options->metrictype = CN_SP_MT_EUCL;
        }
        else {
            if (element->GetText())
                value = element->GetText();
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if (value == CNS_SP_MT_MANH) boastar_algorithm_options->metrictype = CN_SP_MT_MANH;
            else if (value == CNS_SP_MT_EUCL) boastar_algorithm_options->metrictype = CN_SP_MT_EUCL;
            else if (value == CNS_SP_MT_DIAG) boastar_algorithm_options->metrictype = CN_SP_MT_DIAG;
            else if (value == CNS_SP_MT_CHEB) boastar_algorithm_options->metrictype = CN_SP_MT_CHEB;
            else {
                std::cout << "Warning! Value of'" << CNS_TAG_MT << "' is not correctly specified. There is not tag '" << value << "'." << std::endl;
                std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'" << std::endl;
                boastar_algorithm_options->metrictype = CN_SP_MT_EUCL;
            }
        }
        element = algorithm->FirstChildElement(CNS_TAG_HW);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_HW << "' tag found in algorithm section." << std::endl;
            std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1." << std::endl;
            boastar_algorithm_options->hweight = 1;
        }
        else {
            stream << element->GetText();
            stream >> boastar_algorithm_options->hweight;
            stream.str("");
            stream.clear();

            if (boastar_algorithm_options->hweight < 1) {
                std::cout << "Warning! Value of '" << CNS_TAG_HW << "' tag is not correctly specified. Should be >= 1."
                        << std::endl;
                std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1." << std::endl;
                boastar_algorithm_options->hweight = 1;
            }
        }

    }
    if (algorithm_options->algorithm == CN_SP_ST_GAMOPP || algorithm_options->algorithm == CN_SP_ST_BOASTAR) {
        element = algorithm->FirstChildElement(CNS_TAG_DO);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_DO << "' tag found in algorithm section." << std::endl;
            std::cout << "Value of '" << CNS_TAG_DO << "' was defined to 'exp'" << std::endl;
            dangerobjective = CN_SP_DO_EXP;
        } else {
            if (element->GetText())
                value = element->GetText();
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if (value == CNS_SP_DO_INVERT) dangerobjective = CN_SP_DO_INVERT;
            else if (value == CNS_SP_DO_LINEAR) dangerobjective = CN_SP_DO_LINEAR;
            else if (value == CNS_SP_DO_EXP) dangerobjective = CN_SP_DO_EXP;
            else {
                std::cout << "Warning! Value of'" << CNS_TAG_DO << "' is not correctly specified. There is not tag '" << value << "'." << std::endl;
                std::cout << "Value of '" << CNS_TAG_DO << "' was defined to 'invert'" << std::endl;
                dangerobjective = CN_SP_DO_INVERT;
            }
        }
        if (dangerobjective == CN_SP_DO_LINEAR) {
            element = algorithm->FirstChildElement(CNS_TAG_DL);
            if (!element) {
                std::cout << "Warning! No '" << CNS_TAG_DL << "' tag found in algorithm section." << std::endl;
                std::cout << "Value of '" << CNS_TAG_DL << "' was defined to 0" << std::endl;
                dangerlevel = 0;
            } else {
                stream << element->GetText();
                stream >> dangerlevel;
                stream.str("");
                stream.clear();

                if (dangerlevel < 0) {
                    std::cout << "Warning! Value of '" << CNS_TAG_DL << "' tag is not correctly specified. Should be >= 0."
                            << std::endl;
                    std::cout << "Value of '" << CNS_TAG_DL << "' was defined to 0." << std::endl;
                    dangerlevel = 0;
                }
            }
        }

    }
    if (algorithm_options->algorithm == CN_SP_ST_GAMOPP) {
        GeneticAlgorithmOptions *genetic_algorithm_options = dynamic_cast<GeneticAlgorithmOptions*>(algorithm_options.get());
        element = algorithm->FirstChildElement(CNS_TAG_GT_EN);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_GT_EN << "' tag found in algorithm section." << std::endl;
            std::cout << "Value of '" << CNS_TAG_GT_EN << "' was defined to 50" << std::endl;
            genetic_algorithm_options->epoch_number = 50;
        } else {
            stream << element->GetText();
            stream >> genetic_algorithm_options->epoch_number;
            stream.str("");
            stream.clear();

            if (genetic_algorithm_options->epoch_number < 0) {
                std::cout << "Warning! Value of '" << CNS_TAG_GT_EN << "' tag is not correctly specified. Should be >= 0."
                        << std::endl;
                std::cout << "Value of '" << CNS_TAG_GT_EN << "' was defined to 50." << std::endl;
                genetic_algorithm_options->epoch_number = 50;
            }
        }
        
        element = algorithm->FirstChildElement(CNS_TAG_GT_PR);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_GT_PR << "' tag found in algorithm section." << std::endl;
            std::cout << "Value of '" << CNS_TAG_GT_PR << "' was defined to 25" << std::endl;
            genetic_algorithm_options->parents_remain = 25;
        } else {
            stream << element->GetText();
            stream >> genetic_algorithm_options->parents_remain;
            stream.str("");
            stream.clear();

            if (genetic_algorithm_options->parents_remain < 0) {
                std::cout << "Warning! Value of '" << CNS_TAG_GT_PR << "' tag is not correctly specified. Should be >= 0."
                        << std::endl;
                std::cout << "Value of '" << CNS_TAG_GT_PR << "' was defined to 25." << std::endl;
                genetic_algorithm_options->parents_remain = 25;
            }
        }

        element = algorithm->FirstChildElement(CNS_TAG_GT_CC);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_GT_CC << "' tag found in algorithm section." << std::endl;
            std::cout << "Value of '" << CNS_TAG_GT_CC << "' was defined to 25" << std::endl;
            genetic_algorithm_options->child_create = 25;
        } else {
            stream << element->GetText();
            stream >> genetic_algorithm_options->child_create;
            stream.str("");
            stream.clear();

            if (genetic_algorithm_options->child_create < 0) {
                std::cout << "Warning! Value of '" << CNS_TAG_GT_CC << "' tag is not correctly specified. Should be >= 0."
                        << std::endl;
                std::cout << "Value of '" << CNS_TAG_GT_CC << "' was defined to 25." << std::endl;
                genetic_algorithm_options->child_create = 25;
            }
        }
    }
    


    element = algorithm->FirstChildElement(CNS_TAG_AD);
    if (!element) {
        std::cout << "Warning! No '" << CNS_TAG_AD << "' element found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_AD << "' was defined to default - true" << std::endl;
        allowdiagonal = 1;
    }
    else {
        std::string check;
        stream << element->GetText();
        stream >> check;
        stream.clear();
        stream.str("");

        if (check != "1" && check != "true" && check != "0" && check != "false") {
            std::cout << "Warning! Value of '" << CNS_TAG_AD << "' is not correctly specified." << std::endl;
            std::cout << "Value of '" << CNS_TAG_AD << "' was defined to default - true " << std::endl;
            allowdiagonal = 1;
        }
        else if (check == "1" || check == "true")
            allowdiagonal = 1;
        else
            allowdiagonal = 0;
    }

    if (allowdiagonal == 0) {
        cutcorners = 0;
        allowsqueeze = 0;
    }
    else {
        element = algorithm->FirstChildElement(CNS_TAG_CC);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_CC << "' element found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_CC << "' was defined to default - false" << std::endl;
            cutcorners = 0;
        }
        else {
            std::string check;
            stream << element->GetText();
            stream >> check;
            stream.clear();
            stream.str("");
            if (check != "1" && check != "true" && check != "0" && check != "false") {
                std::cout << "Warning! Value of '" << CNS_TAG_CC << "' is not correctly specified." << std::endl;
                std::cout << "Value of '" << CNS_TAG_CC << "' was defined to default - false" << std::endl;
                cutcorners = 0;
            }
            else {
                if (check == "1" || check == "true")
                    cutcorners = 1;
                else
                    cutcorners = 0;
            }
        }
        if (cutcorners == 0) {
            allowsqueeze = 0;
        }
        else {
            element = algorithm->FirstChildElement(CNS_TAG_AS);
            if (!element) {
                std::cout << "Warning! No '" << CNS_TAG_AS << "' element found in XML file." << std::endl;
                std::cout << "Value of '" << CNS_TAG_AS << "' was defined to default - false." << std::endl;
                allowsqueeze = 0;
            }
            else {
                std::string check;
                stream << element->GetText();
                stream >> check;
                stream.clear();
                stream.str("");
                if (check != "1" && check != "true" && check != "0" && check != "false") {
                    std::cout << "Warning! Value of '" << CNS_TAG_AS << "' is not correctly specified." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_AS << "' was defined to default - false." << std::endl;
                    allowsqueeze = 0;
                }
                else {
                    if (check == "1" || check == "true")
                        allowsqueeze = 1;
                    else
                        allowsqueeze = 0;
                }
            }
        }
    }
    return true;
}


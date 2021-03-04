#include "environmentoptions.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "tinyxml2.h"

EnvironmentOptions::EnvironmentOptions()
{
    algorithm = CN_SP_ST_ASTAR;
    hweight = 1.;
    metrictype = CN_SP_MT_EUCL;
    allowsqueeze = false;
    allowdiagonal = true;
    cutcorners = false;
    dangerlevel = 0;
}

EnvironmentOptions::EnvironmentOptions(bool AS, bool AD, bool CC, int MT, int AL, double HW, int DL)
{
    algorithm = AL;
    hweight = HW;
    metrictype = MT;
    allowsqueeze = AS;
    allowdiagonal = AD;
    cutcorners = CC;
    dangerlevel = DL;
}

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
        this->algorithm = CN_SP_ST_DIJK;
    }
    else if (value == CNS_SP_ST_ASTAR) {
        this->algorithm = CN_SP_ST_ASTAR;
        element = algorithm->FirstChildElement(CNS_TAG_HW);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_HW << "' tag found in algorithm section." << std::endl;
            std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1." << std::endl;
            hweight = 1;
        }
        else {
            stream << element->GetText();
            stream >> hweight;
            stream.str("");
            stream.clear();

            if (hweight < 1) {
                std::cout << "Warning! Value of '" << CNS_TAG_HW << "' tag is not correctly specified. Should be >= 1."
                          << std::endl;
                std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1." << std::endl;
                hweight = 1;
            }
        }

        element = algorithm->FirstChildElement(CNS_TAG_MT);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_MT << "' tag found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'." << std::endl;
            metrictype = CN_SP_MT_EUCL;
        }
        else {
            if (element->GetText())
                value = element->GetText();
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if (value == CNS_SP_MT_MANH) metrictype = CN_SP_MT_MANH;
            else if (value == CNS_SP_MT_EUCL) metrictype = CN_SP_MT_EUCL;
            else if (value == CNS_SP_MT_DIAG) metrictype = CN_SP_MT_DIAG;
            else if (value == CNS_SP_MT_CHEB) metrictype = CN_SP_MT_CHEB;
            else {
                std::cout << "Warning! Value of'" << CNS_TAG_MT << "' is not correctly specified." << std::endl;
                std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'" << std::endl;
                metrictype = CN_SP_MT_EUCL;
            }
        }

    }
    else {
        std::cout << "Error! Value of '" << CNS_TAG_ST << "' tag (algorithm name) is not correctly specified."
                  << std::endl;
        std::cout << "Supported algorithm's names are: '" << CNS_SP_ST_DIJK << "', '"
                  << CNS_SP_ST_ASTAR << "'." << std::endl;
        return false;
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


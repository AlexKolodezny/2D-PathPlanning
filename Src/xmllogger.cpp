#include "xmllogger.h"
#include <iostream>

using tinyxml2::XMLElement;
using tinyxml2::XMLNode;

bool XmlLogger::getLog(const char *FileName, const std::string *LogParams)
{
    if (loglevel == CN_LP_LEVEL_NOPE_WORD) return true;

    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening input XML file" << std::endl;
        return false;
    }

    if (LogParams[CN_LP_PATH] == "" && LogParams[CN_LP_NAME] == "") {
        std::string str;
        str.append(FileName);
        size_t found = str.find_last_of(".");
        if (found != std::string::npos)
            str.insert(found, "_log");
        else
            str.append("_log");
        LogFileName.append(str);
    } else if (LogParams[CN_LP_PATH] == "") {
        LogFileName.append(FileName);
        std::string::iterator it = LogFileName.end();
        while (*it != '\\')
            it--;
        ++it;
        LogFileName.erase(it, LogFileName.end());
        LogFileName.append(LogParams[CN_LP_NAME]);
    } else if (LogParams[CN_LP_NAME] == "") {
        LogFileName.append(LogParams[CN_LP_PATH]);
        if (*(--LogParams[CN_LP_PATH].end()) != '\\') LogFileName.append("\\");
        std::string lfn;
        lfn.append(FileName);
        size_t found = lfn.find_last_of("\\");
        std::string str = lfn.substr(found);
        found = str.find_last_of(".");
        if (found != std::string::npos)
            str.insert(found, "_log");
        else
            str.append("_log");
        LogFileName.append(str);
    } else {
        LogFileName.append(LogParams[CN_LP_PATH]);
        if (*(--LogParams[CN_LP_PATH].end()) != '\\') LogFileName.append("\\");
        LogFileName.append(LogParams[CN_LP_NAME]);
    }

    XMLElement *log, *root = doc.FirstChildElement(CNS_TAG_ROOT);

    if (!root) {
        std::cout << "No '" << CNS_TAG_ROOT << "' element found in XML file" << std::endl;
        std::cout << "Can not create log" << std::endl;
        return false;
    }

    root->InsertEndChild(doc.NewElement(CNS_TAG_LOG));

    root = (root->LastChild())->ToElement();


    if (loglevel != CN_LP_LEVEL_NOPE_WORD) {
        log = doc.NewElement(CNS_TAG_MAPFN);
        log->LinkEndChild(doc.NewText(FileName));
        root->InsertEndChild(log);

        root->InsertEndChild(doc.NewElement(CNS_TAG_SUM));

        root->InsertEndChild(doc.NewElement(CNS_TAG_PATHS));
    }

    if (loglevel == CN_LP_LEVEL_FULL_WORD || loglevel == CN_LP_LEVEL_MEDIUM_WORD)
        root->InsertEndChild(doc.NewElement(CNS_TAG_LOWLEVEL));

    return true;
}

void XmlLogger::saveLog()
{
    if (loglevel == CN_LP_LEVEL_NOPE_WORD)
        return;
    doc.SaveFile(LogFileName.c_str());
}

void XmlLogger::writeToLogMap(XMLElement *node, const Map &map, const std::list<Cell> &path)
{
    if (loglevel == CN_LP_LEVEL_NOPE_WORD ||
        loglevel == CN_LP_LEVEL_TINY_WORD ||
        loglevel == CN_LP_LEVEL_SHORT_WORD)
        return;

    XMLElement *mapTag = node->InsertEndChild(doc.NewElement(CNS_TAG_PATH))->ToElement();

    int iterate = 0;
    bool inPath;
    std::string str;
    for (int i = 0; i < map.getMapHeight(); ++i) {
        XMLElement *element = doc.NewElement(CNS_TAG_ROW);
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);

        for (int j = 0; j < map.getMapWidth(); ++j) {
            inPath = false;
            for(std::list<Cell>::const_iterator it = path.begin(); it != path.end(); it++)
                if(it->first == i && it->second == j) {
                    inPath = true;
                    break;
                }
            if (!inPath)
                str += std::to_string(map.getValue({i, j}));
            else
                str += CNS_OTHER_PATHSELECTION;
            str += CNS_OTHER_MATRIXSEPARATOR;
        }

        element->InsertEndChild(doc.NewText(str.c_str()));
        mapTag->InsertEndChild(element);
        str.clear();
        iterate++;
    }
}

/*void XmlLogger::writeToLogOpenClose(const typename &open, const typename &close)
{
    //need to implement
    if (loglevel != CN_LP_LEVEL_FULL_WORD  && !(loglevel == CN_LP_LEVEL_MEDIUM_WORD && last))
        return;


}*/

void XmlLogger::writeToLogPath(tinyxml2::XMLElement *node, const std::list<Cell> &path)
{
    if (loglevel == CN_LP_LEVEL_NOPE_WORD || loglevel == CN_LP_LEVEL_TINY_WORD || path.empty())
        return;
    int iterate = 0;
    XMLElement *lplevel = node->InsertEndChild(doc.NewElement(CNS_TAG_LPLEVEL))->ToElement();

    for (std::list<Cell>::const_iterator it = path.begin(); it != path.end(); it++) {
        XMLElement *element = doc.NewElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, it->second);
        element->SetAttribute(CNS_TAG_ATTR_Y, it->first);
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);
        lplevel->InsertEndChild(element);
        iterate++;
    }
}

void XmlLogger::writeToLogHPath(tinyxml2::XMLElement *node, const std::list<Section> &hppath)
{
    if (loglevel == CN_LP_LEVEL_NOPE_WORD || 
        loglevel == CN_LP_LEVEL_TINY_WORD ||
        hppath.empty())
        return;
    int partnumber = 0;
    XMLElement *hplevel = node->InsertEndChild(doc.NewElement(CNS_TAG_HPLEVEL))->ToElement();
    std::list<Section>::const_iterator iter = hppath.begin();
    std::list<Section>::const_iterator it = hppath.begin();

    while (iter != --hppath.end()) {
        XMLElement *part = doc.NewElement(CNS_TAG_SECTION);
        part->SetAttribute(CNS_TAG_ATTR_NUM, partnumber);
        part->SetAttribute(CNS_TAG_ATTR_STX, it->second);
        part->SetAttribute(CNS_TAG_ATTR_STY, it->first);
        ++iter;
        part->SetAttribute(CNS_TAG_ATTR_FINX, iter->second);
        part->SetAttribute(CNS_TAG_ATTR_FINY, iter->first);
        part->SetAttribute(CNS_TAG_ATTR_LENGTH, iter->length);
        hplevel->LinkEndChild(part);
        ++it;
        ++partnumber;
    }
}

void XmlLogger::writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, double time)
{
    if (loglevel == CN_LP_LEVEL_NOPE_WORD)
        return;

    XMLElement *summary = doc.FirstChildElement(CNS_TAG_ROOT);
    summary = summary->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_SUM);
    XMLElement *element = summary->ToElement();
    element->SetAttribute(CNS_TAG_ATTR_NUMOFSTEPS, numberofsteps);
    element->SetAttribute(CNS_TAG_ATTR_NODESCREATED, nodescreated);
    element->SetAttribute(CNS_TAG_ATTR_TIME, std::to_string(time).c_str());
}

void XmlLogger::writeToLogNotFound()
{
    if (loglevel == CN_LP_LEVEL_NOPE_WORD)
        return;

    XMLElement *node = doc.FirstChildElement(CNS_TAG_ROOT)->LastChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_PATHS);
    node->InsertEndChild(doc.NewText("Path NOT found!"));
}

void XmlLogger::writeToLogPaths(const Map& map, const std::vector<Solution>& sol) {

    XMLElement *paths_tag = doc.FirstChildElement(CNS_TAG_ROOT)->LastChildElement(CNS_TAG_LOG)->LastChildElement(CNS_TAG_PATHS);

    for (auto& s : sol) {
        XMLElement *node = paths_tag->InsertEndChild(doc.NewElement(CNS_TAG_ITEM))->ToElement();
        writeToLogPathSummary(node, s.length, s.danger, map.getCellSize());
        writeToLogPath(node, s.lppath);
        writeToLogMap(node, map, s.lppath);
        writeToLogHPath(node, s.hppath);
    }
    return;

}

void XmlLogger::writeToLogPathSummary(XMLElement *node, float length, float danger, float cell_size) {
    if (loglevel == CN_LP_LEVEL_NOPE_WORD) {
        return;
    }
    node = node->InsertEndChild(doc.NewElement(CNS_TAG_PATH_SUM))->ToElement();
    node->SetAttribute(CNS_TAG_ATTR_LENGTH, length);
    node->SetAttribute(CNS_TAG_ATTR_LENGTH_SCALED, length * cell_size);
    node->SetAttribute(CNS_TAG_ATTR_DANGER, danger);
    return;
}
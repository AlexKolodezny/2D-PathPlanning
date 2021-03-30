#ifndef MAP_H
#define	MAP_H
#include <iostream>
#include "gl_const.h"
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include "tinyxml2.h"
#include "cell.h"
#include "dangerobjective.h"

//That's the class that stores BOTH grid map data AND start-goal locations.
//getValue reads the input XML and fills the Map object.
//Some naive checks are already included in getValue but still on some corrupted XMLs it may fail,
//so be sure to invoke it passing the correct XML (e.g. with correctly filled map tag, e.g. <map>.

//Map is NOT to be modified during the search. It should be passed as a const pointer.
//Think of it as an "independent" piece of data that is managed by outer (non-search related) proccesses.
//Search algorithm should create it own object/structures needed to run the search on that map.

class Map
{
    private:
        int     height, width;
        int     start_i, start_j;
        int     goal_i, goal_j;
        double  cellSize;
        int**   Grid;
        int**   distance_map;

    public:
        Map();
        Map(const Map& orig);
        ~Map();

        bool setMap(const char *FileName);
        bool setDistanceMap();
        bool CellIsTraversable (Cell c) const;
        bool CellOnGrid (Cell c) const;
        bool CellIsObstacle(Cell c) const;
        int getCellDanger(Cell c) const;
        int  getValue(Cell c) const;
        int getMapHeight() const;
        int getMapWidth() const;
        double getCellSize() const;
        Cell getStartNode() const;
        Cell getGoalNode() const;

};

class HashCoordinate {
    int map_width;
public:
    HashCoordinate(int width): map_width(width) {}

    int operator()(Cell coor) const {
        return std::hash<int>()(coor.first * map_width + coor.second);
    }
};

#endif


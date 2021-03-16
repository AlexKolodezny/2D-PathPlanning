#include "genetic.h"
#include "searchresult.h"
#include "map.h"
#include <list>

struct GeneticNode {
    std::pair<int, int> cell;
    int d;
};

//random_path generates path from start to end not included
//resulting legnth and danger return in the poiner arguments
//the end cell does not included in danger
std::list<GeneticNode> random_path(const Map& Map, std::pair<int, int> start, std::pair<int, int> end, double *length, double *danger);

class Individ {
    std::list<GeneticNode> path;
    const Map& grid;
    double length;
    double danger;
public:
    Individ();
    Individ(const Map& grid, size_t k): path{}, grid{grid} {
        std::pair<int, int> start = grid.getStartNode();
        for (size_t i = 0; i < k; ++i) {
            std::pair<int, int> next{rand() % grid.getMapWidth(), rand() % grid.getMapHeight()};
            double add_length, add_danger;
            path.splice(path.end(), random_path(grid, start, next, &add_length, &add_danger));
            length += add_length;
            danger += add_danger;
            start = next;
        }
        std::pair<int, int> end = grid.getGoalNode();
        double add_legnth, add_danger;
        path.splice(path.end(), random_path(grid, start, end, &add_legnth, &add_danger));
        danger += add_danger;
        length += add_legnth;
        danger += grid.getCellDanger(end.first, end.second);
    }
    std::pair<Individ, Individ> crossover(const Individ& other) const {
        std::unordered_map<std::pair<int, int>,
                           std::list<GeneticNode>::const_iterator,
                           HashCoordinate
                           > cells{10, HashCoordinate{grid.getMapHeight()}};
        for (auto it = path.begin(); it != path.end(); ++it) {
            cells[it->cell] = it;
        }
        int count = 0;
        for (auto it = other.path.begin(); it != other.path.end(); ++it) {
            if (cells.find(it->cell) != cells.end()) {
                ++count;
            }
        }
        if (count == 0) {
            //TODO
            return {*this, other};
        }
        int num = rand() % count;
        
        for (auto it = other.path.begin(); it != other.path.end(); ++it) {
            if (cells.find(it->cell) != cells.end()) {
                if (num != 0) {
                    //TODO
                }
            }
        }
        //TODO
    }
    void mutation();
    void invalid_refiner();
    void length_refiner();
    void danger_refiner();
    void remove_cycles();
};

void nsga_ii(std::vector<Individ>& generation);

GeneticAlgorithm::GeneticAlgorithm() {}

GeneticAlgorithm::~GeneticAlgorithm() {}

SearchResult GeneticAlgorithm::startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) {
    //TODO
    sresult.nodescreated = 0;
    sresult.numberofsteps = 0;
    sresult.pathfound = false;
    sresult.time = 0;
    return sresult;
}

std::pair<Individ, Individ> Individ::crossover(const Individ& other) const {

}
#include "search.h"
#include <chrono>
#include <memory>
#include <iterator>
#include <map>
#include <unordered_map>

Search::Search()
{
//set defaults here
}

Search::~Search() {}

class Heuristic {
protected:
    double w;
public:
    Heuristic(double w): w(w) {}
    virtual double metric(int, int) const = 0;
    double operator()(int i, int j) const {
        return metric(i, j) * w;
    }
};

class ZeroHeuristic : public Heuristic {
public:
    ZeroHeuristic(): Heuristic(1.) {}
    double metric(int, int) const override {
        return 0;
    }
};

class EuclidianHeuristic : public Heuristic {
    int x;
    int y;
public:
    EuclidianHeuristic(std::pair<int, int> goal, double w): Heuristic(w), x(goal.first), y(goal.second) {}
    double metric(int i, int j) const override {
        return sqrt((i - x) * (i - x) + (j - y) * (j - y));
    }
};

class ChebishevHeuristic : public Heuristic {
    int x;
    int y;
public:
    ChebishevHeuristic(std::pair<int, int> goal, double w): Heuristic(w), x(goal.first), y(goal.second) {}
    double metric(int i, int j) const override {
        return std::max(abs(i - x), abs(j - y));
    }
};

class OctileHeuristic : public Heuristic {
    int x, y;
public:
    OctileHeuristic(std::pair<int, int> goal, double w): Heuristic(w), x(goal.first), y(goal.second) {}
    double metric(int i, int j) const override {
        return (std::min(abs(i - x), abs(j - y)) * sqrt(2.) + abs(abs(i - x) - abs(j - y)));
    }
};

class ManhattanEuristic : public Heuristic {
    int x, y;
public:
    ManhattanEuristic(std::pair<int, int> goal, double w): Heuristic(w), x(goal.first), y(goal.second) {}
    double metric(int i, int j) const override {
        return (abs(i - x) + abs(j - y));
    }
};

class HashCoordinate {
    int map_width;
public:
    HashCoordinate(int width): map_width(width) {}

    int operator()(std::pair<int, int> coor) const {
        return std::hash<int>()(coor.first * map_width + coor.second);
    }
};

class OpenContainer {
    std::multimap<int ,Node> tree;
    std::unordered_map<std::pair<int, int>, decltype(tree)::const_iterator, HashCoordinate> table;
public:
    OpenContainer(HashCoordinate hash): tree(), table(10, hash) {}

    bool empty() const {
        return tree.empty();
    }

    size_t size() const {
        return tree.size();
    }

    Node extract_min() {
        Node res = tree.begin()->second;
        tree.erase(tree.begin());
        table.erase({res.i, res.j});
        return res;
    }

    bool has_node(Node node) const {
        return table.find({node.i, node.j}) != table.end();
    }

    void update_node(Node node) {
        auto it = table.find({node.i, node.j});
        if (it != table.end()) {                
            if (it->second->second.F < node.F) {
                return;
            }
            tree.erase(it->second);
            table.erase(it);
        }
        table.insert({{node.i, node.j}, tree.insert({node.F, node})});
    }
};

//give access to the nodes which we must update from the current node
class Expansion {
    const Map& map;
    const EnvironmentOptions& op;
    const Heuristic& h;
    const int dx[10]{1, 1, 1, 0, -1, -1, -1, 0, 1, 1};
    const int dy[10]{-1, 0, 1, 1, 1, 0, -1, -1, -1, 0};
    const double dl[10]{sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1.};
public:
    Expansion(const Map& m, const EnvironmentOptions& op, const Heuristic& h): map(m), op(op), h(h) {}

    size_t size() const {
        return 8;
    }

    //get returns the k-th node in which we can pass form cur
    //if k-th node is unreachable, returns node with parent == nullptr
    Node get(Node& cur, int k) const {
        ++k;
        if (!map.CellOnGrid(cur.i + dx[k], cur.j + dy[k]) || map.CellIsObstacle(cur.i + dx[k], cur.j + dy[k])) {
            return {-1, -1, -1, -1, -1, nullptr};
        }
        if (k % 2 == 0) {
            if (!op.allowdiagonal) {
                return {-1, -1, -1, -1, -1, nullptr};
            } else if (!op.cutcorners) {
                if (map.CellIsObstacle(cur.i + dx[k - 1], cur.j + dy[k - 1]) || map.CellIsObstacle(cur.i + dx[k + 1], cur.j + dy[k + 1])) {
                    return {-1, -1, -1, -1, -1, nullptr};
                }
            } else if (!op.allowsqueeze) {
                if (map.CellIsObstacle(cur.i + dx[k - 1], cur.j + dy[k - 1]) && map.CellIsObstacle(cur.i + dx[k + 1], cur.j + dy[k + 1])) {
                    return {-1, -1, -1, -1, -1, nullptr};
                }
            }
        }
        Node nxt{cur.i + dx[k], cur.j + dy[k], 0, 0, 0, &cur};
        nxt.H = h(nxt.i, nxt.j);
        nxt.g = cur.g + dl[k];
        nxt.F = nxt.H + nxt.g;
        return nxt;
    }
};

//make hppath from lppath
std::list<Node> compressPath(const std::list<Node> path) {
    std::list<Node> res;
    if (path.empty()) {
        return res;
    }
    res.push_back(path.front());
    if (path.size() == 1) {
        return res;
    }
    auto cur = *next(path.begin());
    auto prev = res.back();
    for (auto it = next(path.begin(), 2); it != path.end(); ++it) {
        if (it->i - cur.i == cur.i - prev.i && it->j - cur.j == cur.j - prev.j) {
            prev = cur;
            cur = *it;
        } else {
            res.push_back(cur);
            prev = cur;
            cur = *it;
        }
    }
    res.push_back(cur);
    return res;
}

SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{
    auto start_time = std::chrono::steady_clock::now();

    OpenContainer open{HashCoordinate{map.getMapWidth()}};
    std::unordered_map<std::pair<int, int>, Node, HashCoordinate> close{
        10, 
        HashCoordinate{map.getMapWidth()}
    };

    std::unique_ptr<Heuristic> h;
    if (options.algorithm == CN_SP_ST_DIJK) {
        h.reset(new ZeroHeuristic());
    } else {
        switch (options.metrictype) {
        case CN_SP_MT_CHEB:
            h.reset(new ChebishevHeuristic(map.getGoalNode(), options.hweight));
            break;
        case CN_SP_MT_DIAG:
            h.reset(new OctileHeuristic(map.getGoalNode(), options.hweight));
            break;
        case CN_SP_MT_EUCL:
            h.reset(new EuclidianHeuristic(map.getGoalNode(), options.hweight));
            break;
        case CN_SP_MT_MANH:
            h.reset(new ManhattanEuristic(map.getGoalNode(), options.hweight));
            break;
        default:
            break;
        }
    }
    Expansion expansion{map, options, *h};

    Node* end = nullptr;
    open.update_node({map.getStartNode().first, map.getStartNode().second, h->operator()(map.getStartNode().first, map.getStartNode().second), 0, h->operator()(map.getStartNode().first, map.getStartNode().second), nullptr});

    int countNumberOfSteps = 0;

    while (!open.empty()) {
        ++countNumberOfSteps;

        auto node = open.extract_min();

        auto it = close.insert({{node.i, node.j}, node}).first;
        if (it->first == map.getGoalNode()) {
            end = &it->second;
            break;
        }

        for (int k = 0; k < expansion.size(); ++k) {
            auto nxt = expansion.get(it->second, k);
            if (!nxt.parent) {
                continue;
            }

            if (close.find({nxt.i, nxt.j}) != close.end()) {
                continue;
            }

            open.update_node(nxt);
        }
    }

    sresult.pathfound = end != nullptr;
    if (sresult.pathfound) {
        sresult.pathlength = end->g;
    }

    while (end != nullptr) {
        lppath.push_back(*end);
        end = end->parent;
    }

    lppath.reverse();

    sresult.time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000000.0;

    hppath = compressPath(lppath);

    // for (auto node : lppath) {
    //     std::cout << node.i << ' ' << node.j << std::endl;
    // }

    sresult.nodescreated = open.size() + close.size();
    sresult.numberofsteps = countNumberOfSteps;
    sresult.hppath = &hppath;
    sresult.lppath = &lppath;

    //need to implement

    /*sresult.pathfound = ;
    sresult.nodescreated =  ;
    sresult.numberofsteps = ;
    sresult.time = ;
    sresult.hppath = &hppath; //Here is a constant pointer
    sresult.lppath = &lppath;*/
    return sresult;
}

/*void Search::makePrimaryPath(Node curNode)
{
    //need to implement
}*/

/*void Search::makeSecondaryPath()
{
    //need to implement
}*/

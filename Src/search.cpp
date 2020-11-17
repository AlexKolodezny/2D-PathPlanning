#include "search.h"
#include <chrono>
#include <memory>
#include <iterator>

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
    virtual double operator()(int, int) const = 0;
};

class ZeroHeuristic : public Heuristic {
public:
    ZeroHeuristic(): Heuristic(1.) {}
    double operator()(int, int) const override {
        return 0;
    }
};

class EuclidianHeuristic : public Heuristic {
    int x;
    int y;
public:
    EuclidianHeuristic(std::pair<int, int> goal, double w): Heuristic(w), x(goal.first), y(goal.second) {}
    double operator()(int i, int j) const override {
        return sqrt((i - x) * (i - x) + (j - y) * (j - y)) * w;
    }
};

class ChebishevHeuristic : public Heuristic {
    int x;
    int y;
public:
    ChebishevHeuristic(std::pair<int, int> goal, double w): Heuristic(w), x(goal.first), y(goal.second) {}
    double operator()(int i, int j) const override {
        return std::max(abs(i - x), abs(j - y)) * w;
    }
};

class OctileHeuristic : public Heuristic {
    int x, y;
public:
    OctileHeuristic(std::pair<int, int> goal, double w): Heuristic(w), x(goal.first), y(goal.second) {}
    double operator()(int i, int j) const override {
        return std::min(abs(i - x), abs(j - y)) * sqrt(2.) + abs(abs(i - x) - abs(j - y)) * w;
    }
};

class ManhattanEuristic : public Heuristic {
    int x, y;
public:
    ManhattanEuristic(std::pair<int, int> goal, double w): Heuristic(w), x(goal.first), y(goal.second) {}
    double operator()(int i, int j) const override {
        return abs(i - x) + abs(j - y) * w;
    }
};

//give access to the nodes which we must update from the current node
class Expansion {
    const Map& map;
    const EnvironmentOptions& op;
    const Heuristic& h;
    const int dx[10]{1, 1, 1, 0, -1, -1, -1, 0, 1, 1};
    const int dy[10]{-1, 0, 1, 1, 1, 0, -1, -1, -1, 0};
    const double dl[10]{CN_SQRT_TWO, 1., CN_SQRT_TWO, 1., CN_SQRT_TWO, 1., CN_SQRT_TWO, 1., CN_SQRT_TWO, 1.};
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
    open.push_back({map.getStartNode().first, map.getStartNode().second, 0, 0, 0, nullptr});

    int countNumberOfSteps = 0;

    while (!open.empty()) {
        ++countNumberOfSteps;
        auto it = std::min_element(open.begin(), open.end(), [](Node a, Node b) {
            return a.F < b.F;
        });

        close.push_back(*it);
        open.erase(it);
        if (close.back().i == map.getGoalNode().first && close.back().j == map.getGoalNode().second) {
            end = &close.back();
            break;
        }

        for (int k = 0; k < expansion.size(); ++k) {
            auto nxt = expansion.get(close.back(), k);
            if (!nxt.parent) {
                continue;
            }

            if (std::find_if(close.begin(), close.end(), [nxt](Node b){
                return nxt.i == b.i && nxt.j == b.j;
            }) != close.end()) {
                continue;
            }

            auto it = std::find_if(open.begin(), open.end(), [nxt](Node b) {
                return nxt.i == b.i && nxt.j == b.j;
            });

            if (it == open.end()) {
                open.push_back(nxt);
            } else if (it->F > nxt.F) {
                *it = nxt;
            }
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

    hppath = compressPath(lppath);


    sresult.nodescreated = open.size() + close.size();
    sresult.numberofsteps = countNumberOfSteps;
    sresult.time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
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

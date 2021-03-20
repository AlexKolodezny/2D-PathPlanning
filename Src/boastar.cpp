#include "boastar.h"
#include <chrono>
#include <memory>
#include <iterator>
#include <map>
#include <unordered_map>
#include "search.h"
#include "path_utils.h"

BOAstarSearch::BOAstarSearch()
{
//set defaults here
}

BOAstarSearch::~BOAstarSearch() {}

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

class OpenContainerTreeCompare {
public:
    bool operator()(std::pair<double, double> x, std::pair<double, double> y) {
        if (x.first == y.first) {
            return x.second < y.second;
        }
        return x.first < y.first;
    }
};

class OpenContainer {
    std::multimap<std::pair<double, double>, Node> tree;
public:
    OpenContainer(): tree() {}

    bool empty() const {
        return tree.empty();
    }

    size_t size() const {
        return tree.size();
    }

    Node extract_min() {
        Node res = tree.begin()->second;
        tree.erase(tree.begin());
        return res;
    }

    void insert_node(Node node) {
        tree.insert({{node.f1, node.f2}, node});
        return;
    }
};

class CloseContainer {
    std::list<Node> container;
    std::unordered_map<std::pair<int, int>, double, HashCoordinate> g_min;
public:
    CloseContainer(HashCoordinate hash): container{}, g_min{10, hash} {}

    size_t size() const {
        return container.size();
    }

    bool empty() const {
        return container.empty();
    }

    bool is_non_dominated(Node node) const {
        if (g_min.find({node.i, node.j}) == g_min.end()) {
            return true;
        }
        return node.g2 < g_min.at({node.i, node.j});
    }

    Node* insert(Node node) {
        container.push_back(node);
        g_min[{node.i, node.j}] = node.g2;
        return &container.back();
    }
};

class Solves {
    std::vector<Node*> container;
    double g_min;
public:
    Solves(): container{}, g_min{} {}

    bool is_non_dominated(Node node) const {
        return container.empty() || node.f2 < g_min;
    }

    void insert_node(Node *node) {
        g_min = node->g2;
        container.push_back(node);
        return;
    }

    const std::vector<Node*>& path_ends() {
        return container;
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
            return {};
        }
        if (k % 2 == 0) {
            if (!op.allowdiagonal) {
                return {};
            } else if (!op.cutcorners) {
                if (map.CellIsObstacle(cur.i + dx[k - 1], cur.j + dy[k - 1]) || map.CellIsObstacle(cur.i + dx[k + 1], cur.j + dy[k + 1])) {
                    return {};
                }
            } else if (!op.allowsqueeze) {
                if (map.CellIsObstacle(cur.i + dx[k - 1], cur.j + dy[k - 1]) && map.CellIsObstacle(cur.i + dx[k + 1], cur.j + dy[k + 1])) {
                    return {};
                }
            }
        }
        Node nxt{
            cur.i + dx[k], 
            cur.j + dy[k], 
            h(cur.i + dx[k], cur.j + dy[k]), 
            cur.g1 + dl[k], 
            cur.g2 + map.getCellDanger(cur.i + dx[k], cur.j + dy[k]),
            &cur};
        return nxt;
    }
};

std::list<Node> make_primary_path(Node *end) {
    std::list<Node> path;
    while (end != nullptr) {
        path.push_front(*end);
        end = end->parent;
    }
    path.front().parent = nullptr;
    for (auto it = std::next(path.begin()); it != path.end(); ++it) {
        it->parent = &*std::prev(it);
    }
    return path;
}

SearchResult BOAstarSearch::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{
    auto start_time = std::chrono::steady_clock::now();

    OpenContainer open{};
    CloseContainer close{HashCoordinate{map.getMapHeight()}};
    Solves solves{};

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

    open.insert_node({
        map.getStartNode().first, 
        map.getStartNode().second, 
        h->operator()(map.getStartNode().first, map.getStartNode().second), 
        0,
        0,
        nullptr});

    int countNumberOfSteps = 0;

    while (!open.empty()) {
        ++countNumberOfSteps;

        auto node = open.extract_min();

        if (!close.is_non_dominated(node) || !solves.is_non_dominated(node)) {
            continue;
        }

        auto ptr = close.insert(node);
        if (ptr->i == map.getGoalNode().first && ptr->j == map.getGoalNode().second) {
            solves.insert_node(ptr);
        }

        for (int k = 0; k < expansion.size(); ++k) {
            auto nxt = expansion.get(*ptr, k);
            if (!nxt.parent) {
                continue;
            }
            if (!close.is_non_dominated(nxt) || !solves.is_non_dominated(nxt)) {
                continue;
            }
            open.insert_node(nxt);
        }
    }

    for (auto end : solves.path_ends()) {
        sresult.lppaths.push_back(make_primary_path(end));
    }

    sresult.time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000000.0;

    for (auto path : sresult.lppaths) {
        sresult.hppaths.push_back(make_secondary_path(path));
        sresult.pathlength.push_back(path.back().g1);
    }

    sresult.pathfound = !sresult.lppaths.empty();

    sresult.nodescreated = open.size() + close.size();
    sresult.numberofsteps = countNumberOfSteps;
    return sresult;
}



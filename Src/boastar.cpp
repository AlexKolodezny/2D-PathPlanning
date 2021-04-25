#include "boastar.h"
#include <chrono>
#include <memory>
#include <iterator>
#include <map>
#include <unordered_map>
#include "search.h"
#include "section.h"
#include "node.h"
#include <cmath>
#include <queue>

BOAstarSearch::BOAstarSearch(std::unique_ptr<DangerObjective>&& obj): obj(std::move(obj)) {}

BOAstarSearch::~BOAstarSearch() {}

class Heuristic {
protected:
    double w;
public:
    Heuristic(double w): w(w) {}
    virtual double metric(Cell) const = 0;
    double operator()(Cell c) const {
        return metric(c) * w;
    }
};

class ZeroHeuristic : public Heuristic {
public:
    ZeroHeuristic(): Heuristic(1.) {}
    double metric(Cell) const override {
        return 0;
    }
};

class EuclidianHeuristic : public Heuristic {
    Cell goal;
public:
    EuclidianHeuristic(Cell goal, double w): Heuristic(w), goal{goal} {}
    double metric(Cell c) const override {
        c -= goal;
        return sqrt(c.first * c.first + c.second * c.second);
    }
};

class ChebishevHeuristic : public Heuristic {
    Cell goal;
public:
    ChebishevHeuristic(Cell goal, double w): Heuristic(w), goal{goal} {}
    double metric(Cell c) const override {
        c -= goal;
        return std::max(std::abs(c.first), std::abs(c.second));
    }
};

class OctileHeuristic : public Heuristic {
    Cell goal;
public:
    OctileHeuristic(Cell goal, double w): Heuristic(w), goal{goal} {}
    double metric(Cell c) const override {
        c -= goal;
        c.first = std::abs(c.first);
        c.second = std::abs(c.second);
        return (std::min(c.first, c.second) * sqrt(2.) + std::abs(c.first - c.second));
    }
};

class ManhattanEuristic : public Heuristic {
    Cell goal;
public:
    ManhattanEuristic(Cell goal, double w): Heuristic(w), goal{goal} {}
    double metric(Cell c) const override {
        c -= goal;
        return (std::abs(c.first) + std::abs(c.second));
    }
};

class OpenContainerHeapCompare {
public:
    bool operator()(std::pair<std::pair<double, double>, Node> x, std::pair<std::pair<double, double>, Node> y) {
        return x.first > y.first;
    }
};

class OpenContainer {
    std::priority_queue<
        std::pair<std::pair<double, double>, Node>,
        std::vector<std::pair<std::pair<double, double>, Node>>,
        OpenContainerHeapCompare> heap;
public:
    OpenContainer(): heap() {}

    bool empty() const {
        return heap.empty();
    }

    size_t size() const {
        return heap.size();
    }

    Node extract_min() {
        Node res = heap.top().second;
        heap.pop();
        return res;
    }

    void insert_node(Node node) {
        heap.push({{node.f1, node.f2}, node});
        return;
    }
};

class CloseContainer {
    std::list<Node> container;
    std::unordered_map<Cell, double, HashCoordinate> g_min;
    std::unordered_map<Cell, double, HashCoordinate> g_max;
public:
    CloseContainer(HashCoordinate hash): container{}, g_min{10, hash}, g_max{10, hash} {}

    size_t size() const {
        return container.size();
    }

    bool empty() const {
        return container.empty();
    }

    bool is_non_dominated(Node node) const {
        if (g_min.find(node) == g_min.end()) {
            return true;
        }
        return node.g2 < g_min.at(node) && node.g1 > g_max.at(node);
    }

    Node* insert(Node node) {
        container.push_back(node);
        g_min[node] = node.g2;
        g_max[node] = node.g1;
        return &container.back();
    }
};

class Solves {
    std::vector<Node*> container;
    double g_min;
    double g_max;
public:
    Solves(): container{}, g_min{}, g_max{} {}

    bool is_non_dominated(Node node) const {
        return container.empty() || (node.f2 < g_min && node.f1 > g_max);
    }

    void insert_node(Node *node) {
        g_min = node->g2;
        g_max = node->g1;
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
    const DangerObjective& obj;
    const Cell diff[10]{
        {1, -1},
        {1, 0},
        {1, 1},
        {0, 1},
        {-1, 1},
        {-1, 0},
        {-1, -1},
        {0, -1},
        {1, -1},
        {1, 0}
    };
    const double dl[10]{sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1.};
public:
    Expansion(const Map& m, const EnvironmentOptions& op, const Heuristic& h, const DangerObjective& obj): map(m), op(op), h(h), obj(obj) {}

    size_t size() const {
        return 8;
    }

    //get returns the k-th node in which we can pass form cur
    //if k-th node is unreachable, returns node with parent == nullptr
    Node get(Node& cur, int k) const {
        ++k;
        if (!map.CellOnGrid(cur + diff[k]) || map.CellIsObstacle(cur + diff[k])) {
            return {};
        }
        if (k % 2 == 0) {
            if (!op.allowdiagonal) {
                return {};
            } else if (!op.cutcorners) {
                if (map.CellIsObstacle(cur + diff[k - 1]) || map.CellIsObstacle(cur + diff[k + 1])) {
                    return {};
                }
            } else if (!op.allowsqueeze) {
                if (map.CellIsObstacle(cur + diff[k - 1]) && map.CellIsObstacle(cur + diff[k + 1])) {
                    return {};
                }
            }
        }
        Node nxt{
            cur + diff[k],
            h(cur + diff[k]), 
            cur.g1 + dl[k], 
            cur.g2 + obj(map.getCellDanger(cur + diff[k])),
            &cur};
        return nxt;
    }
};

std::list<Cell> make_primary_path(Node *end) {
    std::list<Cell> path;
    while (end != nullptr) {
        path.push_front(*end);
        end = end->parent;
    }
    return path;
}

SearchResult BOAstarSearch::startSearch(ILogger *, const Map &map, const EnvironmentOptions &options)
{

    auto start_time = std::chrono::steady_clock::now();

    OpenContainer open{};
    CloseContainer close{HashCoordinate{map.getMapHeight()}};
    Solves solves{};

    BOAstarAlgorithmOptions *boastar_algorithm_options = dynamic_cast<BOAstarAlgorithmOptions*>(options.algorithm_options.get());

    std::unique_ptr<Heuristic> h;
    if (boastar_algorithm_options->algorithm == CN_SP_ST_DIJK) {
        h.reset(new ZeroHeuristic());
    } else {
        switch (boastar_algorithm_options->metrictype) {
        case CN_SP_MT_CHEB:
            h.reset(new ChebishevHeuristic(map.getGoalNode(), boastar_algorithm_options->hweight));
            break;
        case CN_SP_MT_DIAG:
            h.reset(new OctileHeuristic(map.getGoalNode(), boastar_algorithm_options->hweight));
            break;
        case CN_SP_MT_EUCL:
            h.reset(new EuclidianHeuristic(map.getGoalNode(), boastar_algorithm_options->hweight));
            break;
        case CN_SP_MT_MANH:
            h.reset(new ManhattanEuristic(map.getGoalNode(), boastar_algorithm_options->hweight));
            break;
        default:
            break;
        }
    }
    Expansion expansion{map, options, *h, *obj};

    open.insert_node({
        map.getStartNode(),
        h->operator()(map.getStartNode()), 
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
        if (*ptr == map.getGoalNode()) {
            solves.insert_node(ptr);
        }

        for (size_t k = 0; k < expansion.size(); ++k) {
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
        auto res = make_primary_path(end);
        sresult.paths.push_back({res, make_secondary_path(res), end->g1, end->g2});
    }

    sresult.time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000000.0;

    sresult.nodescreated = open.size() + close.size();
    sresult.numberofsteps = countNumberOfSteps;
    return sresult;
}



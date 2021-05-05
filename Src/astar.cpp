#include "astar.h"
#include <chrono>
#include <memory>
#include <iterator>
#include <map>
#include <unordered_map>
#include "node.h"
#include "heuristic.h"

AstarSearch::AstarSearch()
{
//set defaults here
}

AstarSearch::~AstarSearch() {}

class AstarOpenContainerTreeCompare {
public:
    bool operator()(std::pair<double, double> x, std::pair<double, double> y) const {
        if (x.first == y.first) {
            return x.second > y.second;
        }
        return x.first < y.first;
    }
};

class AstarOpenContainer {
    std::multimap<std::pair<double, double>, Node, AstarOpenContainerTreeCompare> tree;
    std::unordered_map<Cell, decltype(tree)::const_iterator, HashCoordinate> table;
public:
    AstarOpenContainer(HashCoordinate hash): tree(), table(10, hash) {}

    bool empty() const {
        return tree.empty();
    }

    size_t size() const {
        return tree.size();
    }

    Node extract_min() {
        Node res = tree.begin()->second;
        tree.erase(tree.begin());
        table.erase(res);
        return res;
    }

    bool has_node(Node node) const {
        return table.find(node) != table.end();
    }

    void update_node(Node node) {
        auto it = table.find(node);
        if (it != table.end()) {                
            if (it->second->second.f1 <= node.f1) {
                return;
            }
            tree.erase(it->second);
            table.erase(it);
        }
        table[node] = tree.insert({{node.f1, node.g1}, node});
    }
};

//give access to the nodes which we must update from the current node
class AstarExpansion {
    const Map& map;
    const EnvironmentOptions& op;
    const Heuristic& h;
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
    AstarExpansion(const Map& m, const EnvironmentOptions& op, const Heuristic& h): map(m), op(op), h(h) {}

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
            0,
            &cur};
        return nxt;
    }
};

SearchResult AstarSearch::startSearch(ILogger *, const Map &map, const EnvironmentOptions &options)
{
    auto start_time = std::chrono::steady_clock::now();

    AstarOpenContainer open{HashCoordinate{map.getMapHeight()}};
    std::unordered_map<Cell, Node, HashCoordinate> close{
        10, 
        HashCoordinate{map.getMapHeight()}
    };

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
    AstarExpansion expansion{map, options, *h};

    Node* end = nullptr;
    open.update_node({map.getStartNode(), h->operator()(map.getStartNode()), 0, 0, nullptr});

    int countNumberOfSteps = 0;

    while (!open.empty()) {
        ++countNumberOfSteps;

        auto node = open.extract_min();

        auto it = close.insert({node, node}).first;
        if (it->first == map.getGoalNode()) {
            end = &it->second;
            break;
        }

        for (size_t k = 0; k < expansion.size(); ++k) {
            auto nxt = expansion.get(it->second, k);
            if (!nxt.parent) {
                continue;
            }

            if (close.find(nxt) != close.end()) {
                continue;
            }

            open.update_node(nxt);
        }
    }

    if (end != nullptr) {
        auto res = make_primary_path(end);
        sresult.paths.push_back({res, make_secondary_path(res), end->g1, end->g2});
    }

    sresult.time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000000.0;

    sresult.nodescreated = open.size() + close.size();
    sresult.numberofsteps = countNumberOfSteps;

    return sresult;
}

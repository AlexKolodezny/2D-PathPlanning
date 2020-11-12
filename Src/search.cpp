#include "search.h"
#include <chrono>
#include <memory>
#include <iterator>

Search::Search()
{
//set defaults here
}

Search::~Search() {}

//give access to the nodes which we must update from the current node
class Expansion {
    const Map& map;
    const EnvironmentOptions& op;
    std::pair<int, int> goal;
    const int dx[10]{1, 1, 1, 0, -1, -1, -1, 0, 1, 1};
    const int dy[10]{-1, 0, 1, 1, 1, 0, -1, -1, -1, 0};
    const double dl[10]{sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1., sqrt(2.), 1.};
public:
    Expansion(const Map& m, const EnvironmentOptions& op): map(m), op(op), goal(m.getGoalNode()) {}

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
        Node nxt{cur.i + dx[k], cur.j + dy[k], cur.g + dl[k], cur.g + dl[k], 0, &cur};
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

    Expansion expansion(map, options);

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

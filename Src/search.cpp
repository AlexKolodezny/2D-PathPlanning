#include "search.h"
#include <chrono>

Search::Search()
{
//set defaults here
}

Search::~Search() {}


SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{

    int dx[4]{0, 1, 0, -1};
    int dy[4]{1, 0, -1, 0};
    double dl[4]{1., 1., 1., 1.};

    auto start_time = std::chrono::steady_clock::now();

    Node* end = nullptr;
    open.push_back({map.getStartNode().first, map.getStartNode().second, 0, 0, 0, nullptr});

    int countNumberOfSteps = 0;

    while (!open.empty()) {
        ++countNumberOfSteps;
        auto it = std::min_element(open.begin(), open.end(), [](Node a, Node b) {
            return a.F < b.F;
        });

        Node cur = *it;
        open.erase(it);
        close.push_back(cur);
        if (cur.i == map.getGoalNode().first && cur.j == map.getGoalNode().second) {
            end = &close.back();
            break;
        }

        for (int k = 0; k < 4; ++k) {
            Node nxt{cur.i + dx[k], cur.j + dy[k], cur.g + dl[k], cur.g + dl[k], 0, &close.back()};
            if (!map.CellOnGrid(nxt.i, nxt.j) || map.CellIsObstacle(nxt.i, nxt.j)) {
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

    hppath = lppath;


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

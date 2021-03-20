#include "path_utils.h"
#include "node.h"
#include <list>

//make hppath from lppath
std::list<Node> make_secondary_path(const std::list<Node>& path) {
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
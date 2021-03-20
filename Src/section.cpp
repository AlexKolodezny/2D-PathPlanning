#include "section.h"
#include "node.h"
#include <cmath>
#include <list>

//make hppath from lppath
std::list<Section> make_secondary_path(const std::list<Cell>& path) {
    std::list<Section> res;
    if (path.empty()) {
        return res;
    }
    res.emplace_back(path.front(), 0);
    if (path.size() == 1) {
        return res;
    }
    auto cur = *next(path.begin());
    Cell prev = res.back();
    for (auto it = next(path.begin(), 2); it != path.end(); ++it) {
        if (*it - cur == cur - prev) {
            prev = cur;
            cur = *it;
        } else {
            prev -= cur;
            res.emplace_back(cur, std::sqrt(prev.first * prev.first + prev.second * prev.second));
            prev = cur;
            cur = *it;
        }
    }
    prev -= cur;
    res.emplace_back(cur, std::sqrt(prev.first * prev.first + prev.second * prev.second));
    return res;
}
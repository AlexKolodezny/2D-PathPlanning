#include "node.h"
#include <list>

std::list<Cell> make_primary_path(Node *end) {
    std::list<Cell> path;
    while (end != nullptr) {
        path.push_front(*end);
        end = end->parent;
    }
    return path;
}

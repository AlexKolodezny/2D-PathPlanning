#ifndef NODE_H
#define NODE_H

#include "cell.h"

//That's the data structure for storing a single search node.

struct Node : public Cell
{
    double  h1; // h-value of the search node
    double  f1, g1; //f1 and g1-values of the search node
    double  f2, g2; //f2 and g2-values of the search node
    Node    *parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)
    Node(Cell c, double h1, double g1, double g2, Node *parent)
        : Cell(c), h1(h1), f1(h1 + g1), g1(g1), f2(g2), g2(g2), parent(parent) {}
    Node(): Cell{-1, -1}, h1(-1), f1(-1), g1(-1), f2(-1), g2(-1), parent(nullptr) {}
};

#endif

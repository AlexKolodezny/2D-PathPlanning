#ifndef NODE_H
#define NODE_H

//That's the data structure for storing a single search node.

struct Node
{
    int     i, j; //grid cell coordinates
    double  h; // h-value of the search node
    double  f1, g1; //f1 and g1-values of the search node
    double  f2, g2; //f2 and g2-values of the search node
    Node    *parent; //backpointer to the predecessor node (e.g. the node which g-value was used to set the g-velue of the current node)
};
#endif

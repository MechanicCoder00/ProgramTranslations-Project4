#ifndef INC_4280P0CPP_NODE_H
#define INC_4280P0CPP_NODE_H

#include "token.h"

/*
Author: Scott Tabaka
CMPSCI 4280 Project4
*/

typedef struct node
{
    string label;
    token node_tk;

    node *child1;
    node *child2;
    node *child3;
    node *child4;
}node;

#endif
#include <iostream>
#include <fstream>
using namespace std;
#include "node.h"
#include "testTree.h"
#include "scanner.h"


/*
Author: Scott Tabaka
CMPSCI 4280 Project4
*/

void treePrint(node *currentNode, int level)
{
    if (currentNode == NULL)
    {
        return;
    } else {
        cout << string(2*level, ' ');
        level++;

        cout << currentNode->label << " ";
        token t = currentNode->node_tk;

        if (t.tokenInstance != "")
        {
            cout << "Token ID: " << tokenNames[t.tokenId] << " \"" << t.tokenInstance << "\"";
        }
        cout << endl;

        treePrint(currentNode->child1, level);
        treePrint(currentNode->child2, level);
        treePrint(currentNode->child3, level);
        treePrint(currentNode->child4, level);
    }
}

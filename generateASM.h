#ifndef INC_4280P3_STATSEM_H
#define INC_4280P3_STATSEM_H

/*
Author: Scott Tabaka
CMPSCI 4280 Project4
*/

void errorStatSem1(token t, ofstream &output);
void errorStatSem2(token t, ofstream &output);
void generateASM(node *currentNode, int stack_lvl, ofstream &output, string outputFilename);
static void generateCode(const node *currentNode, int scope_lvl, ofstream &output);
void declareVars(const node *currentNode, int level, ofstream &output);
bool verify(string s, int level);
void checkDeclaration(node *currentNode, int level, ofstream &output);
int findStackLocation(string s, int level);
int find(string s, int level);
void emptyStack(int level, ofstream &output);
int countElements(int level);

void switchTraversalPrint(const node *currentNode);
void displayScopeStack(int level);
void debugDisplay(const node *currentNode, int stack_lvl);

#endif

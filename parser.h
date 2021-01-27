#ifndef TESTSCANNER_H
#define TESTSCANNER_H

#include <iostream>
using namespace std;
#include "token.h"
#include "node.h"
#include "testTree.h"


/*
Author: Scott Tabaka
CMPSCI 4280 Project4
*/

void error(string s1, string s2);
void error(string s1, string s2, string s3, string s4);
void error(string s1, string s2, string s3, string s4, string s5, string s6);

node* parser(ifstream& input);

node* program(ifstream& input);
node* block(ifstream& input);
node* vars(ifstream& input);
node* expr(ifstream& input);
node* A(ifstream& input);
node* N(ifstream& input);
node* M(ifstream& input);
node* R(ifstream& input);
node* stats(ifstream& input);
node* mStat(ifstream& input);
node* stat(ifstream& input);
node* in(ifstream& input);
node* out(ifstream& input);
node* if2(ifstream& input);
node* loop(ifstream& input);
node* assign(ifstream& input);
node* RO(ifstream& input);


#endif

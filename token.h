#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
using namespace std;


/*
Author: Scott Tabaka
CMPSCI 4280 Project4
*/

enum tokenID {
    IDENT_tk,
    NUM_tk,
    ASSIGN_tk,
    RELAT_tk,
    ARITH_tk,
    COMMA_tk,
    PAREN_tk,
    DELIM_tk,
    KW_tk,
    EOF_tk,
};

struct token {
    tokenID tokenId;
    string tokenInstance;
    int lineNum;
};


#endif
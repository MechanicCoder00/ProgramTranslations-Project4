#include <iostream>
#include <fstream>
using namespace std;
#include "token.h"
#include "scanner.h"
#include "parser.h"


/*
Author: Scott Tabaka
CMPSCI 4280 Project4
*/

//Finite State Automaton Table
int fsaTable[24][23] = {
        {0,1,-1,9,17,19,19,20,20,20,20,20,21,22,22,22,22,22,22,23,23,23,-1},
        {1001,2,2,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1001,3,3,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1001,4,4,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1001,5,5,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1001,6,6,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1001,7,7,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1001,8,8,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1001,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1002,-1,-1,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1002,-1,-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1002,-1,-1,12,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1002,-1,-1,13,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1002,-1,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1002,-1,-1,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1002,-1,-1,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1002,-1,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1003,-1,-1,-1,18,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1004,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1004,-1,-1,-1,18,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1005,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1006,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1007,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1008,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

string tokenNames[10] = {"IDENT_tk",
                         "NUM_tk",
                         "ASSIGN_tk",
                         "RELAT_tk",
                         "ARITH_tk",
                         "COMMA_tk",
                         "PAREN_tk",
                         "DELIM_tk",
                         "KW_tk",
                         "EOF_tk"};

//array of keywords
string keywordArray[12] = {"start","stop","iterate","void","var","return","in","out","program","cond","then","let"};

int lookupKW(string instance)       //function for matching an identity token instance to a keyword
{
    for(int i=0;i<12;i++)
    {
        if(instance == keywordArray[i])     //if a match is found
        {
            return 1009;        //return keyword token final state
        }
    }
    return 1001;    //if no match is found then return identity token final state
}

token lookupToken(int val, string instance)         //function for matching final state to a token
{
    token t;
    switch (val)
    {
        case 1001:                          //if final state==1001
            t.tokenId = IDENT_tk;           //assign identity token to the token id
            break;
        case 1002:
            t.tokenId = NUM_tk;
            break;
        case 1003:
            t.tokenId = ASSIGN_tk;
            break;
        case 1004:
            t.tokenId = RELAT_tk;
            break;
        case 1005:
            t.tokenId = ARITH_tk;
            break;
        case 1006:
            t.tokenId = COMMA_tk;
            break;
        case 1007:
            t.tokenId = PAREN_tk;
            break;
        case 1008:
            t.tokenId = DELIM_tk;
            break;
        case 1009:
            t.tokenId = KW_tk;
            break;
        case 1010:
            t.tokenId = EOF_tk;
            break;
        default:
            break;
    }
    if(t.tokenId != EOF_tk)
    {
        t.tokenInstance = instance;     //assign instance string to token instance string
    } else
    {
        t.tokenInstance = "EOF";        //assign "EOF" to token instance string
    }
    return t;
}

int lookupFSAInput(char lookAhead)      //function to identify individual characters and return them as a integer
{
    if (isspace(lookAhead))     //if a Whitespace
    {
        return 0;
    } else if (isalpha(lookAhead))  //if a letter
    {
        if(islower(lookAhead))      //if a lowercase letter
        {
            return 1;
        }
        else return 2;              //if a uppercase letter
    } else if (isdigit(lookAhead))  //if a digit
    {
        return 3;
    }
    switch(lookAhead)               //switch for matching included characters for this project
    {
        case '=':                   //if character is a '='
            return 4;               //return 4 for the fsa input
        case '<':
            return 5;
        case '>':
            return 6;
        case '+':
            return 7;
        case '-':
            return 8;
        case '*':
            return 9;
        case '/':
            return 10;
        case '%':
            return 11;
        case ',':
            return 12;
        case '(':
            return 13;
        case ')':
            return 14;
        case '{':
            return 15;
        case '}':
            return 16;
        case '[':
            return 17;
        case ']':
            return 18;
        case ':':
            return 19;
        case '.':
            return 20;
        case ';':
            return 21;
        default:
            return 22;
    }
}

token scanner(ifstream& input, int lineNum)         //scanner function that builds from a file and returns tokens
{
    char lookahead;
    int fsaInput = 0;
    int currState = 0;
    int nextState = 0;
    string instance;
    token t;

    while (currState >= 0 && currState <= 1000)         //while not in a final state or error state
    {
        if(!input.eof())                                //while not end of file
        {

            input.get(lookahead);
            if(input.eof() && currState != 0)           //needed to read last char in file
            {
                fsaInput = lookupFSAInput(' ');
                nextState = fsaTable[currState][fsaInput];

                if(nextState == 1001)           //if final state was a identity token
                {
                    nextState = lookupKW(instance);     //check if "instance" is a keyword
                }

                t = lookupToken(nextState, instance);
                t.lineNum = lineNum;
                return t;
            }

            if(lookahead != '#')                    //if not a comment
            {
                fsaInput = lookupFSAInput(lookahead);

                if(fsaInput != 0)
                {
                    instance += lookahead;
                }

                nextState = fsaTable[currState][fsaInput];
            } else if (lookahead == '#')            //if a comment
            {
                while (lookahead != '\n' && !input.eof())   //while loop that will skip the remainder of the line
                {
                        input.get(lookahead);
                }
            }
        } else if(currState != 0)                   //if in a current state at eof
        {
            fsaInput = lookupFSAInput(' ');
            nextState = fsaTable[currState][fsaInput];
        } else                                      //will assign eof final state to the next state
        {
            nextState = 1010;
        }

        if(nextState < 0)                   //if in a error state
        {
            if(!input.eof())
            {
                while (fsaInput != 0 && !input.eof())       //loop that will capture the entire string of an error
                {
                    input.get(lookahead);
                    fsaInput = lookupFSAInput(lookahead);

                    if (fsaInput != 0 && !input.eof())
                    {
                        instance += lookahead;
                    }
                }
            }

            if(nextState == -2)         //error messages
            {
                cout << "SCANNER ERROR: Input greater than 8 characters --> " << instance << " <-- on line# " << lineNum << endl;
            } else
            {
                cout << "SCANNER ERROR: Invalid input --> " << instance << " <-- on line# " << lineNum << endl;
            }
            exit(1);

        } else if(nextState > 1000)          //if in a final state
        {
            if(nextState == 1001)           //if final state was a identity token
            {
                nextState = lookupKW(instance);     //check if "instance" is a keyword
            }

            t = lookupToken(nextState, instance);
            t.lineNum = lineNum;

            input.putback(lookahead);
            return t;
        } else if(lookahead == '\n')      //this is for counting on unix systems
        {
            lineNum++;          //advances line number count
        }
        currState = nextState;          //assigns the state for the next iteration of scanner loop
    }

    t.tokenInstance = "Scanner Error: No output";
    return t;
}
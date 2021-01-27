#include <iostream>
#include <fstream>
using namespace std;
#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "testTree.h"


/*
Author: Scott Tabaka
CMPSCI 4280 Project4
*/

token t;

node *newNode(string s)     //Creates a node with label
{
    node *node = new struct node;
    node->label = s;
    return node;
}

void error(string s1, string s2)    //Error function with 2 inputs
{
    if (s2 != "")
    {
        cout << "PARSER ERROR: <" << tokenNames[t.tokenId] << "> \"" << t.tokenInstance << "\" at line " << t.lineNum
             << ". Expecting <" << s1 << "> \"" << s2 << "\"\n";
        exit(1);
    } else {
        cout << "PARSER ERROR: <" << tokenNames[t.tokenId] << "> \"" << t.tokenInstance << "\" at line " << t.lineNum
             << ". Expecting <" << s1 << ">\n";
        exit(1);
    }
}

void error(string s1, string s2, string s3, string s4)  //Error function with 4 inputs
{
    if(s4 != "")
    {
        cout << "PARSER ERROR: <" << tokenNames[t.tokenId] << "> \"" << t.tokenInstance << "\" at line " << t.lineNum
             << ". Expecting <" << s1 << "> \"" << s2 << "\" or <" << s3 << "> \"" << s4 << "\"\n";
        exit(1);
    } else {
        cout << "PARSER ERROR: <" << tokenNames[t.tokenId] << "> \"" << t.tokenInstance << "\" at line " << t.lineNum
             << ". Expecting <" << s1 << "> \"" << s2 << "\" or <" << s3 << ">\n";
        exit(1);
    }
}

void error(string s1, string s2, string s3, string s4, string s5, string s6)    //Error function with 6 inputs
{
    if(s3 == "")
    {
        cout << "PARSER ERROR: <" << tokenNames[t.tokenId] << "> \"" << t.tokenInstance << "\" at line " << t.lineNum
             << ". Expecting <" << s1 << "> \"" << s2 << "\" or <" << s5 << "> or <" << s6 << ">\n";
        exit(1);
    } else {
        cout << "PARSER ERROR: <" << tokenNames[t.tokenId] << "> \"" << t.tokenInstance << "\" at line " << t.lineNum
             << ". Expecting <" << s1 << "> \"" << s2 << "\" or <" << s3 << "> \"" << s4 << "\" or <" << s5 << "> or <"
             << s6 << ">\n";
        exit(1);
    }
}

node* parser(ifstream& input)       //Parser function
{
    t.lineNum = 1;
    t = scanner(input,t.lineNum);

    node *currentNode = program(input);

//    cout << "***Parse Successful***\n";

    return currentNode;
}

node* program(ifstream& input)      //Program function
{
    node *currentNode = newNode("<program>");
    currentNode->child1 = vars(input);
    currentNode->child2 = block(input);

    if(t.tokenId != EOF_tk)
    {
        error("EOF_tk","");
    }
    return currentNode;
}

node* block(ifstream& input)        //Block function
{
    node *currentNode = newNode("<block>");
    if(t.tokenId == KW_tk && t.tokenInstance == "start")
    {
        t = scanner(input, t.lineNum);
    } else {
        error("KW_tk","start");
    }
    currentNode->child1 = vars(input);
    currentNode->child2 = stats(input);
    if(t.tokenId == KW_tk && t.tokenInstance == "stop")
    {
        t = scanner(input, t.lineNum);
    } else {
        error("KW_tk","stop");
    }
    return currentNode;
}

node* vars(ifstream& input)     //Vars function
{
    node *currentNode = newNode("<vars>");
    if(t.tokenId == KW_tk && t.tokenInstance == "var")
    {
        t = scanner(input, t.lineNum);
        if(t.tokenId == IDENT_tk)
        {
            currentNode->child1 = newNode("Identifier");
            currentNode->child1->node_tk = t;
            t = scanner(input, t.lineNum);
            if(t.tokenId == DELIM_tk && t.tokenInstance == ":")
            {
                t = scanner(input,t.lineNum);
                if(t.tokenId == NUM_tk)
                {
                    currentNode->child2 = newNode("Integer");
                    currentNode->child2->node_tk = t;
                    t = scanner(input,t.lineNum);
                } else {
                    error("NUM_tk","");
                }
            } else {
                error("DELIM_tk",":");
            }
        }else {
            error("IDENT_tk","");
        }

        currentNode->child3 = vars(input);
        return currentNode;
    }
    return currentNode;
}

node* expr(ifstream& input)     //Expr function
{
    node *currentNode = newNode("<expr>");
    currentNode->child1 = A(input);

    if(t.tokenId == ARITH_tk && t.tokenInstance == "+")
    {
        currentNode->child2 = newNode("Operator");
        currentNode->child2->node_tk = t;
        t = scanner(input, t.lineNum);
        currentNode->child3 = expr(input);
        return currentNode;
    }
    return currentNode;
}

node* A(ifstream& input)        //A function
{
    node *currentNode = newNode("<A>");
    currentNode->child1 = N(input);

    if(t.tokenId == ARITH_tk && t.tokenInstance == "-")
    {
        currentNode->child2 = newNode("Operator");
        currentNode->child2->node_tk = t;
        t = scanner(input, t.lineNum);
        currentNode->child3 = A(input);
        return currentNode;
    }
    return currentNode;
}

node* N(ifstream& input)        //N function
{
    node *currentNode = newNode("<N>");
    currentNode->child1 = M(input);

    if(t.tokenId == ARITH_tk && t.tokenInstance == "/")
    {
        currentNode->child2 = newNode("Operator");
        currentNode->child2->node_tk = t;
        t = scanner(input, t.lineNum);
        currentNode->child3 = N(input);
        return currentNode;
    } else if(t.tokenId == ARITH_tk && t.tokenInstance == "*")
    {
        currentNode->child2 = newNode("Operator");
        currentNode->child2->node_tk = t;
        t = scanner(input, t.lineNum);
        currentNode->child3 = N(input);
        return currentNode;
    }
    return currentNode;
}

node* M(ifstream& input)        //M function
{
    node *currentNode = newNode("<M>");
    if(t.tokenId == ARITH_tk && t.tokenInstance == "-")
    {
        currentNode->child1 = newNode("Operator");
        currentNode->child1->node_tk = t;
        t = scanner(input, t.lineNum);
        currentNode->child2 = M(input);
        return currentNode;
    } else if((t.tokenId == PAREN_tk && t.tokenInstance == "[") || (t.tokenId == IDENT_tk) || (t.tokenId == NUM_tk))
    {
        currentNode->child1 = R(input);
        return currentNode;
    } else {
        error("ARITH_tk","-","PAREN_tk","[","IDENT_tk","NUM_tk");
    }
    return currentNode;
}

node* R(ifstream& input)        //R function
{
    node *currentNode = newNode("<R>");
    if(t.tokenId == PAREN_tk && t.tokenInstance == "[")
    {
        t = scanner(input, t.lineNum);
        currentNode->child1 = expr(input);
        if(t.tokenId == PAREN_tk && t.tokenInstance == "]")
        {
            t = scanner(input, t.lineNum);
            return currentNode;
        } else {
            error("PAREN_tk","]");
        }
    } else if(t.tokenId == IDENT_tk)
    {
        currentNode->child1 = newNode("Identifier");
        currentNode->child1->node_tk = t;
        t = scanner(input, t.lineNum);
        return currentNode;
    } else if(t.tokenId == NUM_tk)
    {
        currentNode->child1 = newNode("Integer");
        currentNode->child1->node_tk = t;
        t = scanner(input, t.lineNum);
        return currentNode;
    } else {
        error("PAREN_tk","[","","","IDENT_tk","NUM_tk");
    }
    return currentNode;
}

node* stats(ifstream& input)        //Stats function
{
    node *currentNode = newNode("<stats>");
    currentNode->child1 = stat(input);

    if(t.tokenId == DELIM_tk && t.tokenInstance == ";")
    {
        t = scanner(input,t.lineNum);
    } else {
        error("DELIM_tk",";");
    }

    currentNode->child2 = mStat(input);
    return currentNode;
}

node* mStat(ifstream& input)        //MStat function
{
    node *currentNode = newNode("<mStat>");
    if((t.tokenId == KW_tk && (t.tokenInstance == "in" || t.tokenInstance == "out" || t.tokenInstance == "start"
    || t.tokenInstance == "cond" || t.tokenInstance == "iterate")) || t.tokenId == IDENT_tk)
    {
        currentNode->child1 = stat(input);

        if(t.tokenId == DELIM_tk && t.tokenInstance == ";")
        {
            t = scanner(input,t.lineNum);
        } else {
            error("DELIM_tk",";");
        }

        currentNode->child2 = mStat(input);
        return currentNode;
    }
    return currentNode;
}

node* stat(ifstream& input)         //Stat function
{
    node *currentNode = newNode("<stat>");
    if(t.tokenId == KW_tk && t.tokenInstance == "in")
    {
        currentNode->child1 = in(input);
        return currentNode;
    } else if(t.tokenId == KW_tk && t.tokenInstance == "out")
    {
        currentNode->child1 = out(input);
        return currentNode;
    }else if(t.tokenId == KW_tk && t.tokenInstance == "start")
    {
        currentNode->child1 = block(input);
        return currentNode;
    }else if(t.tokenId == KW_tk && t.tokenInstance == "cond")
    {
        currentNode->child1 = if2(input);
        return currentNode;
    }else if(t.tokenId == KW_tk && t.tokenInstance == "iterate")
    {
        currentNode->child1 = loop(input);
        return currentNode;
    }else if(t.tokenId == IDENT_tk)
    {
        currentNode->child1 = assign(input);
        return currentNode;
    }else {
        error("KW_tk","in/out/start/cond/iterate","IDENT_tk","");
    }
    return currentNode;
}

node* in(ifstream& input)       //In function
{
    node *currentNode = newNode("<in>");
    if(t.tokenId == KW_tk && t.tokenInstance == "in")
    {
        t = scanner(input,t.lineNum);
        if(t.tokenId == IDENT_tk)
        {
            currentNode->child1 = newNode("Identifier");
            currentNode->child1->node_tk = t;
            t = scanner(input,t.lineNum);
            return currentNode;
        } else {
            error("IDENT_tk","");
        }
    } else {
        error("KW_tk","in");
    }
    return currentNode;
}

node* out(ifstream& input)      //Out function
{
    node *currentNode = newNode("<out>");
    if(t.tokenId == KW_tk && t.tokenInstance == "out")
    {
        t = scanner(input,t.lineNum);
        currentNode->child1 = expr(input);
        return currentNode;
    } else {
        error("KW_tk","out");
    }
    return currentNode;
}

node* if2(ifstream& input)          //If function
{
    node *currentNode = newNode("<if2>");
    if(t.tokenId == KW_tk && t.tokenInstance == "cond")
    {
        t = scanner(input,t.lineNum);

        if(t.tokenId == PAREN_tk && t.tokenInstance == "(")
        {
            t = scanner(input,t.lineNum);
            if(t.tokenId == PAREN_tk && t.tokenInstance == "(")
            {
                t = scanner(input,t.lineNum);
                currentNode->child1 = expr(input);
                currentNode->child2 = RO(input);
                currentNode->child3 = expr(input);
                if(t.tokenId == PAREN_tk && t.tokenInstance == ")")
                {
                    t = scanner(input,t.lineNum);
                    if(t.tokenId == PAREN_tk && t.tokenInstance == ")")
                    {
                        t = scanner(input,t.lineNum);
                        currentNode->child4 = stat(input);
                        return currentNode;
                    } else {
                        error("PAREN_tk",")");
                    }
                } else {
                    error("PAREN_tk",")");
                }
            } else {
                error("PAREN_tk","(");
            }
        } else {
            error("PAREN_tk","(");
        }
    }
    return currentNode;
}

node* loop(ifstream& input)         //Loop function
{
    node *currentNode = newNode("<loop>");
    if(t.tokenId == KW_tk && t.tokenInstance == "iterate")
    {
        t = scanner(input,t.lineNum);
        if(t.tokenId == PAREN_tk && t.tokenInstance == "(")
        {
            t = scanner(input,t.lineNum);
            if(t.tokenId == PAREN_tk && t.tokenInstance == "(")
            {
                t = scanner(input,t.lineNum);
                currentNode->child1 = expr(input);
                currentNode->child2 = RO(input);
                currentNode->child3 = expr(input);
                if(t.tokenId == PAREN_tk && t.tokenInstance == ")")
                {
                    t = scanner(input,t.lineNum);
                    if(t.tokenId == PAREN_tk && t.tokenInstance == ")")
                    {
                        t = scanner(input,t.lineNum);
                        currentNode->child4 = stat(input);
                        return currentNode;
                    } else {
                        error("PAREN_tk",")");
                    }
                } else {
                    error("PAREN_tk",")");
                }
            } else {
                error("PAREN_tk","(");
            }
        } else {
            error("PAREN_tk","(");
        }
    }
    return currentNode;
}

node* assign(ifstream& input)       //Assign function
{
    node *currentNode = newNode("<assign>");
    if(t.tokenId == IDENT_tk)
    {
        currentNode->child1 = newNode("Identifier");
        currentNode->child1->node_tk = t;
        t = scanner(input,t.lineNum);
        if(t.tokenId == RELAT_tk && t.tokenInstance == "<")
        {
            t = scanner(input,t.lineNum);
            if(t.tokenId == RELAT_tk && t.tokenInstance == "<")
            {
                t = scanner(input,t.lineNum);
                currentNode->child2 = expr(input);
                return currentNode;
            } else {
                error("RELAT_tk","<");
            }
        } else {
            error("RELAT_tk","<");
        }
    } else {
        error("IDENT_tk","");
    }
    return currentNode;
}

node* RO(ifstream& input)       //RO function
{
    node *currentNode = newNode("<RO>");
    if(t.tokenId == RELAT_tk && t.tokenInstance == "<")
    {
        currentNode->child1 = newNode("<");
        t = scanner(input,t.lineNum);
        if(t.tokenId == RELAT_tk && t.tokenInstance == "<")
        {
            currentNode->child1->label = "<<";
            t = scanner(input,t.lineNum);
            return currentNode;
        } else if(t.tokenId == RELAT_tk && t.tokenInstance == ">")
        {
            currentNode->child1->label = "<>";
            t = scanner(input,t.lineNum);
            return currentNode;
        }
    } else if(t.tokenId == RELAT_tk && t.tokenInstance == ">")
    {
        currentNode->child1 = newNode(">");
        t = scanner(input,t.lineNum);
        if(t.tokenId == RELAT_tk && t.tokenInstance == ">")
        {
            currentNode->child1->label = ">>";
            t = scanner(input,t.lineNum);
            return currentNode;
        }
    } else if(t.tokenId == ASSIGN_tk && t.tokenInstance == "=")
    {
        currentNode->child1 = newNode("=");
        t = scanner(input,t.lineNum);
    } else {
        error("RELAT_tk","<\" or \">","ASSIGN_tk","=");
    }
    return currentNode;
}

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <fstream>
using namespace std;
#include "node.h"
#include "scanner.h"
#include "generateASM.h"


/*
Author: Scott Tabaka
CMPSCI 4280 Project4
*/

/*
    Problems:
    1. Code generation test file 11 has problem(should be 1-9 as written not 1-10)
    2. Both local test files has semicolon problems(easy fix)
 */

static string filename;                             //For deleting output file if there is an error
static int labelCount=0;                            //Stores the current label count to create unique label names
static int varCountTemp=0;                          //Stores the current temporary variable count to create unique temporary variable names
static int varCount=0;                              //Stores the current program variable count to create unique program variable names
typedef enum {VAR, TEMP, LABEL} nameType;           //Gives a numeric value to each type of argument for use in creating unique variable/label names
vector <string> tempVars;                           //Stores all temporary variables for printing temporary variable declaration at the end of a asm file
vector<pair<string,int>> varList;                   //Stores all program variables for printing program variable declaration with values at the end of a asm file
vector<vector<pair<string,int>>> scopeStack(20);    //2d Vector of pairs for scoping each level of the stack --ScopeLevel,ScopeStack,Variable ID & Value--

static string newTempVar(nameType what, int value)  //Function that creates a unique (program/temporary)variable/label name
{
    string name;
    if(what == VAR)
    {
        name = "V" + to_string(varCount);           //Creates a new program variable name
        varList.push_back(make_pair(name,value));
        varCount++;
    } else if(what == TEMP)
    {
        name = "T" + to_string(varCountTemp);       //Creates a new temporary variable name
        tempVars.push_back(name);
        varCountTemp++;
    } else {
        name = "L" + to_string(labelCount);         //Creates a new label name
        labelCount++;
    }
    return name;
}

enum switchValue {                      //Give a numeric value to variables for use in switch
    other,
    program,
    block,
    vars,
    expr,
    A,
    N,
    M,
    R,
    stats,
    mStat,
    stat,
    in,
    out,
    if2,
    loop,
    assign,
    RO,
    Identifier,
    Integer,
    Operator
};

static map<string,switchValue> switchMap;   //Map declaration

void loadSwitchMap()        //Function that maps strings to a enum value for use in switch
{
    switchMap["<other>"] = other;
    switchMap["<program>"] = program;
    switchMap["<block>"] = block;
    switchMap["<vars>"] = vars;
    switchMap["<expr>"] = expr;
    switchMap["<A>"] = A;
    switchMap["<N>"] = N;
    switchMap["<M>"] = M;
    switchMap["<R>"] = R;
    switchMap["<stats>"] = stats;
    switchMap["<mStat>"] = mStat;
    switchMap["<stat>"] = stat;
    switchMap["<in>"] = in;
    switchMap["<out>"] = out;
    switchMap["<if2>"] = if2;
    switchMap["<loop>"] = loop;
    switchMap["<assign>"] = assign;
    switchMap["<RO>"] = RO;
    switchMap["Identifier"] = Identifier;
    switchMap["Integer"] = Integer;
    switchMap["Operator"] = Operator;
}

void errorStatSem1(token t, ofstream &output)    //error if variable is already declared within same scope
{
    cout << "\nSTATIC SEMANTIC ERROR: " << tokenNames[t.tokenId] << " \"" << t.tokenInstance << "\" on line number "
    << t.lineNum << " already declared within this scope.\n";

    output.close();
    remove(filename.c_str());           //deletes output file
    exit(1);
}

void errorStatSem2(token t, ofstream &output)     //error if variable is not declared before use
{
    cout << "\nSTATIC SEMANTIC ERROR: " << tokenNames[t.tokenId] << " \"" << t.tokenInstance << "\" on line "
    << t.lineNum << " not declared.\n";

    output.close();
    remove(filename.c_str());           //deletes output file
    exit(1);
}

void generateASM(node *currentNode, int stack_lvl, ofstream &output, string outputFilename)       //main program for checking static semantics and asm code generation called from main
{
    filename = outputFilename;      //Stores the output filename into a global variable for use with errors
    loadSwitchMap();                //Calls function that maps strings to a enum value for use in switch
    generateCode(currentNode, stack_lvl, output);   //First call to generateCode
}

static void generateCode(const node *currentNode, int scope_lvl, ofstream &output)   //subprogram for checking static semantics
{
    int stackLocation;
    string tempVar;     //temporary variable
    string tempLabel;   //temporary label for if and loop
    string tempLabel2;  //second temporary label for loop

    if (currentNode == NULL)
    {
        return;
    } else {
        switch (switchMap[currentNode->label])      //switch depending on which label is present in current node being traversed
        {
            case program:
                switchTraversalPrint(currentNode);  //For debugging

                generateCode(currentNode->child1, scope_lvl, output);   //<vars>
                generateCode(currentNode->child2, scope_lvl, output);   //<block>
                emptyStack(scope_lvl, output);                      //empty the final level of variables from scopeStack and asm stack
                output << "STOP " << endl;
                for (int i = 0; i < varList.size(); i++)       //outputs all of the program variables at the end of the .asm file
                {
                    output << varList[i].first << " " << varList[i].second << endl;
                }
                for (int i = 0; i < tempVars.size(); i++)       //outputs all of the temporary variables at the end of the .asm file
                {
                    output << tempVars[i] << " 0" << endl;
                }
                break;
            case block:
                switchTraversalPrint(currentNode);

                scope_lvl++;
                generateCode(currentNode->child1, scope_lvl, output);   //<vars>
                generateCode(currentNode->child2, scope_lvl, output);   //<stats>
                emptyStack(scope_lvl, output);          //empty the current level of variables from scopeStack and asm stack
                break;
            case vars:
                switchTraversalPrint(currentNode);
                if(currentNode->child1 != NULL)             //if not empty
                {
                    declareVars(currentNode, scope_lvl, output);   //function to push variables into scopeStack and onto the asm stack
                }

                generateCode(currentNode->child3, scope_lvl, output);   //<vars>

                break;
            case expr:
                switchTraversalPrint(currentNode);

                if (currentNode->child2 == NULL)    //<A>
                {
                    generateCode(currentNode->child1, scope_lvl, output);   //<A>
                } else {                            //<A>+<expr>
                    generateCode(currentNode->child3, scope_lvl, output);   //<expr>
                    tempVar = newTempVar(TEMP,0);
                    output << "STORE " << tempVar << endl;
                    generateCode(currentNode->child1, scope_lvl, output);   //<A>
                    output << "ADD " << tempVar << endl;
                }
                break;
            case A:
                switchTraversalPrint(currentNode);

                if (currentNode->child2 == NULL)    //<N>
                {
                    generateCode(currentNode->child1, scope_lvl, output);   //<N>
                } else {                            //<N>-<A>
                    generateCode(currentNode->child3, scope_lvl, output);   //<A>
                    tempVar = newTempVar(TEMP,0);
                    output << "STORE " << tempVar << endl;
                    generateCode(currentNode->child1, scope_lvl, output);   //<N>
                    output << "SUB " << tempVar << endl;
                }
                break;
            case N:
                switchTraversalPrint(currentNode);

                if (currentNode->child2 == NULL)
                {
                    generateCode(currentNode->child1, scope_lvl, output);   //<M>
                } else {                                                    //<M>/<N> or <M>*<N>
                    if(currentNode->child2->node_tk.tokenInstance == "/")   //<M>/<N>
                    {
                        generateCode(currentNode->child3, scope_lvl, output);   //<N>
                        tempVar = newTempVar(TEMP,0);
                        output << "STORE " << tempVar << endl;
                        generateCode(currentNode->child1, scope_lvl, output);   //<M>
                        output << "DIV " << tempVar << endl;
                    } else {                                                    //<M>*<N>
                        generateCode(currentNode->child3, scope_lvl, output);   //<N>
                        tempVar = newTempVar(TEMP,0);
                        output << "STORE " << tempVar << endl;
                        generateCode(currentNode->child1, scope_lvl, output);   //<M>
                        output << "MULT " << tempVar << endl;
                    }
                }
                break;
            case M:
                switchTraversalPrint(currentNode);

                if (currentNode->child1->label == "Operator")               // -<M>
                {
                    generateCode(currentNode->child2, scope_lvl, output);   //<M>
                    output << "MULT -1" << endl;                            //negates M
                } else {
                    generateCode(currentNode->child1, scope_lvl, output);   //<R>
                }
                break;
            case R:
                switchTraversalPrint(currentNode);
                checkDeclaration(currentNode->child1, scope_lvl, output);   //function to check if variable has been declared or not

                if(currentNode->child1->label == "Identifier")              //Identifier
                {
                    stackLocation = findStackLocation(currentNode->child1->node_tk.tokenInstance, scope_lvl);     //finds location in asm stack
                    output << "STACKR " << stackLocation << endl;
                } else if(currentNode->child1->label == "Integer")          //Integer
                {
                    token t = currentNode->child1->node_tk;                 //Stores integer token into temp token
                    output << "LOAD " << stoi(t.tokenInstance) << endl;
                } else {                                                    //[<expr>]
                    generateCode(currentNode->child1, scope_lvl, output);
                }
                break;
            case stats:
                switchTraversalPrint(currentNode);

                generateCode(currentNode->child1, scope_lvl, output);   //<stat>
                generateCode(currentNode->child2, scope_lvl, output);   //<mStat>
                break;
            case mStat:
                switchTraversalPrint(currentNode);

                generateCode(currentNode->child1, scope_lvl, output);   //<stat>
                generateCode(currentNode->child2, scope_lvl, output);   //<mStat>
                break;
            case stat:
                switchTraversalPrint(currentNode);

                generateCode(currentNode->child1, scope_lvl, output);   //<in>,<out>,<block>,<if>,<loop>,<assign>
                break;
            case in:
                switchTraversalPrint(currentNode);
                checkDeclaration(currentNode->child1, scope_lvl, output);   //function to check if variable has been declared or not

                stackLocation = findStackLocation(currentNode->child1->node_tk.tokenInstance, scope_lvl);     //finds location in asm stack
                tempVar = newTempVar(TEMP,0);
                output << "READ " << tempVar << endl;
                output << "LOAD " << tempVar << endl;
                output << "STACKW " << stackLocation << endl;
                break;
            case out:
                switchTraversalPrint(currentNode);

                generateCode(currentNode->child1, scope_lvl, output);   //<expr>
                tempVar = newTempVar(TEMP,0);
                output << "STORE " << tempVar << endl;
                output << "WRITE " << tempVar << endl;
                break;
            case if2:
                switchTraversalPrint(currentNode);

                generateCode(currentNode->child3, scope_lvl, output);   //<expr>
                tempVar = newTempVar(TEMP,0);
                output << "STORE " << tempVar << endl;
                generateCode(currentNode->child1, scope_lvl, output);   //<expr>
                output << "SUB " << tempVar << endl;
                tempLabel = newTempVar(LABEL,0);
                if(currentNode->child2->child1->label == ">>")          // <RO> is >=
                {
                    output << "BRNEG " << tempLabel << endl;
                } else if(currentNode->child2->child1->label == ">")    // <RO> is >
                {
                    output << "BRZNEG " << tempLabel << endl;
                } else if(currentNode->child2->child1->label == "<<")   // <RO> is <=
                {
                    output << "BRPOS " << tempLabel << endl;
                } else if(currentNode->child2->child1->label == "<")    // <RO> is <
                {
                    output << "BRZPOS " << tempLabel << endl;
                } else if(currentNode->child2->child1->label == "<>")   // <RO> is !=
                {
                    output << "BRZERO " << tempLabel << endl;
                } else {                                                // <RO> is =
                    output << "BRNEG " << tempLabel << endl;
                    output << "BRPOS " << tempLabel << endl;
                }

                generateCode(currentNode->child4, scope_lvl, output);   //<stat>
                output << tempLabel << ": NOOP" << endl;
                break;
            case loop:
                switchTraversalPrint(currentNode);

                tempLabel = newTempVar(LABEL,0);
                output << tempLabel << ": NOOP" << endl;
                generateCode(currentNode->child3, scope_lvl, output);   //<expr>
                tempVar = newTempVar(TEMP,0);
                output << "STORE " << tempVar << endl;
                generateCode(currentNode->child1, scope_lvl, output);   //<expr>
                output << "SUB " << tempVar << endl;
                tempLabel2 = newTempVar(LABEL,0);
                if(currentNode->child2->child1->label == ">>")          // <RO> is >=
                {
                    output << "BRNEG " << tempLabel2 << endl;
                } else if(currentNode->child2->child1->label == ">")    // <RO> is >
                {
                    output << "BRZNEG " << tempLabel2 << endl;
                } else if(currentNode->child2->child1->label == "<<")   // <RO> is <=
                {
                    output << "BRPOS " << tempLabel2 << endl;
                } else if(currentNode->child2->child1->label == "<")    // <RO> is <
                {
                    output << "BRZPOS " << tempLabel2 << endl;
                } else if(currentNode->child2->child1->label == "<>")   // <RO> is !=
                {
                    output << "BRZERO " << tempLabel2 << endl;
                } else {                                                // <RO> is =
                    output << "BRNEG " << tempLabel2 << endl;
                    output << "BRPOS " << tempLabel2 << endl;
                }

                generateCode(currentNode->child4, scope_lvl, output);   //<stat>
                output << "BR " << tempLabel << endl;                   //label if continuing loop
                output << tempLabel2 << ": NOOP" << endl;               //label if breaking out of loop
                break;
            case assign:
                switchTraversalPrint(currentNode);
                checkDeclaration(currentNode->child1, scope_lvl, output);   //function to check if variable has been declared or not

                generateCode(currentNode->child2, scope_lvl, output);
                stackLocation = findStackLocation(currentNode->child1->node_tk.tokenInstance, scope_lvl);     //finds location in asm stack
                output << "STACKW " << stackLocation << endl;
                break;
            case RO:
                switchTraversalPrint(currentNode);
                break;
            case Identifier:
                switchTraversalPrint(currentNode);
                break;
            case Integer:
                switchTraversalPrint(currentNode);
                break;
            case Operator:
                switchTraversalPrint(currentNode);
                break;
            case other:
                cout << currentNode->label;
            default:
                break;
        }
//        debugDisplay(currentNode, scope_lvl);
    }
}

void switchTraversalPrint(const node *currentNode)      //For debugging switch traversals
{
//    cout << currentNode->label << "!";
//
//    if(currentNode->label == "Identifier" || currentNode->label == "Integer" || currentNode->label == "Operator")
//    {
//        cout << currentNode->node_tk.tokenInstance;
//    }
}

int find(string s, int level)   //Finds what scope level the string is found and returns it
{
    for(;level >= 0;level--)    //iterates through all scopeLevels
    {
        if(verify(s, level))    //function call to check if an element exists within the scopeStack
        {
            return level;       //if found returns the level the string was found
        }
    }
    return -1;                  //if string is not found within the scopeStack
}

int findStackLocation(string s, int level)   //Finds where in the "asm stack" the string is found
{
    int elementLocation;
    int elementCount = countElements(level);    //Counts how many elements are in all of the scope stacks

    for(;level >= 0;level--)                    //iterates through all scopeLevels
    {
        for(int i=0;i < scopeStack[level].size();i++)   //iterate through all elements in a scopeStack
        {
            if (scopeStack[level][i].first == s)    //If new string is found in the stack of current scope.
            {
                elementLocation = (i + countElements(level-1)); //Calculates the element location of the string (Treats it like one big array)
                return (elementCount-elementLocation-1);    //Calculates the asm stack location from the elementLocation and elementCount
            }
        }
    }
    return -1;
}

int countElements(int level)    //Counts all of elements in the scopeStack
{
    int count = 0;
    for(int i=0;i <= (level);i++)       //iterates through all scope levels
    {
        count += scopeStack[i].size();  //adds the current scopeStack size to a variable
    }
    return count;
}

bool verify(string s, int level)        //Verifies if string is within the current scope
{
    for(int i=0;i < scopeStack[level].size();i++)
    {
        if (scopeStack[level][i].first == s)    //If new string is found in the stack of current scope.
        {
            return true;
        }
    }
    return false;
}

void declareVars(const node *currentNode, int level, ofstream &output)   //function that declares a variable in a scope
{
    string tempVar;
    if (currentNode == NULL)
    {
        return;
    } else {
        token idtk = currentNode->child1->node_tk;      //Child 1 of vars is Identity
        token numtk = currentNode->child2->node_tk;     //Child 2 of vars is Integer

        if(!verify(idtk.tokenInstance, level))      //check to make sure variable is not present in current scope
        {
            scopeStack[level].push_back(make_pair(idtk.tokenInstance,stoi(numtk.tokenInstance)));   //Insert id & int into a pair for scope
            output << "PUSH" << endl;                   //Prints PUSH for asm push to stack
            tempVar = newTempVar(VAR,stoi(numtk.tokenInstance));
            output << "LOAD " << tempVar << endl;
            output << "STACKW " << 0 << endl;       //This will write then initial value for the variable into the top of the stack
        } else {
            errorStatSem1(idtk, output);
        }
    }
}

void checkDeclaration(node *currentNode, int level, ofstream &output)      //function that checks if a variable has been declared
{
    if (currentNode == NULL || currentNode->label != "Identifier")
    {
        return;
    } else {
        token t = currentNode->node_tk;

        if(find(t.tokenInstance, level) < 0)    //Calls find function to make sure string exists somewhere in the scopeStack
        {
            errorStatSem2(t, output);
        }
    }
}

void emptyStack(int level, ofstream &output)   //function that pops all variables in a scope
{
    for(int i=0;i<scopeStack[level].size();i++)     //iterates through each element in the scopeStack level
    {
        output << "POP" << endl;        //prints "POP" to the asm file
    }
    scopeStack[level].clear();          //clears all of the elements from the scopeStack level
}

void displayScopeStack(int level)   //For debugging scopeStack
{
    for(;level >= 0;level--)
    {
        cout << "scope level:" << level << "--> ";

        for(int i=0;i<scopeStack[level].size();i++)
        {
            cout << scopeStack[level][i].first << ":" << scopeStack[level][i].second << " ";
        }

//        cout << "Found:" << find("x2",level);    //display what level the string is found on
        cout << endl;
    }
}

void debugDisplay(const node *currentNode, int level)   //debug display of nodes
{
    token t = currentNode->node_tk;

    if (t.tokenInstance != "")
    {
        cout << "Token ID: " << tokenNames[t.tokenId] << " \"" << t.tokenInstance << "\"";
    }
    cout << endl;
    displayScopeStack(level);
}

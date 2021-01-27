#include <iostream>
#include <fstream>
using namespace std;
#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "node.h"
#include "testTree.h"
#include "generateASM.h"


/*
Author: Scott Tabaka
CMPSCI 4280 Project4

Purpose: This program uses the scanner and parser built in P1 and P2 to create a tree of the program in the input file.
 The tree is then sent into a function that checks the static semantics of the tree and also generates asm code that
 prints into an output file.
*/

string appendFS19(string s)             //adds .fs19 file extension if it does not exist
{
    string fileEnding = ".fs19";
    if(s.length() > fileEnding.length())
    {
        if (s.substr(s.length() - 5, 5).compare(fileEnding) != 0)
        {
            s.append(fileEnding);
        }
    }else{
        s.append(fileEnding);
    }
    return s;
}

string removeFileExtension(string s)         //removes .fs19 file extension if it exists
{
    if(s.length() > 5)
    {
        if (s.substr(s.length() - 5, 5).compare(".fs19") == 0)
        {
            s = s.substr(0,s.length() - 5);
        }
    }
    return s;
}

string appendASM(string s)             //adds .fs19 file extension if it does not exist
{
    string fileEnding = ".asm";
    if(s.length() > fileEnding.length())
    {
        if (s.substr(s.length() - 4, 4).compare(fileEnding) != 0)
        {
            s.append(fileEnding);
        }
    }else{
        s.append(fileEnding);
    }
    return s;
}

string changeFileExtension(string s)             //changes file extension from .fs19 to .asm
{
    s = removeFileExtension(s);
    s = appendASM(s);
    return s;
}

int main(int argc, char *argv[])
{
    if(argc > 2)                                //checks for correct amount of arguments
    {
        cout << "Usage Error: P1 <filename>" << endl;
        exit(1);
    }

    string inputFilename = "testfile66.fs19";
//    string inputFilename = "testfile2.fs19";
//    string inputFilename = "tf1.fs19";

    ofstream output;

//    string inputFilename;
//
//    if(argc == 1)       //if there is no command line argument given
//    {
//        inputFilename = "kb.fs19";
//        //ofstream output(inputFilename);
//        output.open(inputFilename);
//
//        cout << "Please enter valid inputs and press <ctrl+d> when finished\n";
//        output << string(istreambuf_iterator<char>(cin), {});           //inputs user input into the output file
//        cout << endl;
//
//        output.close();
//        output.clear();
//    }
//    else if (argc == 2)             //if command line argument is given
//    {
//        inputFilename = argv[1];        //use the file name provided by user
//        inputFilename = appendFS19(inputFilename); //append .fs19 if not present
//    }
//    else
//    {
//        cout << "Usage Error: P1 <filename>" << endl;		//display for proper usage
//        exit(1);
//    }

    ifstream input(inputFilename);                  //opens input file stream

    string outputFilename = changeFileExtension(inputFilename);

    if(input)
    {
        node *parseTree = parser(input);            //creates parse tree
//        treePrint(parseTree, 0);                    //print parse tree Preorder
//        cout << "\n---Separator---\n\n";
        output.open(outputFilename);                //opens output file stream
        generateASM(parseTree, 0, output, outputFilename);
        cout << "Generated output file --> " << outputFilename << endl;     //Display file created message
    }else{
        cout << "Error: Cannot open file -- " << inputFilename << endl;
    }
    input.close();
    output.close();

    return 0;
}
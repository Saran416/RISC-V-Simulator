/**
 * This file is the main entry point for the simulator. 
 * It takes input from the user and calls the appropriate functions from simulator.cpp
 * It provides an interface to the user to interact with the simulator.
 */

#include "simulator.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;


int main( int argc, char *argv[] )
{
    if (argc != 2)
    {
        cout << "Invalid number of arguments" << endl;
        return 0;
    }
    if(argv[1] == "run"){
        loadProgram("input.s");
        run();
    }
    else if(argv[1] == "step"){
        loadProgram("input.s");
        step();
    }
    
}



/*
Entry point of entire application
*/
// int main(  )
// {
    // while (1)
    // {
    //     string command;
    //     cin >> command;
    //     if (command == "load")
    //     {
    //         string file;
    //         cin >> file;
    //         loadProgram(file);
    //         cout << endl;
    //     }
    //     else if (command == "run") // run the entire code till end until a breakpoint is encountered
    //     {
    //         run();
    //         cout << endl;
    //     }
    //     else if (command == "regs") // print the registers
    //     {
    //         printRegs();
    //         cout << endl;
    //     }
    //     else if (command == "exit") // exit the simulator
    //     {
    //         cout << "Exited the simulator" << endl;
    //         break;
    //     }
    //     else if (command == "mem") // show memory contents
    //     {
    //         string input1;
    //         string input2;
    //         cin >> input1;
    //         cin >> input2;
    //         unsigned long address;
    //         address = stoul(input1, nullptr, 16);
    //         int count;
    //         count = stoi(input2);
    //         printMem(address, count);
    //         cout << endl;
    //     }
    //     else if (command == "step") // run line by line
    //     {
    //         step();
    //         cout << endl;
    //     }
    //     else if (command == "break") // add breakpoint
    //     {
    //         int lineNumber;
    //         cin >> lineNumber;
    //         addBreakpoint(lineNumber);
    //         cout << endl;
    //     }
    //     else if (command == "del") // remove breakpoint
    //     {
    //         string temp;
    //         cin >> temp;
    //         if (temp != "break")
    //         {
    //             cout << "Invalid command" << endl;
    //             cout << endl;
    //             continue;
    //         }
    //         cin >> temp;
    //         int lineNumber = stoi(temp);
    //         removeBreakpoint(lineNumber);
    //         cout << endl;
    //     }
    //     else if (command == "show-stack") // show the call stack
    //     {
    //         showStack();
    //         cout << endl;
    //     }
    //     else
    //     {
    //         cout << "Invalid command" << endl;
    //         cout << endl;
    //     }
    // }
    // return 0;
//}

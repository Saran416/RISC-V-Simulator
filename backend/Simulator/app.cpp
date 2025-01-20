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


int main(int argc, char *argv[])
{
    string filename = "";
    cache* newCache;
    bool cacheEnabled = true;
    string cacheConfig = "config.txt";
    newCache = enableCache(cacheConfig);
    // we see the first argument and if it is "run" we run the program till the end
    if (string(argv[1]) == "run")
    {
        loadProgram("input.s");
        updateStatus(stoi(argv[2]),cacheEnabled,newCache);
        run(true,cacheEnabled,newCache);
    }
    
    else if (string(argv[1]) == "step")
    {
        loadProgram("input.s");
        updateStatus(stoi(argv[2]),cacheEnabled,newCache);
        step(true,cacheEnabled,newCache);
    }
}

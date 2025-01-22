#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "cache_simulator.h"

using namespace std;

/*
    Prints the registers
*/
void printRegs();

/*
    Performs all the tasks before starting the execution
*/
bool loadProgram(string file);

/*
    Runs the entire code starting from the current PC
*/
void run(bool flag, bool cacheEnabled, cache *newCache);

/*
    Step by step execution after the execution is stopped by breakpoint or from the start itself
*/
void step(bool flag, bool cacheEnabled, cache *newCache);

void updateStatus(int pc, bool cacheEnabled, cache *newCache);

void setPc(int pc);

/*
    Prints memory from the given address and count
*/
void printMem(unsigned long address, int count);

/*
    Adds breakpoint at the given line
*/
void addBreakpoint(int lineNumber);

/*
    Removes breakpoint at the given line
*/
void removeBreakpoint(int lineNumber);

/*
    Prints the call stack
*/
void showStack();

void printCacheRes(cache *newCache);

void changeCacheConfigFile(string file);
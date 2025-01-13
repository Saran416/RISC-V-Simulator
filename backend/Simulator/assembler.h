#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
using namespace std;


/*
    Function to print the vector of strings
    params: {vector<string>} v
    return: {void}
*/
void printVector(vector<string> v);

/*
    Function to check if a string can be safely and 
    entirely converted into an integer.
    params: {string} s, {int} line, {int} base
    return: {bool}
*/
bool safeStoi(string s, int line, int base=10);

/*
    Function to convert a binary 32 bit string to a hexadecimal string
    which is the final instruction code
    params: {string} bin
    returnL {string}
*/
string binToHex(string bin);


/*
    Function to convert a hexadecimal string to an integer 
    and returns error if the string is not a valid hexadecimal
    params: {string} hex, {int} line
    return: {int}
*/
int hexToInt(string hex, int line);

/*
    Function to convert a binary string to an integer
    and returns error if the string is not a valid binary
    params: {string} bin, {int} line
    return: {int}
*/
int binToInt(string bin, int line);

/*
    Function to check if a register is within the bounds of 0-31
    and returns error if it is not
    params: {int} reg, {int} line
    return: {bool}
*/
bool checkRegister(int reg, int line);

/*
    Function to get the register number by the number part of the argument
    converting it to an interger or by checking if it is in the alias 
    map or not.
    params: {string} reg, {unordered_map<string, string>} alias, {int} line
    return: {int}
*/
int getRegister(string reg, unordered_map<string, string> alias, int line);

/*
    Function to get all the arguments from the args string
    and returns a vector of strings and a boolean value
    which is true if there is an error in the arguments
    params: {int} line, {int} count, {string} args, {bool} flag
    return: {pair<vector<string>, bool>}
*/
pair<vector<string>, bool> getArguments(int line, int count, string args, bool flag);

pair<int, bool> getImmediate(string str, int pc, unordered_map<string, int> label, bool flag);

int convert();

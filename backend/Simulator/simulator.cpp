/**
 * This file contains implementations of backend simulator functions that
 * are used by app.cpp to provide output to the user
 */

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <stack>
#include <math.h>
#include "simulator.h"

using namespace std;

long int registers[32]; // 32 registers
unsigned long memsize = 0x50000;
unsigned char memory[0x50000];   // byte addressable memory
vector<pair<int, string> > lines; // stores the pc and the line
int mainPC = 0;
stack<pair<string, int> > st;          // stores the function name and the previous pc value
unordered_map<int, bool> breakpoints; // stores breakpoint status for each line
unordered_map<std::string, std::string> opcode;
unordered_map<int, int> labelIndex;
unordered_map<int, string> inverseLabel;
unordered_map<string, string> alias;
unordered_map<int, int> comments; // stores the pc and number and index where the comment starts
unordered_map<string, int> label; // stores all the labels and their corresponding pc values
bool funcCall = false;            // stores whether a function call is made or not and is changed after use
bool funcReturn = false;          // stores whether a function return is made or not and is changed after use
int memLines = 0;                 // number of lines for .data section (includes one line for .text )
string fileName = "";

int timer = 0;

void setPc(int pc)
{
    mainPC = pc;
}

/*
takes immediate in any form as input, find out whether hex, decimal or binary converts it into a long value accordingly and also return a bool that indicates if there is an error while converting it.
*/
pair<long, bool> getDec(string s)
{
    unsigned long pos;
    unsigned long imm;
    bool err = false;
    if (s[0] != '-')
    {
        if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) // hex
        {
            try
            {
                imm = stoul(s, &pos, 16);
                if (pos != s.size())
                {
                    err = true;
                }
            }
            catch (invalid_argument)
            {
                err = true;
            }
            catch (out_of_range)
            {
                err = true;
            }
        }
        else if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) // binary
        {
            try
            {
                imm = stoul(s, &pos, 2);
                if (pos != s.size())
                {
                    err = true;
                }
            }
            catch (invalid_argument)
            {
                err = true;
            }
            catch (out_of_range)
            {
                err = true;
            }
        }
        else // decimal
        {
            try
            {
                imm = stoul(s, &pos, 10);
                if (pos != s.size())
                {
                    err = true;
                }
            }
            catch (invalid_argument)
            {
                err = true;
            }
            catch (out_of_range)
            {
                err = true;
            }
        }
    }
    else // negative number in .data section
    {
        pair<long, bool> temp = getDec(s.substr(1, s.length() - 1));
        temp.first = -1 * temp.first;
        return temp;
    }
    return pair<long, bool>(imm, err);
}

/*
    To store .dword, .word, .half, .byte data in the memory from the .data section
*/
bool memHandle(string args, int size, unsigned long &address)
{
    vector<string> arguments;
    int prev = -1;
    for (int i = 0; i < args.length(); i++) // parser to separate the arguments
    {
        if (args[i] == ' ' || args[i] == ',')
        {
            prev++;
        }
        else if (i + 1 < args.length() && (args[i + 1] == ' ' || args[i + 1] == ','))
        {
            arguments.push_back(args.substr(prev + 1, i - prev));
            prev = i;
        }
        else if (i == args.length() - 1)
        {
            arguments.push_back(args.substr(prev + 1, i - prev + 1));
        }
    }
    if (arguments.size() < 1)
    {
        cout << "Invalid value in .data section" << endl;
        return false;
    }
    long int num = 0;
    for (int i = 0; i < arguments.size(); i++)
    {
        string curr = arguments[i];
        pair<long, bool> res = getDec(curr);
        if (res.second)
        {
            cout << "Invalid value in .data section" << endl;
            return false;
        }
        num = res.first;
        if (num > (pow(2, (size * 8)) - 1) || num < (-1 * pow(2, (size * 8 - 1))))
        {
            cout << "Value out of range in .data section" << endl;
            return false;
        }
        for (int i = 0; i < size; i++) // storing the value in memory
        {
            memory[address + i] = (num >> (i * 8)) & 0xff;
        }
        address += size;
    }
    return true;
}

/*
    Load the file into vector of lines
*/
pair<bool, int> loadFile(string inputFile)
{
    unsigned long baseAddress = 0x10000;
    int pc = 0;
    int dataLines = 0;
    ifstream file(inputFile);
    string line;
    bool isTextSection = true;
    while (getline(file, line))
    {
        if (line[0] == '\0')

        {
            pc += 4;
            lines.push_back(make_pair(pc, line));
            continue;
        }
        else if (line[0] == ';')
        {
            continue;
        }
        else if (line == ".data") // .data is encountered, so need to allocate memory
        {
            dataLines++;
            isTextSection = false;
            continue;
        }
        else if (line == ".text") // .text is encountered, so need to start storing the instructions
        {
            dataLines++;
            isTextSection = true;
            continue;
        }
        else if (isTextSection)
        {
            lines.push_back(make_pair(pc, line));
            pc += 4;
        }
        else if (line.length() >= 6 && line.substr(0, 6) == ".dword")
        {
            dataLines++;
            if (!memHandle(line.substr(7), 8, baseAddress))
                return make_pair(false, 0);
        }
        else if (line.length() >= 5 && line.substr(0, 5) == ".word")
        {
            dataLines++;
            if (!memHandle(line.substr(6), 4, baseAddress))
                return make_pair(false, 0);
        }
        else if (line.length() >= 5 && line.substr(0, 5) == ".half")
        {
            dataLines++;
            if (!memHandle(line.substr(6), 2, baseAddress))
                return make_pair(false, 0);
        }
        else if (line.length() >= 5 && line.substr(0, 5) == ".byte")
        {
            dataLines++;
            if (!memHandle(line.substr(6), 1, baseAddress))
                return make_pair(false, 0);
        }
        else
        {
            cout << "Invalid data type in .data section" << endl;
            return make_pair(false, 0);
        }
    }
    file.close();
    return make_pair(true, dataLines); // return true if file is loaded successfully
}

/*
    Function to get starting index of the comments from the input file
*/
void getComments(string file)
{
    int pc = 0;
    ifstream input(file);
    string line;
    while (getline(input, line)) // storing starting point of comments
    {
        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == ';' && i == 0)
            {
                break;
            }
            if (line[i] == ';')
            {
                comments[pc] = i;
                break;
            }
        }
        pc += 4;
    }
    input.close();
}

/*
    Function to map labels to the pc values
*/
bool getLabels(string file)
{
    int pc = 0; // program counter
    ifstream input(file);
    string line;
    bool isTextSection = true;
    while (getline(input, line))
    {
        string curr_label = "";
        if (line == ".text")
        {
            isTextSection = true;
            continue;
        }
        else if (line == ".data")
        {
            isTextSection = false;
            continue;
        }
        if (line[0] == ';')
        {
            continue;
        }
        if (comments.find(pc) != comments.end())
        {
            line = line.substr(0, comments[pc]);
        }
        if (!isTextSection)
        {
            continue;
        }
        while (line.length() > 0 && line[0] == ' ')
        {
            line = line.substr(1);
        }
        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == ':')
            {
                curr_label = line.substr(0, i);
                i++;
                while (i < line.length() && line[i] == ' ')
                {
                    i++;
                }
                labelIndex[pc] = i;
                break;
            }
        }
        if (curr_label != "")
        {
            if (label.find(curr_label) == label.end())
            {
                label[curr_label] = pc;
                inverseLabel[pc] = curr_label;
            }
            else
            {
                cout << "Line " << (pc / 4 + 1 + memLines) << ": Multiple Definitions for label" << endl;
                return false;
            }
        }
        pc += 4;
    }
    input.close();
    return true;
}

/*
    Function to initialise the maps for opcode, funct3, funct7, alias
*/
void initialiseMaps()
{
    inverseLabel[0] = "main";
    opcode["add"] = "0110011";
    opcode["sub"] = "0110011";
    opcode["and"] = "0110011";
    opcode["or"] = "0110011";
    opcode["xor"] = "0110011";
    opcode["sll"] = "0110011";
    opcode["srl"] = "0110011";
    opcode["sra"] = "0110011";
    opcode["addi"] = "0010011";
    opcode["andi"] = "0010011";
    opcode["ori"] = "0010011";
    opcode["xori"] = "0010011";
    opcode["slli"] = "0010011";
    opcode["srli"] = "0010011";
    opcode["srai"] = "0010011";
    opcode["lui"] = "0110111";
    opcode["auipc"] = "0010111";
    opcode["jal"] = "1101111";
    opcode["jalr"] = "1100111";
    opcode["beq"] = "1100011";
    opcode["bne"] = "1100011";
    opcode["blt"] = "1100011";
    opcode["bge"] = "1100011";
    opcode["bltu"] = "1100011";
    opcode["bgeu"] = "1100011";
    opcode["lw"] = "0000011";
    opcode["sw"] = "0100011";
    opcode["ld"] = "0000011";
    opcode["sd"] = "0100011";
    opcode["sh"] = "0100011";
    opcode["sb"] = "0100011";
    opcode["lh"] = "0000011";
    opcode["lb"] = "0000011";
    opcode["lbu"] = "0000011";
    opcode["lhu"] = "0000011";
    opcode["slt"] = "0110011";
    opcode["sltu"] = "0110011";
    opcode["slti"] = "0010011";
    opcode["sltiu"] = "0010011";
    opcode["lwu"] = "0000011";

    alias["zero"] = "x0";
    alias["ra"] = "x1";
    alias["sp"] = "x2";
    alias["gp"] = "x3";
    alias["tp"] = "x4";
    alias["t0"] = "x5";
    alias["t1"] = "x6";
    alias["t2"] = "x7";
    alias["s0"] = "x8";
    alias["fp"] = "x8";
    alias["s1"] = "x9";
    alias["a0"] = "x10";
    alias["a1"] = "x11";
    alias["a2"] = "x12";
    alias["a3"] = "x13";
    alias["a4"] = "x14";
    alias["a5"] = "x15";
    alias["a6"] = "x16";
    alias["a7"] = "x17";
    alias["s2"] = "x18";
    alias["s3"] = "x19";
    alias["s4"] = "x20";
    alias["s5"] = "x21";
    alias["s6"] = "x22";
    alias["s7"] = "x23";
    alias["s8"] = "x24";
    alias["s9"] = "x25";
    alias["s10"] = "x26";
    alias["s11"] = "x27";
    alias["t3"] = "x28";
    alias["t4"] = "x29";
    alias["t5"] = "x30";
    alias["t6"] = "x31";
}

/*
    Performs ALU operations for the R and I type instructions
*/
long int ALU(long int v1, long int v2, string instr)
{
    if (instr == "add" || instr == "addi")
    {
        return v1 + v2;
    }
    else if (instr == "sub")
    {
        return v1 - v2;
    }
    else if (instr == "and" || instr == "andi")
    {
        return v1 & v2;
    }
    else if (instr == "or" || instr == "ori")
    {
        return v1 | v2;
    }
    else if (instr == "xor" || instr == "xori")
    {
        return v1 ^ v2;
    }
    else if (instr == "sll" || instr == "slli")
    {
        return v1 << v2;
    }
    else if (instr == "srl" || instr == "srli")
    {
        return (unsigned long)v1 >> v2;
    }
    else if (instr == "sra" || instr == "srai")
    {
        return v1 >> v2;
    }
    else if (instr == "slt" || instr == "slti" || instr == "slt" || instr == "slti")
    {
        return v1 < v2;
    }
    else
    {
        return 0;
    }
}

/*
    A function to check if a string can be converted to integer or not
*/
bool safeStoi(string s, int base)
{
    try
    {
        size_t i = 0;
        bool isNegative = false;
        int st = stoi(s, &i, base);
        if (s[0] == '-')
        {
            isNegative = true;
        }
        if (i != s.length())
        {
            if (isNegative && i == s.length() - 3)
            {
                return true;
            }
            else
            {
                throw exception();
            }
        }
        return true;
    }
    catch (exception e)
    {
        return false;
    }
}

/*
    A function that converts decimal number into its hexadecimal form
*/
string decToHex(long int num)
{
    if (num == 0)
    {
        return "0";
    }

    string hex = "";
    unsigned long long num2 = (num < 0) ? (0xffffffffffffffff + num + 1) : num;

    while (num2)
    {
        int rem = num2 % 16;
        if (rem < 10)
        {
            hex = std::to_string(rem) + hex;
        }
        else
        {
            hex = (char)(rem - 10 + 'a') + hex;
        }
        num2 /= 16;
    }

    return hex;
}

/*
    A function to convert hexadecimal string to integer also
    considering the safe conversion and returning an error if
    the conversion is not possible
*/
pair<int, bool> hexToInt(string hex, int line)
{
    if (safeStoi(hex, 16))
    {
        return make_pair(stoi(hex, 0, 16), false);
    }
    else
    {
        return make_pair(0, true);
    }
}

/*
    A function to convert binary string to integer also
    considering the safe conversion and returning an error if
    the conversion is not possible
*/
pair<int, bool> binToInt(string bin, int line)
{
    if (safeStoi(bin, 2))
    {
        return make_pair(stoi(bin, 0, 2), false);
    }
    else
    {
        return make_pair(0, true);
    }
}

/*
    A function to check if the register is in the range of 0 to 31.
    It returns an error if register is out of bounds.
*/
bool checkRegister(int reg, int line)
{
    if (reg < 0 || reg > 31)
    {
        cout << "Line " << line << ": Register " << reg << " not found" << endl;
        return true;
    }
    return false;
}

/*
    A function to get the register number from the string
    It also checks if the register is in the range of 0 to 31
    and returns with an error if the register is not found.
*/
int getRegister(string reg, unordered_map<string, string> alias, int line)
{
    if (reg[0] == 'x')
    {
        if (safeStoi(reg.substr(1), 10))
        {
            return stoi(reg.substr(1));
        }
        else
        {
            cout << "Line " << (line) << ": Register " << reg << " not found" << endl;
            return -1;
        }
    }
    else
    {
        if (alias.find(reg) != alias.end() && safeStoi(alias[reg].substr(1), 10))
        {
            return stoi(alias[reg].substr(1));
        }
        else
        {
            cout << "Line " << (line) << ": Register " << reg << " not found" << endl;
            return -1;
        }
    }
}

/*
    A function to get the arguments from the string.
    It returns a pair of vector of strings and a boolean value.
    The boolean value is true if there is an error in the arguments.
    The vector of strings contains the arguments extracted from the args string.
*/
pair<vector<string>, bool> getArguments(int line, int count, string args, bool flag)
{
    vector<string> arguments;
    bool err = false;
    int index = 0, prev = -1, curr = 0, stackOpcounter = 0;
    stack<char> st;
    int pc = 4 * (line - 1) - 1;
    while (count != curr && index < args.length())
    {
        if (args[index] == ' ' || args[index] == ',' || args[index] == '(' || args[index] == ')')
        {
            if (args[index] == '(')
            {
                st.push('(');
                stackOpcounter++;
            }
            else if (args[index] == ')')
            {
                if (st.size() == 0)
                {
                    cout << "Line " << line << " : Mismanaged brackets" << endl;
                    err = true;
                    break;
                }
                if (st.size() > 1)
                {
                    cout << "Line " << line << ": Too many brackets, only one set allowed around one argument" << endl;
                    err = true;
                    break;
                }
                st.pop();
                stackOpcounter++;
            }
            prev++;
        }
        else if (index + 1 < args.length() && (args[index + 1] == ' ' || args[index + 1] == ',' || args[index + 1] == '(' || args[index + 1] == ')'))
        {
            arguments.push_back(args.substr(prev + 1, index - prev));
            prev = index;
            curr++;
        }
        else if (index == args.length() - 1 && count != curr)
        {
            arguments.push_back(args.substr(prev + 1, index - prev));
            curr++;
        }
        index++;
    }
    while (index < args.length() && (args[index] == ' ' || args[index] == ',' || args[index] == '(' || args[index] == ')'))
    {
        if (args[index] == '(')
        {
            st.push('(');
            stackOpcounter++;
        }
        else if (args[index] == ')')
        {
            stackOpcounter++;
            if (st.size() == 0)
            {
                cout << "Line " << line << " : Mismatching brackets" << endl;
                err = true;
                break;
            }
            if (st.size() > 1)
            {
                cout << "Line " << line << " : Too many brackets, only one set allowed around one argument" << endl;
                err = true;
                break;
            }
            st.pop();
        }
        index++;
    }
    if (args[index] == ')')
    {
        index++;
        if (st.size() == 0)
        {
            cout << "Line " << line << " : mismatching brackets" << endl;
            err = true;
        }
        st.pop();
        stackOpcounter++;
    }
    while (index < args.length() && (args[index] == ' ' || args[index] == ','))
        index++;
    if (st.size() != 0)
    {
        cout << "Line " << line << ": mismatching brackets" << endl;
        err = true;
    }
    if (arguments.size() != count)
    {
        cout << "Line " << line << ": Less arguments than required" << endl;
        err = true;
    }
    else if (arguments.size() == count && index < args.length())
    {
        cout << "Line " << line << ": Extra arguments" << endl;
        err = true;
    }
    return make_pair(arguments, err);
}

/*
    A function to get the immediate value from the string containing a number in dec, hex or binary.
    It returns a pair of integer and a boolean value.
    The boolean value is true if there is an error in the immediate value.
    The integer value is the immediate value extracted from the string.
*/
pair<int, bool> getImmediate(string str, int pc, unordered_map<string, int> label, bool flag)
{
    int imm, neg = 0;
    int line = pc / 4 + 1 + memLines;
    bool err = false;
    if (str[0] == '-')
    {
        neg = 1;
    }
    if ((str[0] >= 'A' || (neg && str[1] >= 'A')) && flag)
    {
        if (label.find(str) != label.end())
        {
            imm = (label[str] - pc);
            return make_pair(imm, true);
        }
        else
        {
            cout << "Line " << (line) << ": Label not found" << endl;
            err = true;
        }
    }
    else if ((str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) || (neg && str[1] == '0' && (str[2] == 'x' || str[2] == 'X')))
    {
        pair<int, bool> res = hexToInt(str, 16);
        if (res.second)
        {
            return make_pair(0, true);
        }
        else
        {
            imm = res.first;
        }
    }
    else if ((str[0] == '0' && (str[1] == 'b' || str[1] == 'B')) || (neg && str[1] == '0' && (str[2] == 'b' || str[2] == 'B')))
    {
        pair<int, bool> res = binToInt(str, 2);
        if (res.second)
        {
            return make_pair(0, true);
        }
        else
        {
            imm = res.first;
        }
    }
    else if (safeStoi(str, 10))
    {
        imm = stoi(str);
    }
    else
    {
        err = true;
    }
    return make_pair(imm, err);
}

string addZeroes(string hex, int num)
{
    string temp = "";
    for (int i = 0; i < num - hex.length(); i++)
    {
        temp += "0";
    }
    return temp + hex;
}

/*
    Performs tasks, manipulate the memory and register for the given instruction line
*/
pair<int, bool> convert(string line, int pc, bool step, bool cacheEnabled, cache *newCache)
{
    bool flag = false;
    int lineNum = pc / 4 + 1 + memLines;
    if (breakpoints.find(lineNum) != breakpoints.end() && breakpoints[lineNum] && !step)
    {
        cout << "Execution stopped at breakpoint" << endl;
        return make_pair(-2, flag);
    }
    if (line[0] == ';')
    { // starting with semicolon is treated as a comment
        return make_pair(0, flag);
    }
    if (comments.find(pc) != comments.end())
    {
        line = line.substr(0, comments[pc]);
    }
    string instr = "";
    string args = "";
    int prev = -1;
    int i = 0;

    while (line[i] == ' ' && i < line.length())
    {
        prev++;
        i++;
    }

    for (i; i < line.length(); i++) // extracting instruction and arguments
    {
        if (line[i] == ':' || line[i] == ' ' || line[i] == ',')
        {
            prev++;
        }
        else if (i + 1 < line.length() && line[i + 1] == ':')
        {
            prev = i;
        }
        else if (i + 1 < line.length() && (line[i + 1] == ' ' || line[i + 1] == ','))
        {
            instr = line.substr(prev + 1, i - prev);
            args = line.substr(i + 1);
            break;
        }
        else if (i == line.length() - 1)
        {
            instr = line.substr(prev + 1, i - prev + 1);
            args = "";
            break;
        }
    }
    if (instr == "")
    {
        cout << "Line " << (pc / 4 + 1) << ": Invalid Instruction" << endl;
        return make_pair(-1, flag);
    }
    if (opcode.find(instr) == opcode.end())
    {
        cout << "Line " << (pc / 4 + 1) << ": Instruction " << instr << " not found" << endl;
        return make_pair(-1, flag);
    }
    if (opcode[instr] == "0110011") // R type instructions and,xor,or,add,sub,sll,srl,sra,slt,sltu
    {
        vector<string> arguments;
        bool err;
        pair<vector<string>, bool> res = getArguments(pc / 4 + 1, 3, args, false);
        err = res.second;
        arguments = res.first;
        if (err)
        {
            return make_pair(-1, flag);
        }
        int rd, rs1, rs2;
        rd = getRegister(arguments[0], alias, pc / 4 + 1);
        rs1 = getRegister(arguments[1], alias, pc / 4 + 1);
        rs2 = getRegister(arguments[2], alias, pc / 4 + 1);
        if (rd == -1 || rs1 == -1 || rs2 == -1)
        {
            return make_pair(-1, flag);
        }

        if (checkRegister(rd, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1) || checkRegister(rs2, pc / 4 + 1))
        {
            return make_pair(-1, flag);
        }
        if (rd == 0)
        {
            return make_pair(0, flag);
        }
        registers[rd] = ALU(registers[rs1], registers[rs2], instr);
    }
    else if (opcode[instr] == "0010011") // I type instructions addi, andi, ori, xori, slti, sltiu, slli, srli, srai
    {
        vector<string> arguments;
        int count = 0;
        int index = 0;
        int prev = -1;
        int rd, rs1;
        int imm;
        arguments = getArguments(pc / 4 + 1, 3, args, false).first;
        rd = getRegister(arguments[0], alias, pc / 4 + 1);
        rs1 = getRegister(arguments[1], alias, pc / 4 + 1);
        if (rd == -1 || rs1 == -1)
            return make_pair(-1, flag);

        if (checkRegister(rd, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1))
        {
            return make_pair(-1, flag);
        }

        pair<int, bool> res;
        res = getImmediate(arguments[2], pc, label, false);

        imm = res.first;
        if (imm > 2047 || imm < -2048)
        {
            cout << "Line " << (pc / 4 + 1) << ": Value cannot be stored in 12 bits" << endl;
            return make_pair(-1, flag);
        }

        if (instr == "slli" || instr == "srli" || instr == "srai")
        {
            if (imm > 63 || imm < 0)
            {
                cout << "Line " << (pc / 4 + 1) << ": Cannot shift by " << imm << " bits" << endl;
                return make_pair(-1, flag);
            }
        }
        if (instr == "srai") // special case of srai where the 6 MSB bits are always having value 16
        {
            pair<int, bool> res = hexToInt("0x10", pc / 4 + 1);
            if (res.second)
            {
                cout << "Line " << (pc / 4 + 1) << ": Invalid immediate" << endl;
                return make_pair(-1, flag);
            }

            int imm_6_11 = res.first;

            int imm_0_5 = imm & 63;

            imm = imm_0_5 | (imm_6_11 << 6);
        }
        if (rd == 0)
        {
            return make_pair(0, flag);
        }
        registers[rd] = ALU(registers[rs1], imm, instr);
    }
    else if (opcode[instr] == "0000011" || opcode[instr] == "1100111") // Load type ld lh lw ....
    {
        vector<string> arguments;
        int count = 0;
        int index = 0;
        int prev = -1;
        arguments = getArguments(pc / 4 + 1, 3, args, true).first;
        int rd, rs1, imm;
        rd = getRegister(arguments[0], alias, pc / 4 + 1);
        rs1 = getRegister(arguments[2], alias, pc / 4 + 1);
        if (rd == -1 || rs1 == -1)
            return make_pair(-1, flag);

        if (checkRegister(rd, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1))
        {
            return make_pair(-1, flag);
        }
        pair<int, bool> res = getImmediate(arguments[1], pc, label, false);
        imm = res.first;

        if (imm > 2047 || imm < -2048)
        {
            cout << "Line: " << (pc / 4 + 1) << " Value cannot be stored in 12 bits" << endl;
            return make_pair(-1, flag);
        }

        if (instr == "jalr")
        {
            funcReturn = true;
            st.pop();
            if (rd == 0)
            {
                return make_pair(registers[rs1] + imm, true);
            }
            registers[rd] = pc + 4;
            return make_pair(registers[rs1] + imm, true);
        }
        unsigned long address = registers[rs1] + imm;
        if (address > memsize)
        {
            cout << "Line: " << (pc / 4 + 1) << " Memory address out of bounds" << endl;
            return make_pair(-1, flag);
        }

        unsigned long extracted_num = 0;
        int size = 0;
        int sign_extension = false;
        if (instr == "ld")
        {
            size = 8;
        }
        else if (instr == "lw" || instr == "lwu")
        {
            if (instr == "lw")
                sign_extension = true;
            size = 4;
        }
        else if (instr == "lh" || instr == "lhu")
        {
            if (instr == "lh")
                sign_extension = true;
            size = 2;
        }
        else
        {
            if (instr == "lb")
                sign_extension = true;
            size = 1;
        }
        if (!cacheEnabled)
        {
            for (int i = 0; i < size; i++)
            {
                extracted_num = extracted_num + (memory[address + i] << (i * 8));
            }
            if (sign_extension)
            {
                if (size == 1 && (extracted_num & 0x80))
                {
                    extracted_num = extracted_num | 0xffffffffffffff00;
                }
                else if (size == 2 && (extracted_num & 0x8000))
                {
                    extracted_num = extracted_num | 0xffffffffffff0000;
                }
                else if (size == 4 && (extracted_num & 0x80000000))
                {
                    extracted_num = extracted_num | 0xffffffff00000000;
                }
            }
            if (rd == 0)
            {
                return make_pair(0, flag);
            }
            registers[rd] = extracted_num;
            return make_pair(0, flag);
        }
        else
        {
            string binAddress = bitset<20>(address).to_string();
            // extracting the sizes of the fields
            int indexSize = (int)log2(newCache->cache_size / (newCache->block_size * newCache->associativity));
            int offset = (int)log2(newCache->block_size);
            int tagsize = 20 - offset - indexSize;
            // extracting the tag, index and offset from the address
            string tag = binAddress.substr(0, tagsize);
            string index = binAddress.substr(tagsize, indexSize);
            string offtag = binAddress.substr(indexSize + tagsize, offset);
            int idx;
            if (index == "")
            {
                idx = 0;
            }
            else
            {
                idx = stoi(index, 0, 2);
            }

            // make offset bits zero to get the base address of the block
            unsigned long baseaddress = (unsigned long)address >> offset;
            baseaddress = baseaddress << offset;
            if (address + size > newCache->block_size + baseaddress)
            {
                cout << "Unaligned Memory Access" << endl;
                return make_pair(-1, flag);
            }

            for (auto i : newCache->table[idx])
            {
                if (i->tag == tag && i->valid)
                {
                    string outputFile = fileName + ".output";

                    newCache->hits++;

                    // ofstream file(outputFile, ios::app);
                    
                    // file << "R: Address: " << hex << "0x" << address << ", Set: 0x" << idx << ", Hit, Tag: 0x" << stoul(tag, 0, 2) << ", " << (i->dirty ? "Dirty" : "Clean") << endl;
                    // file.close();

                    // updated the timer of access of the block
                    if (newCache->replacement_policy == "LRU")
                    {
                        timer++;
                        i->toa = timer;
                    }

                    if (rd == 0)
                    {
                        return make_pair(0, flag);
                    }

                    // extracting value from the block
                    unsigned long extracted_num = 0;

                    for (int k = 0; k < size; k++)
                    {
                        extracted_num = extracted_num + (i->data[stoi(offtag, 0, 2) + k] << (k * 8));
                    }
                    //cout << extracted_num << endl;
                    if (sign_extension)
                    {
                        if (size == 1 && (extracted_num & 0x80))
                        {
                            extracted_num = extracted_num | 0xffffffffffffff00;
                        }
                        else if (size == 2 && (extracted_num & 0x8000))
                        {
                            extracted_num = extracted_num | 0xffffffffffff0000;
                        }
                        else if (size == 4 && (extracted_num & 0x80000000))
                        {
                            extracted_num = extracted_num | 0xffffffff00000000;
                        }
                    }
                    registers[rd] = extracted_num;
                    // registers[rd] = i->data[stoi(offtag, 0, 2)];
                    return make_pair(0, flag);
                }
            }

            newCache->misses++;

            // search for empty block
            int emptyBlock = -1;
            for (int i = 0; i < newCache->associativity; i++)
            {
                if (!newCache->table[idx][i]->valid)
                {
                    emptyBlock = i;
                    break;
                }
            }
            //cout << "Found Empty Block: " << emptyBlock << endl;

            // if empty block not found
            if (emptyBlock == -1)
            {
                if (newCache->replacement_policy == "RANDOM")
                {
                    emptyBlock = rand() % newCache->associativity;
                }

                else if (newCache->replacement_policy == "LRU" || newCache->replacement_policy == "FIFO")
                {
                    int min = INT_MAX;
                    for (auto i : newCache->table[idx])
                    {
                        if (i->toa < min)
                        {
                            min = i->toa;
                        }
                    }
                    int it = 0;
                    for (auto i : newCache->table[idx])
                    {
                        if (i->toa == min)
                        {
                            emptyBlock = it;
                            break;
                        }
                        it++;
                    }
                }
            }
            //cout << "Found Empty Block 1: " << emptyBlock << endl;
            // updating memory if the block is dirty
            if (newCache->table[idx][emptyBlock]->valid && newCache->table[idx][emptyBlock]->dirty)
            {
                //cout << "Chosen block was Dirty" << endl;
                string offTagZeroes(offtag.length(), '0'); // number of bytes in a block
                unsigned long currBaseAddress = stoul(newCache->table[idx][emptyBlock]->tag + index + offTagZeroes, 0, 2);
                //cout << "Current Base Address: " << hex << "0x" << currBaseAddress << endl;
                for (int k = 0; k < newCache->block_size; k++)
                {
                    memory[currBaseAddress + k] = newCache->table[idx][emptyBlock]->data[k];
                }
            }

            for (int k = 0; k < newCache->block_size; k++)
            {
                newCache->table[idx][emptyBlock]->data[k] = memory[baseaddress + k];
            }

            newCache->table[idx][emptyBlock]->tag = tag;
            newCache->table[idx][emptyBlock]->valid = true;

            if (newCache->replacement_policy == "FIFO" || newCache->replacement_policy == "LRU")
            {
                timer++;
                newCache->table[idx][emptyBlock]->toa = timer;
            }

            for (auto i : newCache->table[idx])
            {
                if (i->tag == tag && i->valid)
                {
                    //cout << "mil gaya" << endl;
                    // string outputFile = fileName + ".output";
                    // ofstream file(outputFile, ios::app);
                    
                    // file << "R: Address: " << hex << "0x" << address << ", Set: 0x" << idx << ", Miss, Tag: 0x" << stoul(tag, 0, 2) << ", Clean" << endl;
                    // file.close();

                    if (rd == 0)
                    {
                        return make_pair(0, flag);
                    }

                    // extracting value from the block
                    unsigned long extracted_num = 0;

                    for (int k = 0; k < size; k++)
                    {
                        extracted_num = extracted_num + (i->data[stoi(offtag, 0, 2) + k] << (k * 8));
                    }
                    // cout << "Extracted num: " << extracted_num << endl;
                    // extending sign
                    if (sign_extension)
                    {
                        if (size == 1 && (extracted_num & 0x80))
                        {
                            extracted_num = extracted_num | 0xffffffffffffff00;
                        }
                        else if (size == 2 && (extracted_num & 0x8000))
                        {
                            extracted_num = extracted_num | 0xffffffffffff0000;
                        }
                        else if (size == 4 && (extracted_num & 0x80000000))
                        {
                            extracted_num = extracted_num | 0xffffffff00000000;
                        }
                    }
                    registers[rd] = extracted_num;
                    // registers[rd] = i->data[stoi(offtag, 0, 2)];
                    return make_pair(0, flag);
                }
            }
        }
    }
    else if (opcode[instr] == "0100011") // S type
    {
        vector<string> arguments;
        int count = 0;
        int index = 0;
        int prev = -1;
        arguments = getArguments(pc / 4 + 1, 3, args, true).first;
        int rs2, rs1, imm;

        rs2 = getRegister(arguments[0], alias, pc / 4 + 1);
        rs1 = getRegister(arguments[2], alias, pc / 4 + 1);
        if (rs2 == -1 || rs1 == -1)
            return make_pair(-1, flag);

        if (checkRegister(rs2, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1))
        {
            return make_pair(-1, flag);
        }
        pair<int, bool> res = getImmediate(arguments[1], pc, label, false);
        imm = res.first;
        if (imm > 2047 || imm < -2048)
        {
            cout << "Line: " << (pc / 4 + 1) << ": Value cannot be stored in 12 bits" << endl;
            return make_pair(-1, flag);
        }
        long num = registers[rs2];
        int size = 0;
        if (instr == "sd")
        {
            size = 8;
        }
        else if (instr == "sw")
        {
            size = 4;
        }
        else if (instr == "sh")
        {
            size = 2;
        }
        else
        {
            size = 1;
        }
        unsigned long address = registers[rs1] + imm;
        if (address < 0x10000)
        {
            cout << "Line: " << (pc / 4 + 1) << ": Segmentation Fault" << endl;
            return make_pair(-1, flag);
        }

        if (!cacheEnabled)
        {
            for (unsigned long i = 0; i < size; ++i)
            {
                memory[i + address] = (num >> (i * 8)) & 0xff; // little endian format
            }
        }
        else
        {
            string binAddress = bitset<20>(address).to_string();

            int indexSize = (int)log2(newCache->cache_size / (newCache->block_size * newCache->associativity));
            int offset = (int)log2(newCache->block_size);
            int tagsize = 20 - offset - indexSize;
            string offtag = binAddress.substr(indexSize + tagsize, offset);
            string tag = binAddress.substr(0, tagsize);
            //cout << "Tag: " << tag << endl;
            string index = binAddress.substr(tagsize, indexSize);
            //cout << "stoi test" << endl;
            int idx;
            if (index == "")
            {
                idx = 0;
            }
            else
            {
                idx = stoi(index, 0, 2);
            }

            unsigned long baseaddress = (unsigned long)address >> offset;
            baseaddress = baseaddress << offset;
            if (address + size > newCache->block_size + baseaddress)
            {
                cout << "Unaligned Memory Access" << endl;
                return make_pair(-1, flag);
            }

            for (auto i : newCache->table[idx])
            {
                if (i->tag == tag && i->valid)
                {
                    //cout << "found" << endl;
                    newCache->hits++;
                    //string outputFile = fileName + ".output";
                    //ofstream file(outputFile, ios::app);

                    if (newCache->write_back_policy == "WB")
                    {
                        //file << "W: Address: " << hex << "0x" << address << ", Set: 0x" << idx << ", Hit, Tag: 0x" << stoul(tag, 0, 2) << ", Dirty" << endl;
                    }
                    else if (newCache->write_back_policy == "WT")
                    {
                        //file << "W: Address: " << hex << "0x" << address << ", Set: 0x" << idx << ", Hit, Tag: 0x" << stoul(tag, 0, 2) << ", Clean" << endl;

                        // write through replaces the value in memory at the same timer
                        for (unsigned long k = 0; k < size; k++)
                        {
                            memory[address + k] = (num >> (k * 8)) & 0xff; // little endian format
                        }
                    }

                    //file.close();
                    if (newCache->replacement_policy == "LRU")
                    {
                        timer++;
                        i->toa = timer;
                    }

                    // change the value in the cache
                    for (int k = 0; k < size; k++)
                    {
                        i->data[stoi(offtag, 0, 2) + k] = (num >> (k * 8)) & 0xff;
                    }
                    if (newCache->write_back_policy == "WB")
                    {
                        i->dirty = true;
                    }
                    return make_pair(0, flag);
                }
            }

            //cout << "not found" << endl;

            newCache->misses++;

            // search for empty block
            int emptyBlock = -1;
            for (int i = 0; i < newCache->associativity; i++)
            {
                if (!newCache->table[idx][i]->valid)
                {
                    emptyBlock = i;
                    break;
                }
            }

            // if empty block not found
            if (emptyBlock == -1)
            {
                if (newCache->replacement_policy == "RANDOM")
                {
                    emptyBlock = rand() % newCache->associativity;
                }

                else if (newCache->replacement_policy == "LRU" || newCache->replacement_policy == "FIFO")
                {
                    int min = INT_MAX;
                    for (auto i : newCache->table[idx])
                    {
                        if (i->toa < min)
                        {
                            min = i->toa;
                        }
                    }
                    int it = 0;
                    for (auto i : newCache->table[idx])
                    {
                        if (i->toa == min)
                        {
                            emptyBlock = it;
                            break;
                        }
                        it++;
                    }
                }
            }

            if (newCache->write_back_policy == "WB") // follow write allocate policy if write back
            {
                if (newCache->table[idx][emptyBlock]->valid && newCache->table[idx][emptyBlock]->dirty)
                {
                    string offTagZeroes(offtag.length(), '0'); // number of dwords in a block
                    unsigned long currBaseAddress = stoul(newCache->table[idx][emptyBlock]->tag + index + offTagZeroes, 0, 2);
                    for (int k = 0; k < newCache->block_size; k++)
                    {
                        memory[currBaseAddress + k] = newCache->table[idx][emptyBlock]->data[k];
                    }
                }

                unsigned long baseaddress = (unsigned long)address >> (offset);
                baseaddress = baseaddress << (offset);
                for (int k = 0; k < newCache->block_size; k++)
                {
                    newCache->table[idx][emptyBlock]->data[k] = memory[baseaddress + k];
                }

                newCache->table[idx][emptyBlock]->tag = tag;
                //cout << "empty block " << emptyBlock << endl;
                newCache->table[idx][emptyBlock]->valid = true;
                newCache->table[idx][emptyBlock]->dirty = true;

                if (newCache->replacement_policy == "FIFO" || newCache->replacement_policy == "LRU")
                {
                    timer++;
                    newCache->table[idx][emptyBlock]->toa = timer;
                }

                for (auto i : newCache->table[idx])
                {
                    if (i->tag == tag && i->valid)
                    {
                        string outputFile = fileName + ".output";
                        // ofstream file(outputFile, ios::app);

                        // file << "W: Address: " << hex << "0x" << address << ", Set: 0x" << idx << ", Hit, Tag: 0x" << stoul(tag, 0, 2) << ", Dirty" << endl;

                        // file.close();

                        // put the value from the register to the cache
                        for (int k = 0; k < size; k++)
                        {
                            i->data[stoi(offtag, 0, 2) + k] = (num >> (k * 8)) & 0xff;
                        }
                    }
                }
            }
            else if (newCache->write_back_policy == "WT") // follow no write allocate policy if write through
            {

                // string outputFile = fileName + ".output";
                // ofstream file(outputFile, ios::app);
                // file << "W: Address: " << hex << "0x" << address << ", Set: 0x" << idx << ", Miss, Tag 0x" << stoul(tag, 0, 2) << ", Clean" << endl;
                for (unsigned long k = 0; k < size; k++)
                {
                    memory[address + k] = (num >> (k * 8)) & 0xff; // little endian format
                }
            }
        }
    }
    else if (opcode[instr] == "1100011") // B type beq,bge,blt,bne,bltu,bgeu
    {
        vector<string> arguments;
        bool err;
        pair<vector<string>, bool> res = getArguments(pc / 4 + 1, 3, args, false);
        err = res.second;
        arguments = res.first;
        if (err)
            return make_pair(-1, flag);
        int rs2, rs1, imm;

        rs1 = getRegister(arguments[0], alias, pc / 4 + 1);
        rs2 = getRegister(arguments[1], alias, pc / 4 + 1);
        if (rs2 == -1 || rs1 == -1)
            return make_pair(-1, flag);
        if (checkRegister(rs2, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1))
        {
            return make_pair(-1, flag);
        }

        pair<int, bool> res1 = getImmediate(arguments[2], pc, label, true);
        int curr_label = res1.first;
        if (curr_label > 4095 || curr_label < -4096)
        {
            cout << "Line: " << (pc / 4 + 1) << " value cannot be stored in 13 bits" << endl;
            return make_pair(-1, flag);
        }
        int neg = (arguments[2][0] == '-' ? 1 : 0);
        imm = curr_label;
        if (imm % 4 == 2 || imm % 4 == 3) // not a valid pc to jump to
        {
            return make_pair(-1, flag);
        }
        else if (imm % 4 == 1)
        {
            imm = imm - 1;
        }
        bool ans = false;
        if (instr == "beq" && registers[rs1] == registers[rs2])
        {
            ans = true;
        }
        else if (instr == "bne" && registers[rs1] != registers[rs2])
        {
            ans = true;
        }
        else if (instr == "blt" && registers[rs1] < registers[rs2])
        {
            ans = true;
        }
        else if (instr == "bge" && registers[rs1] >= registers[rs2])
        {
            ans = true;
        }
        else if (instr == "bltu" && (unsigned long)registers[rs1] < (unsigned long)registers[rs2])
        {
            ans = true;
        }
        else if (instr == "bgeu" && (unsigned long)registers[rs1] >= (unsigned long)registers[rs2])
        {
            ans = true;
        }
        if (ans)
        {
            // cout << "branch taken " << endl;
            return make_pair(pc + imm, true);
        }
        else
        {
            // cout << "branch not taken" << endl;
            return make_pair(0, flag);
        }
    }
    else if (opcode[instr] == "1101111") // J type jal
    {
        vector<string> arguments;
        int count = 0;
        int index = 0;
        int prev = -1;
        arguments = getArguments(pc / 4 + 1, 2, args, false).first;

        int rd, imm;

        rd = getRegister(arguments[0], alias, pc / 4 + 1);
        if (rd == -1)
            return make_pair(-1, flag);
        if (checkRegister(rd, pc / 4 + 1))
        {
            return make_pair(-1, flag);
        }
        int neg = (arguments[1][0] == '-' ? 1 : 0);
        int curr_label = getImmediate(arguments[1], pc, label, true).first;
        bool flag = getImmediate(arguments[1], pc, label, true).second;
        if (curr_label > 1048575 || curr_label < -1048576)
        {
            cout << "Line: " << (pc / 4 + 1) << " value cannot be stored in 21 bits" << endl;
            return make_pair(-1, flag);
        }
        imm = curr_label;
        if (imm % 4 == 2 || imm % 4 == 3)
        {
            return make_pair(-1, flag);
        }
        else if (imm % 4 == 1)
        {
            imm = imm - 1;
        }
        if (rd == 0)
        {
            funcCall = true;
            return make_pair(pc + imm, true);
        }
        // cout << "imm is " << imm << endl;
        // cout << "pc is " << pc << endl;
        funcCall = true;
        registers[rd] = pc + 4;
        return make_pair(pc + imm, true);
    }
    else if (opcode[instr] == "0110111") // lui
    {
        pair<vector<string>, bool> res = getArguments(pc / 4 + 1, 2, args, false);
        bool err = res.second;
        vector<string> arguments = res.first;
        if (err)
            return make_pair(-1, flag);
        int rd;
        int imm;

        rd = getRegister(arguments[0], alias, pc / 4 + 1);
        if (rd == -1)
            return make_pair(-1, flag);
        if (checkRegister(rd, pc / 4 + 1))
        {
            return make_pair(-1, flag);
        }

        string val = arguments[1];

        if (val[0] == '-')
        {
            cout << "Line: " << (pc / 4 + 1) << " value cannot be negative" << endl;
            return make_pair(-1, flag);
        }
        size_t i = 0;

        if (val[0] == '0' && val[1] == 'x')
        {
            imm = stoi(val, &i, 16);
        }
        else
        {
            imm = stoi(val, &i, 10);
        }
        if (i != val.length())
        {
            cout << "Line " << (pc / 4 + 1) << " : Wrong immediate value " << endl;
            return make_pair(-1, flag);
        }
        if (imm >> 31 > 0)
        {
            cout << "Line " << (pc / 4 + 1) << " Immediate value cannot be stored in 20 bits" << endl;
            return make_pair(-1, flag);
        }
        long int imm2 = (long)imm;
        long int temp = (long)imm & 0x00000000000fffff;

        if (imm2 & 0x80000)
        {
            temp = temp | 0xfffffffffff00000;
        }
        long int ans = 0;
        ans = temp << 12;

        if (rd == 0)
        {
            return make_pair(0, flag);
        }
        // cout << "value stored is " << ans << endl;
        // cout << "value stored is " << decToHex(ans) << endl;
        registers[rd] = ans;
    }
    return make_pair(0, flag);
}

/*
    Initialises the memory with 0
*/
void initialiseMemory()
{
    for (int i = 0; i < memsize; i++)
    {
        memory[i] = 0;
    }
}

/*
    Initialises the registers with 0
*/
void setup()
{
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0;
    }
    initialiseMemory();
}

/*
    Prints the registers
*/
void printRegs()
{
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        string hex = decToHex(registers[i]);
        cout << "0x" << hex << endl;
    }
    for (; i < 32; i++)
    {
        string hex = decToHex(registers[i]);
        cout << "0x" << hex << endl;
    }
}

/*
    Performs all the tasks before starting the execution
*/
bool loadProgram(string file)
{
    mainPC = 0;
    // cleaning up
    lines.clear();
    setup();
    while (!st.empty())
    {
        st.pop();
    }
    st.push(pair<string, int>("main", 0));
    breakpoints.clear();
    label.clear();
    comments.clear();
    inverseLabel.clear();
    pair<bool, int> res = loadFile(file);
    if (!res.first)
        return false;
    initialiseMaps();
    if (!getLabels(file))
        return false;
    getComments(file);
    memLines = res.second;
    return true;
}

/*
    Runs the entire code starting from the current PC
*/
void run(bool toPrint,bool cacheEnabled, cache* newCache)
{
    int numLines = lines.size();
    if (mainPC / 4 >= numLines)
    {

        return;
    }
    while ((mainPC / 4) < numLines && mainPC >= 0)
    {
        string line = lines[mainPC / 4].second;
        if (line[0] == '\0')
        {
            mainPC += 4;
            continue;
        }
        pair<int, bool> ans = convert(lines[mainPC / 4].second, mainPC, false, cacheEnabled,newCache);
        int res = ans.first;
        bool flag = ans.second;
        if (res == -2) // -2: breakpoint, -1, 0: normal
        {
            return;
        }
        else if (res == -1)
        {
            while (!st.empty())
            {
                st.pop();
            }
            return;
        }
        else if (res != 0 || flag)
        {
            string hexPC = decToHex(mainPC);
            pair<string, int> temp(st.top().first, mainPC / 4 + 1 + memLines);
            mainPC = res;
            if (funcReturn)
            {
                funcReturn = false;
                continue;
            }
            st.pop();
            st.push(temp);

            if (funcCall)
            {
                funcCall = false;
                st.push(pair<string, int>(inverseLabel[mainPC], mainPC / 4 + memLines));
            }
        }
        else
        {
            pair<string, int> temp(st.top().first, mainPC / 4 + 1 + memLines);
            st.pop();
            st.push(temp);
            string hexPC = decToHex(mainPC);
            mainPC += 4;
        }
    }

    if (toPrint)
    {
        cout << "Code executed successfully" << endl;
        printRegs();
        cout << endl;
        printMem(0x10000, 1);
        printCacheRes(newCache);
    }

    while (!st.empty())
    {
        st.pop();
    }
}

/*
    Step by step execution after the execution is stopped by breakpoint or from the start itself
*/
void step(bool toPrint,bool cacheEnabled, cache* newCache)
{
    if (mainPC / 4 >= lines.size())
    {
        while (!st.empty())
        {
            st.pop();
        }
        return;
    }
    string line = lines[mainPC / 4].second;
    if (line[0] == '\0')
    {
        mainPC += 4;

        if (toPrint)
        {
            cout << "Encountered Empty Line" << endl;
            printRegs();
            cout << endl;
            printMem(0x10000, 1);
            printCacheRes(newCache);
        }
        return;
    }
    pair<int, bool> ans = convert(lines[mainPC / 4].second, mainPC, true,cacheEnabled,newCache);
    int res = ans.first;
    bool flag = ans.second;
    if (res == -2) // -2: breakpoint, -1, 0: normal
    {
        return;
    }
    else if (res == -1)
    {
        while (!st.empty())
        {
            st.pop();
        }
        return;
    }
    else if (res != 0 || flag) // if branch or jump
    {
        string hexPC = decToHex(mainPC);
        if (toPrint)
            cout << "Executed " << line.substr(labelIndex[mainPC]) << endl; // "; PC = " << "0x" + addZeroes(hexPC, 8) << endl;
        pair<string, int> temp(st.top().first, mainPC / 4 + 1 + memLines);
        mainPC = res;
        if (funcReturn)
        {
            funcReturn = false;
            return;
        }
        st.pop();
        st.push(temp);

        if (funcCall)
        {
            funcCall = false;
            st.push(pair<string, int>(inverseLabel[mainPC], mainPC / 4 + memLines));
        }
    }
    else
    {

        pair<string, int> temp(st.top().first, mainPC / 4 + 1 + memLines);
        st.pop();
        st.push(temp);
        string hexPC = decToHex(mainPC);
        if (toPrint)
            cout << "Executed " << line.substr(labelIndex[mainPC]) << endl; //<< "; PC = " << "0x" + addZeroes(hexPC, 8) << endl;
        mainPC += 4;
    }

    if (toPrint)
    {
        printRegs();
        cout << endl;
        printMem(0x10000, 1);
        printCacheRes(newCache);
    }
}

void updateStatus(int pc, bool cacheEnabled, cache* newCache)
{

    int stepsRequired = pc / 4;

    for (int i = 0; i < stepsRequired; ++i)
    {
        step(false,cacheEnabled,newCache );
    }
}

/*
    Prints memory from the given address and count
*/
void printMem(unsigned long address, int count)
{
    for (int i = 0; i < 0x3ff; i++)
    {
        
        cout << "0x" << hex << (unsigned long)(memory[address + i]) << endl;
    }
}

/*
    Adds breakpoint at the given line
*/
void addBreakpoint(int line)
{
    breakpoints[line] = 1;
    cout << "Breakpoint set at line " << line << endl;
}

/*
    Removes breakpoint at the given line
*/
void removeBreakpoint(int line)
{
    breakpoints[line] = 0; // 0: no breakpoint, 1: breakpoint
}

/*
    Prints the call stack
*/
void showStack()
{
    stack<pair<string, int> > stTemp = st;
    stack<pair<string, int> > stTemp1;
    if (st.empty())
    {
        cout << "Empty Call Stack: Execution complete" << endl;
        return;
    }
    while (!stTemp.empty()) // reverses the stack to print it as required
    {
        stTemp1.push(stTemp.top());
        stTemp.pop();
    }
    cout << "Call Stack:" << endl;
    while (!stTemp1.empty())
    {
        cout << stTemp1.top().first << ":" << stTemp1.top().second << endl;
        stTemp1.pop();
    }
}


void printCacheRes(cache* newCache){
    cout << newCache->hits << endl;
    cout << newCache->misses << endl;
}
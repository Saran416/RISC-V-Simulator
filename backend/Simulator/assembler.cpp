#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <stack>
#include "assembler.h"
using namespace std;

/*
    A function to print the vector of strings which helps in logging the messages and errors
*/
void printVector(vector<string> v)
{

    cout << "printing vector" << endl;
    for (int i = 0; i < v.size(); i++)
    {
        cout << i << "th " << v[i] << endl;
    }
}

/*
    A function to check if a string can be converted to integer or not
*/
bool safeStoi(string s, int line, int base)
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
        cout << "Line " << line << ": Invalid number " << s << " cannot be stored in 32 bits" << endl;
        exit(1);
    }
}

/*
    A function to convert binary string to hexadecimal string
    It is used in generating the final hexcode answer from the combined binary
    string.
*/
string binToHex(string bin)
{
    string ans = "";
    for (int i = 0; i < bin.length(); i += 4)
    {
        string curr = bin.substr(i, 4);
        int dec = stoi(curr, 0, 2);
        if (dec < 10)
        {
            ans += to_string(dec);
        }
        else
        {
            ans += (char)(97 + dec - 10);
        }
    }
    return ans;
}

/*
    A function to convert hexadecimal string to integer also
    considering the safe conversion and returning an error if
    the conversion is not possible
*/
int hexToInt(string hex, int line)
{
    if (safeStoi(hex, line, 16))
    {
        return stoi(hex, 0, 16);
    }
    else
    {
        return 0;
    }
}

/*
    A function to convert binary string to integer also
    considering the safe conversion and returning an error if
    the conversion is not possible
*/
int binToInt(string bin, int line)
{
    if (safeStoi(bin, line, 2))
    {
        return stoi(bin, 0, 2);
    }
    else
    {
        return 0;
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
        if (safeStoi(reg.substr(1), line))
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
        if (alias.find(reg) != alias.end() && safeStoi(alias[reg].substr(1), line))
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
                    cout << "Line " << line << " :mismanaged brackets" << endl;
                    exit(1);
                }
                if (st.size() > 1)
                {
                    cout << "Line " << line << ": Too many brackets, only one set allowed around one argument" << endl;
                    exit(1);
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
                cout << "Line " << line << " : mismatching brackets" << endl;
                exit(1);
            }
            if (st.size() > 1)
            {
                cout << "Line " << line << " : Too many brackets, only one set allowed around one argument" << endl;
                exit(1);
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
            exit(1);
        }
        st.pop();
        stackOpcounter++;
    }
    while (index < args.length() && (args[index] == ' ' || args[index] == ','))
        index++;
    if (st.size() != 0)
    {
        cout << "Line " << line << ": mismatching brackets" << endl;
        exit(1);
    }
    if (arguments.size() != count)
    {
        cout << "Line " << line << ": Less arguments than required" << endl;
        exit(1);
    }
    else if (arguments.size() == count && index < args.length())
    {
        cout << "Line " << line << ": Extra arguments" << endl;
        exit(1);
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
    int line = pc / 4 + 1;
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
            cout << "Line " << (line) << ": label not found" << endl;
            exit(1);
        }
    }
    else if ((str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) || (neg && str[1] == '0' && (str[2] == 'x' || str[2] == 'X')))
    {
        imm = hexToInt(str, line);
    }
    else if ((str[0] == '0' && (str[1] == 'b' || str[1] == 'B')) || (neg && str[1] == '0' && (str[2] == 'b' || str[2] == 'B')))
    {
        imm = binToInt(str, line);
    }
    else if (safeStoi(str, line))
    {
        imm = stoi(str);
    }
    else
    {
        exit(1);
    }
    return make_pair(imm, false);
}

int convert()
{
    ifstream input("input.s");
    ofstream output("output.hex");
    string line;

    unordered_map<string, string> opcode;
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

    unordered_map<string, string> funct3;
    funct3["add"] = "000";
    funct3["sub"] = "000";
    funct3["and"] = "111";
    funct3["or"] = "110";
    funct3["xor"] = "100";
    funct3["sll"] = "001";
    funct3["srl"] = "101";
    funct3["sra"] = "101";
    funct3["addi"] = "000";
    funct3["andi"] = "111";
    funct3["ori"] = "110";
    funct3["xori"] = "100";
    funct3["slli"] = "001";
    funct3["srli"] = "101";
    funct3["srai"] = "101";
    funct3["lw"] = "010";
    funct3["sw"] = "010";
    funct3["beq"] = "000";
    funct3["bne"] = "001";
    funct3["blt"] = "100";
    funct3["bge"] = "101";
    funct3["bltu"] = "110";
    funct3["bgeu"] = "111";
    funct3["jalr"] = "000";
    funct3["ld"] = "011";
    funct3["sd"] = "011";
    funct3["sh"] = "001";
    funct3["sb"] = "000";
    funct3["lh"] = "001";
    funct3["lb"] = "000";
    funct3["lbu"] = "100";
    funct3["lhu"] = "101";
    funct3["slt"] = "010";
    funct3["sltu"] = "011";
    funct3["slti"] = "010";
    funct3["sltiu"] = "011";
    funct3["lwu"] = "110";

    unordered_map<string, string> funct7;
    funct7["add"] = "0000000";
    funct7["sub"] = "0100000";
    funct7["and"] = "0000000";
    funct7["or"] = "0000000";
    funct7["xor"] = "0000000";
    funct7["sll"] = "0000000";
    funct7["srl"] = "0000000";
    funct7["sra"] = "0100000";
    funct7["slt"] = "0000000";
    funct7["sltu"] = "0000000";

    unordered_map<string, string> alias;
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
    
    int pc = 0;   // program counter
    unordered_map<int, int> comments; // stores the pc and number and index where the comment starts
    while (getline(input, line))  // storing starting point of comments
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
    input.open("input.s");

    pc = 0;

    unordered_map<string, int> label; // stores all the labels and their corresponding pc values
    while (getline(input, line))
    {
        string curr_label = "";
        if (line[0] == ';')
        {
            continue;
        }
        if (comments.find(pc) != comments.end())
        {
            line = line.substr(0, comments[pc]);
        }
        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == ':')
            {
                curr_label = line.substr(0, i);
                break;
            }
        }
        if (curr_label != "")
            label[curr_label] = pc;
        pc += 4;
    }

    input.close();
    input.open("input.s"); // close file and again open it to move to beginning of it
    pc = 0;                // restarting from beginning
    while (getline(input, line))
    {
        string ans = "";
        if (line[0] == ';')
        { // starting with semicolon is treated as a comment
            continue;
        }
        if (comments.find(pc) != comments.end())
        {
            line = line.substr(0, comments[pc]);
        }
        string instr = "";
        string args = "";
        int prev = -1;
        for (int i = 0; i < line.length(); i++) // extracting instruction and arguments
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
            continue;
        }
        if (opcode.find(instr) == opcode.end())
        {
            cout << "instr is " << instr << endl;
            cout << "Line " << (pc / 4 + 1) << ": instruction " << instr << " not found" << endl;
            break;
        }
        if (opcode[instr] == "0110011") // R type instructions and,xor,or,add,sub,sll,srl,sra,slt,sltu
        {
            vector<string> registers;
            bool err;
            pair<vector<string>, bool> res = getArguments(pc / 4 + 1, 3, args, false);
            err = res.second;
            registers = res.first;
            if (err)
            {
                break;
            }
            int rd, rs1, rs2;
            rd = getRegister(registers[0], alias, pc / 4 + 1);
            rs1 = getRegister(registers[1], alias, pc / 4 + 1);
            rs2 = getRegister(registers[2], alias, pc / 4 + 1);
            if (rd == -1 || rs1 == -1 || rs2 == -1)
            {
                break;
            }

            if (checkRegister(rd, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1) || checkRegister(rs2, pc / 4 + 1))
            {
                break;
            }
            ans = funct7[instr] + bitset<5>(rs2).to_string() + bitset<5>(rs1).to_string() + funct3[instr] + bitset<5>(rd).to_string() + opcode[instr];
        }
        else if (opcode[instr] == "0010011") // I type instructions addi, andi, ori, xori, slti, sltiu, slli, srli, srai, jalr
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
                break;

            if (checkRegister(rd, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1))
            {
                break;
            }

            pair<int, bool> res;

            res = getImmediate(arguments[2], pc, label, false);

            imm = res.first;
            if (imm > 2047 || imm < -2048)
            {
                cout << "Line " << (pc / 4 + 1) << " :value cannot be stored in 12 bits" << endl;
                return 0;
            }

            if (instr == "slli" || instr == "srli" || instr == "srai")
            {
                if (imm > 63 || imm < 0)
                {
                    cout << "Line " << (pc / 4 + 1) << " :Cannot shift by " << imm << " bits" << endl;
                    return 0;
                }
            }
            if (instr == "srai") // special case of srai where the 6 MSB bits are always having value 16
            {
                int imm_6_11 = hexToInt("0x10", pc / 4 + 1);

                int imm_0_5 = imm & 63;

                imm = imm_0_5 | (imm_6_11 << 6);
            }
            ans = bitset<12>(imm).to_string() + bitset<5>(rs1).to_string() + funct3[instr] + bitset<5>(rd).to_string() + opcode[instr];
        }
        else if (opcode[instr] == "0000011" || opcode[instr] == "1100111") // I Load type ld lh lw ....
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
                break;

            if (checkRegister(rd, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1))
            {
                break;
            }
            pair<int, bool> res = getImmediate(arguments[1], pc, label, false);
            imm = res.first;

            if (imm > 2047 || imm < -2048)
            {
                cout << "Line: " << (pc / 4 + 1) << " Value cannot be stored in 12 bits" << endl;
                return 0;
            }

            ans = bitset<12>(imm).to_string() + bitset<5>(rs1).to_string() + funct3[instr] + bitset<5>(rd).to_string() + opcode[instr];
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
                break;

            if (checkRegister(rs2, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1))
            {
                break;
            }
            pair<int, bool> res = getImmediate(arguments[1], pc, label, false);
            imm = res.first;
            if (imm > 2047 || imm < -2048)
            {
                cout << "Line: " << (pc / 4 + 1) << "  Value cannot be stored in 12 bits" << endl;
                return 0;
            }
            int imm_5_11 = imm >> 5;
            int imm_0_4 = imm & 31;

            ans = bitset<7>(imm_5_11).to_string() + bitset<5>(rs2).to_string() + bitset<5>(rs1).to_string() + funct3[instr] + bitset<5>(imm_0_4).to_string() + opcode[instr];
        }
        else if (opcode[instr] == "1100011") // B type beq,bge,blt,bne,bltu,bgeu
        {
            vector<string> arguments;
            bool err;
            pair<vector<string>, bool> res = getArguments(pc / 4 + 1, 3, args, false);
            err = res.second;
            arguments = res.first;
            if (err)
                break;
            int rs2, rs1, imm;

            rs1 = getRegister(arguments[0], alias, pc / 4 + 1);
            rs2 = getRegister(arguments[1], alias, pc / 4 + 1);
            if (rs2 == -1 || rs1 == -1)
                break;
            if (checkRegister(rs2, pc / 4 + 1) || checkRegister(rs1, pc / 4 + 1))
            {
                break;
            }

            pair<int, bool> res1 = getImmediate(arguments[2], pc, label, true);
            int curr_label = res1.first;
            if (curr_label > 4095 || curr_label < -4096)
            {
                cout << "Line: " << (pc / 4 + 1) << " value cannot be stored in 13 bits" << endl;
                return 0;
            }
            int neg = (arguments[2][0] == '-' ? 1 : 0);
            if (res1.second)
            {
                imm = curr_label / 4 * 2;
            }
            else
            {
                imm = (curr_label + (neg ? -1 : 0)) / 2;
            }
            int imm_5_12 = imm >> 4;
            int imm_12 = imm >> 11;
            int imm_5_11 = imm_5_12 & 127;
            int imm_11 = imm_5_11 >> 6;
            int imm_5_10 = imm_5_11 & 63;
            int imm_1_4 = imm & 15;
            ans = bitset<1>(imm_12).to_string() + bitset<6>(imm_5_10).to_string() + bitset<5>(rs2).to_string() + bitset<5>(rs1).to_string() + funct3[instr] + bitset<4>(imm_1_4).to_string() + bitset<1>(imm_11).to_string() + opcode[instr];
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
                break;
            if (checkRegister(rd, pc / 4 + 1))
            {
                break;
            }
            int neg = (arguments[1][0] == '-' ? 1 : 0);
            int curr_label = getImmediate(arguments[1], pc, label, true).first;
            bool flag = getImmediate(arguments[1], pc, label, true).second;
            if (curr_label > 1048575 || curr_label < -1048576)
            {
                cout << "Line: " << (pc / 4 + 1) << " value cannot be stored in 21 bits" << endl;
                return 0;
            }
            if (flag)
                imm = (curr_label / 4) * 2;
            else
                imm = (curr_label + (neg ? -1 : 0)) / 2;

            int imm_20 = imm >> 19;
            int imm_10_1 = imm & 1023;
            int imm_11 = (imm >> 10) & 1;
            int imm_19_12 = (imm >> 11) & 255;
            ans = bitset<1>(imm_20).to_string() + bitset<10>(imm_10_1).to_string() + bitset<1>(imm_11).to_string() + bitset<8>(imm_19_12).to_string() + bitset<5>(rd).to_string() + opcode[instr];
        }
        else if (opcode[instr] == "0110111") // lui
        {
            pair<vector<string>, bool> res = getArguments(pc / 4 + 1, 2, args, false);
            bool err = res.second;
            vector<string> arguments = res.first;
            if (err)
                break;
            int rd;
            long long imm;

            rd = getRegister(arguments[0], alias, pc / 4 + 1);
            if (rd == -1)
                break;
            if (checkRegister(rd, pc / 4 + 1))
            {
                break;
            }

            string val = arguments[1];
            if (val[0] == '-')
            {
                cout << "Line: " << (pc / 4 + 1) << " value cannot be negative" << endl;
                return 0;
            }
            size_t i = 0;

            if (val[0] == '0' && val[1] == 'x')
            {
                imm = stoll(val, &i, 16);
            }
            else
            {
                imm = stoll(val, &i, 10);
            }
            if (i != val.length())
            {
                cout << "Line " << (pc / 4 + 1) << " : Wrong immediate value " << endl;
                return 0;
            }
            if (imm > 4294967295) // larger than the allowed limit of int
            {
                cout << "Line: " << (pc / 4 + 1) << " value cannot be stored in 32 bits" << endl;
                return 0;
            }
            int imm_12_31;
            if ((imm >> 31) > 0)
            {
                imm_12_31 = imm >> 12;
            }
            else
            {
                imm_12_31 = imm;
            }

            ans = bitset<20>(imm_12_31).to_string() + bitset<5>(rd).to_string() + opcode[instr];
        }
        else if (opcode[instr] == "0010111") // auipc
        {
            vector<string> arguments;
            int count = 0;
            int index = 0;
            int prev = -1;
            arguments = getArguments(pc / 4 + 1, 2, args, false).first;
            int rd, imm;

            rd = getRegister(arguments[0], alias, pc / 4 + 1);
            if (rd == -1)
                break;
            if (checkRegister(rd, pc / 4 + 1))
            {
                break;
            }
            imm = getImmediate(arguments[1], pc, label, false).first;

            int imm_12_31;
            if ((imm >> 31) > 0)
            {
                imm_12_31 = imm >> 12;
            }
            else
            {
                imm_12_31 = imm;
            }
            
            ans = bitset<20>(imm_12_31).to_string() + bitset<5>(rd).to_string() + opcode[instr];
        }
        cout << binToHex(ans) << endl;
        pc += 4;
    }

    input.close();
    return 0;
}

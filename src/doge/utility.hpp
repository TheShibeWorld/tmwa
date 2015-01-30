#ifndef DOGE_UTILITY
#define DOGE_UTILITY
#include <iostream>
#include <fstream>
#include <string.h>
#include "../strings/mstring.hpp"
#include <algorithm>
#include <string>

using namespace std;
using namespace tmwa;
using namespace strings;

std::string toString(int number);
MString toMString(std::string str);
MString toMString(int num);

////////////////////////////// FILES
void readIntFromFile(char* file_path, int* output);
void readStringFromFile(char* file_path, char* output);
void writeIntToFile(char* file_path, int number);
void writeStringToFile(char* file_path, char* str);

///////////////////////////////DOGE
string dogeDeposit(string character);
string dogeBalance(string character);
string dogeWithdraw(string character, int amount, string address);
string dogeTip(string character, int amount, string character_to);
#endif

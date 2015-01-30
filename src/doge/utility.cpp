#include "utility.hpp"

std::string toString(int number)
{
    if (number == 0)
        return "0";
    std::string temp="";
    std::string returnvalue="";
    while (number>0)
    {
        temp+=number%10+48;
        number/=10;
    }
    for (int i=0; i<(int)temp.length(); i++)
        returnvalue+=temp[temp.length()-i-1];
    return returnvalue;
}
MString toMString(std::string str)
{
    MString return_value;
    for (int i=0; i<(int)str.length(); i++)
        return_value+=str[i];
    return return_value;
}
MString toMString(int num)
{
    return toMString(toString(num));
}

////////////////////////////// FILES
void readIntFromFile(char* file_path, int* output)
{
    std::ifstream in(file_path);
    if(!in.is_open())
    {
        *output=0;
        return;
    }
    in>>*output;
    in.close();
}
void readStringFromFile(char* file_path, char* output)
{
    std::ifstream address_file(file_path);
    address_file>>output;
}
void writeIntToFile(char* file_path, int number)
{
    std::ofstream out(file_path);
    out<<number;
    out.close();
}
void writeStringToFile(char* file_path, char* str)
{
    std::ofstream out(file_path);
    out<<str;
    out.close();
}

///////////////////////////////DOGE
string dogeDeposit(string character)
{
    int system_return=-1;
    std::transform(character.begin(), character.end(), character.begin(), ::tolower);
    char doge_command_rm[255];
    strcpy(doge_command_rm,"rm doge/");
    strcat(doge_command_rm,character.c_str());
    strcat(doge_command_rm,"_address");
    system_return = system(doge_command_rm);
    char doge_command[255];
    strcpy(doge_command,"./dogecoind getaccountaddress tmwplayer_");
    strcat(doge_command,character.c_str());
    strcat(doge_command," >> doge/");
    strcat(doge_command,character.c_str());
    strcat(doge_command,"_address");
    system_return = system(doge_command);
    if(system_return!=0)
        return "shibe sorry, dogecoin server down";
    char file_path[255];
    strcpy(file_path,"doge/");
    strcat(file_path,character.c_str());
    strcat(file_path,"_address");
    char doge_address[40];
    readStringFromFile(file_path,doge_address);
    return doge_address;
}
string dogeBalance(string character)
{
    int system_return=-1;
    std::transform(character.begin(), character.end(), character.begin(), ::tolower);
    char doge_command_rm[255];
    strcpy(doge_command_rm,"rm doge/");
    strcat(doge_command_rm,character.c_str());
    strcat(doge_command_rm,"_deposits");
    system_return = system(doge_command_rm);
    char doge_command[255];
    strcpy(doge_command,"./dogecoind getreceivedbyaccount tmwplayer_");
    strcat(doge_command,character.c_str());
    strcat(doge_command," >> doge/");
    strcat(doge_command,character.c_str());
    strcat(doge_command,"_deposits");
    system_return = system(doge_command);
    if(system_return!=0)
        return "shibe sorry, dogecoin server down";
    char file_path[255];
    strcpy(file_path,"doge/");
    strcat(file_path,character.c_str());
    strcat(file_path,"_balance");
    int balance;
    readIntFromFile(file_path,&balance);
    char file_path_deposits[255];
    strcpy(file_path_deposits,"doge/");
    strcat(file_path_deposits,character.c_str());
    strcat(file_path_deposits,"_deposits");
    int deposits;
    readIntFromFile(file_path_deposits,&deposits);
    return toString(balance+deposits)+"Ð";
}
string dogeWithdraw(string character, int amount, string address)
{
    int system_return=-1;
    std::transform(character.begin(), character.end(), character.begin(), ::tolower);
    if(amount<=0)
    {
        return "naughty shibe";
    }
//Deposits
    char doge_command_rm[255];
    strcpy(doge_command_rm,"rm doge/");
    strcat(doge_command_rm,character.c_str());
    strcat(doge_command_rm,"_deposits");
    system_return = system(doge_command_rm);
    char doge_command[255];
    strcpy(doge_command,"./dogecoind getreceivedbyaccount tmwplayer_");
    strcat(doge_command,character.c_str());
    strcat(doge_command," >> doge/");
    strcat(doge_command,character.c_str());
    strcat(doge_command,"_deposits");
    system_return = system(doge_command);
    if(system_return!=0)
        return "shibe sorry, dogecoin server down";
    char file_path_deposits[255];
    strcpy(file_path_deposits,"doge/");
    strcat(file_path_deposits,character.c_str());
    strcat(file_path_deposits,"_deposits");
    int deposits;
    readIntFromFile(file_path_deposits,&deposits);
//Balance
    char file_path[255];
    strcpy(file_path,"doge/");
    strcat(file_path,character.c_str());
    strcat(file_path,"_balance");
    int balance;
    readIntFromFile(file_path,&balance);
    int total = deposits + balance;
    total-=(amount+1);//-1 for tax fee
    if(total<0)
        return "You have not enough doges";
    char doge_command_send[255];
    strcpy(doge_command_send,"./dogecoind sendtoaddress ");
    strcat(doge_command_send, address.c_str());
    strcat(doge_command_send, " ");
    strcat(doge_command_send, toString(amount).c_str());
    system_return = system(doge_command_send);
    if(system_return!=0)
        return "shibe sorry, dogecoin server down";
    writeIntToFile(file_path, balance-amount-1);
    return toString(amount) + "Ð sent to " + address;
}
string dogeTip(string character, int amount, string character_to)
{
    int system_return=-1;
    std::transform(character.begin(), character.end(), character.begin(), ::tolower);
    std::transform(character_to.begin(), character_to.end(), character_to.begin(), ::tolower);
    if(amount<0)
    {
        return "naughty shibe";
    }
    char doge_command_rm[255];
    strcpy(doge_command_rm,"rm doge/");
    strcat(doge_command_rm,character.c_str());
    strcat(doge_command_rm,"_deposits");
    system_return = system(doge_command_rm);
    char doge_command[255];
    strcpy(doge_command,"./dogecoind getreceivedbyaccount tmwplayer_");
    strcat(doge_command,character.c_str());
    strcat(doge_command," >> doge/");
    strcat(doge_command,character.c_str());
    strcat(doge_command,"_deposits");
    system_return = system(doge_command);
    if(system_return!=0)
        return "shibe sorry, dogecoin server down";
    char file_path_deposits[255];
    strcpy(file_path_deposits,"doge/");
    strcat(file_path_deposits,character.c_str());
    strcat(file_path_deposits,"_deposits");
    int deposits;
    readIntFromFile(file_path_deposits,&deposits);
//Balance
    char file_path[255];
    strcpy(file_path,"doge/");
    strcat(file_path,character.c_str());
    strcat(file_path,"_balance");
    int balance;
    readIntFromFile(file_path,&balance);
    int total = balance + deposits;
    total-=amount;
    if(total<0)
    {
        return "You have not enough doges";
    }
    writeIntToFile(file_path, balance-amount);
//Receiver
    char file_path_receiver[255];
    strcpy(file_path_receiver,"doge/");
    strcat(file_path_receiver,character_to.c_str());
    strcat(file_path_receiver,"_balance");
    int balance_receiver;
    readIntFromFile(file_path_receiver,&balance_receiver);
    balance_receiver+=amount;
    writeIntToFile(file_path_receiver, balance_receiver);
    return toString(amount) + "Ð sent to " + character_to;
}

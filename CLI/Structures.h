#pragma once
#include <vector>
#include <string>
using namespace std;

struct ParsedCommand {
    string argument;
    bool isPath = true;
    string command;
    string option;
    string error;
};